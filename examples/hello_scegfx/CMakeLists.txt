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

file(GLOB SHADERS hello_scegfx.vert.glsl hello_scegfx.frag.glsl)

mark_for_spirv_compilation(SHADERS ${SHADERS})

add_executable(hello_scegfx hello_scegfx.c ${SHADERS})

if(EMSCRIPTEN)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_SDL=2 --emrun")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  add_compile_definitions(hello_scegfx PUBLIC EMSCRIPTEN=1)
  foreach(FILE ${SHADERS})
    get_filename_component(FILE_BASENAME "${FILE}" NAME)
    set(
      PRELOAD_FILE_STR
      "${PRELOAD_FILE_STR} --preload-file ${FILE_BASENAME}.spv"
    )
  endforeach()
  set(
    CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} --emrun ${PRELOAD_FILE_STR}"
  )
else()
  find_package(SDL2 REQUIRED)
  set(SDL_LIBRARIES SDL2::SDL2)
endif()

target_link_libraries(hello_scegfx PRIVATE ${SDL_LIBRARIES} scegfx)
