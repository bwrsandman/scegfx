find_program(GLSLANG_VALIDATOR NAMES glslangValidator REQUIRED)
function(mark_for_spirv_compilation)
  set(options "")
  set(oneValueArgs "")
  set(multiValueArgs SHADERS BRIDGING_HEADERS)
  cmake_parse_arguments(
    SPIRV_COMPILATION
      "${options}"
      "${oneValueArgs}"
      "${multiValueArgs}"
      "${ARGN}"
  )
  foreach(SHADER_FILE ${SPIRV_COMPILATION_SHADERS})
    get_filename_component(SHADER_FILE_BASENAME ${SHADER_FILE} NAME)
    get_filename_component(SHADER_FILE_ABSOLUTE ${SHADER_FILE} ABSOLUTE)
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_FILE_BASENAME}.spv
      COMMAND
        ${GLSLANG_VALIDATOR}
        -V
        ${SHADER_FILE_ABSOLUTE}
        "$<$<CONFIG:debug>:-g>$<$<CONFIG:relwithdebinfo>:-g>"
        -o
        ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_FILE_BASENAME}.spv
      DEPENDS ${SPIRV_COMPILATION_BRIDGING_HEADERS}
      MAIN_DEPENDENCY ${SHADER_FILE_ABSOLUTE}
    )
  endforeach()
endfunction()

function(mark_for_copy)
  set(options "")
  set(oneValueArgs "")
  set(multiValueArgs "")
  cmake_parse_arguments(
    FILE_COPY
      "${options}"
      "${oneValueArgs}"
      "${multiValueArgs}"
      "${ARGN}"
  )
  foreach(FILE ${ARGN})
    get_filename_component(FILE_BASENAME ${FILE} NAME)
    get_filename_component(FILE_ABSOLUTE ${FILE} ABSOLUTE)
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${FILE_BASENAME}
      COMMAND
        ${CMAKE_COMMAND}
        -E
        copy
        ${FILE_ABSOLUTE}
        ${CMAKE_CURRENT_BINARY_DIR}/${FILE_BASENAME}
      MAIN_DEPENDENCY ${FILE_ABSOLUTE}
    )
  endforeach()
endfunction()

file(GLOB SHADERS hello_scegfx.vert.glsl hello_scegfx.frag.glsl)
file(GLOB IMAGES opengl-logo.png vulkan-logo.png webgl-logo.png)

mark_for_spirv_compilation(
  SHADERS ${SHADERS} BRIDGING_HEADERS bridging_header.h
)
mark_for_copy(${IMAGES})

add_executable(
  hello_scegfx
  hello_scegfx.c
  ${SHADERS}
  bridging_header.h
  ${IMAGES}
)
target_compile_definitions(hello_scegfx PRIVATE SCEGFX_C_SOURCE)

if(EMSCRIPTEN)
  set(
    CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='[\"png\"]' --emrun"
  )
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  add_compile_definitions(hello_scegfx PUBLIC EMSCRIPTEN=1)
  foreach(FILE ${SHADERS})
    get_filename_component(FILE_BASENAME "${FILE}" NAME)
    set(
      PRELOAD_FILE_STR
      "${PRELOAD_FILE_STR} --preload-file ${FILE_BASENAME}.spv"
    )
  endforeach()
  foreach(FILE ${IMAGES})
    get_filename_component(FILE_BASENAME "${FILE}" NAME)
    set(PRELOAD_FILE_STR "${PRELOAD_FILE_STR} --preload-file ${FILE_BASENAME}")
  endforeach()
  set(
    CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} --emrun ${PRELOAD_FILE_STR}"
  )
else()
  find_package(SDL2 REQUIRED)
  include(FindPkgConfig)
  pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image>=2.0.0)
  set(SDL_LIBRARIES SDL2::SDL2 ${SDL2_IMAGE_LIBRARIES})
endif()

target_link_libraries(hello_scegfx PRIVATE ${SDL_LIBRARIES} scegfx)
