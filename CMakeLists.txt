cmake_minimum_required(VERSION 3.14)
project(scegfx VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)

function(define_file_basename_for_sources targetname)
  get_target_property(source_files "${targetname}" SOURCES)
  foreach(sourcefile ${source_files})
    # Get source file's current list of compile definitions.
    get_property(defs SOURCE "${sourcefile}" PROPERTY COMPILE_DEFINITIONS)
    # Add the FILE_BASENAME=filename compile definition to the list.
    get_filename_component(basename "${sourcefile}" NAME)
    list(APPEND defs "FILE_BASENAME=\"${basename}\"")
    # Set the updated compile definitions on the source file.
    set_property(SOURCE "${sourcefile}" PROPERTY COMPILE_DEFINITIONS ${defs})
  endforeach()
endfunction()

if(NOT EMSCRIPTEN)
  option(SCEGFX_SHARED_LIB "Build shared library (.so/.dll/.dylib)" ON)
  option(SCEGFX_SUPPORTS_VULKAN "Include the Vulkan Renderer" ON)
endif()
option(SCEGFX_SUPPORTS_WEBGPU "Include the Webgpu Renderer" OFF)
option(SCEGFX_BUILD_EXAMPLES "Include examples in project" ON)

if(NOT EMSCRIPTEN AND SCEGFX_SHARED_LIB)
  set(LIB_TYPE SHARED)
else()
  set(LIB_TYPE STATIC)
endif()

include(GNUInstallDirs)

file(
  GLOB SOURCE_FILES
  include/scegfx/allocator.h
  include/scegfx/buffer.h
  include/scegfx/command_buffer.h
  include/scegfx/common.h
  include/scegfx/context.h
  include/scegfx/device_memory.h
  include/scegfx/descriptor_set.h
  include/scegfx/descriptor_set_layout.h
  include/scegfx/fence.h
  include/scegfx/framebuffer.h
  include/scegfx/image.h
  include/scegfx/image_view.h
  include/scegfx/pipeline_layout.h
  include/scegfx/pipeline.h
  include/scegfx/render_pass.h
  include/scegfx/sampler.h
  include/scegfx/semaphore.h
  include/scegfx/shader_module.h
  include/scegfx/swapchain.h
  src/context.c
  src/backend/opengl/buffer_opengl.h
  src/backend/opengl/buffer_opengl.c
  src/backend/opengl/commands_opengl.h
  src/backend/opengl/commands_opengl.c
  src/backend/opengl/command_buffer_opengl.h
  src/backend/opengl/command_buffer_opengl.c
  src/backend/opengl/context_opengl.h
  src/backend/opengl/context_opengl.c
  src/backend/opengl/device_memory_opengl.h
  src/backend/opengl/fence_opengl.h
  src/backend/opengl/fence_opengl.c
  src/backend/opengl/framebuffer_opengl.h
  src/backend/opengl/framebuffer_opengl.c
  src/backend/opengl/image_opengl.h
  src/backend/opengl/image_opengl.c
  src/backend/opengl/image_view_opengl.h
  src/backend/opengl/image_view_opengl.c
  src/backend/opengl/descriptor_set_opengl.h
  src/backend/opengl/descriptor_set_layout_opengl.h
  src/backend/opengl/descriptor_set_layout_opengl.c
  src/backend/opengl/pipeline_opengl.h
  src/backend/opengl/pipeline_opengl.c
  src/backend/opengl/pipeline_layout_opengl.h
  src/backend/opengl/pipeline_layout_opengl.c
  src/backend/opengl/render_pass_opengl.h
  src/backend/opengl/render_pass_opengl.c
  src/backend/opengl/sampler_opengl.h
  src/backend/opengl/sampler_opengl.c
  src/backend/opengl/semaphore_opengl.h
  src/backend/opengl/semaphore_opengl.c
  src/backend/opengl/shader_module_opengl.h
  src/backend/opengl/shader_module_opengl.c
  src/backend/opengl/swapchain_opengl.h
  src/backend/opengl/swapchain_opengl.c
  src/backend/opengl/vao_map.h
  src/backend/opengl/vao_map.c
)

file(
  GLOB WEBGPU_SOURCE_FILES
  src/backend/webgpu/buffer_webgpu.h
  src/backend/webgpu/buffer_webgpu.c
  src/backend/webgpu/context_webgpu.h
  src/backend/webgpu/context_webgpu.c
  src/backend/webgpu/device_memory_webgpu.h
  src/backend/webgpu/swapchain_webgpu.h
  src/backend/webgpu/swapchain_webgpu.c
)

file(
  GLOB VULKAN_SOURCE_FILES
  src/backend/vulkan/buffer_vulkan.h
  src/backend/vulkan/buffer_vulkan.c
  src/backend/vulkan/command_buffer_vulkan.h
  src/backend/vulkan/command_buffer_vulkan.c
  src/backend/vulkan/context_vulkan.h
  src/backend/vulkan/context_vulkan.c
  src/backend/vulkan/device_memory_vulkan.h
  src/backend/vulkan/descriptor_set.h
  src/backend/vulkan/descriptor_set_layout_vulkan.h
  src/backend/vulkan/descriptor_set_layout_vulkan.c
  src/backend/vulkan/fence_vulkan.h
  src/backend/vulkan/fence_vulkan.c
  src/backend/vulkan/framebuffer_vulkan.h
  src/backend/vulkan/framebuffer_vulkan.c
  src/backend/vulkan/image_vulkan.h
  src/backend/vulkan/image_vulkan.c
  src/backend/vulkan/image_view_vulkan.h
  src/backend/vulkan/image_view_vulkan.c
  src/backend/vulkan/pipeline_layout_vulkan.h
  src/backend/vulkan/pipeline_layout_vulkan.c
  src/backend/vulkan/pipeline_vulkan.h
  src/backend/vulkan/pipeline_vulkan.c
  src/backend/vulkan/render_pass_vulkan.h
  src/backend/vulkan/render_pass_vulkan.c
  src/backend/vulkan/semaphore_vulkan.h
  src/backend/vulkan/semaphore_vulkan.c
  src/backend/vulkan/sampler_vulkan.h
  src/backend/vulkan/sampler_vulkan.c
  src/backend/vulkan/shader_module_vulkan.h
  src/backend/vulkan/shader_module_vulkan.c
  src/backend/vulkan/swapchain_vulkan.h
  src/backend/vulkan/swapchain_vulkan.c
)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

if(EMSCRIPTEN)
  set(SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS ON)
  set(SPIRV_CROSS_SHARED ON)
  set(SPIRV_CROSS_STATIC ON)
  set(SPIRV_CROSS_CLI OFF)
  set(SPIRV_CROSS_ENABLE_TESTS OFF)
  set(SPIRV_CROSS_ENABLE_MSL OFF)
  set(SPIRV_CROSS_ENABLE_HLSL OFF)
  set(SPIRV_CROSS_ENABLE_CPP OFF)
  set(SPIRV_CROSS_ENABLE_REFLECT OFF)
  set(SPIRV_CROSS_ENABLE_C_API ON)
  set(SPIRV_CROSS_ENABLE_UTIL OFF)
  add_subdirectory(third_party/SPIRV-Cross)
  set(SPIRV_CROSS_LIBRARIES "spirv-cross-c")

  list(APPEND EMSCRIPTEN_OPTIONS "USE_SDL=2")
  list(APPEND EMSCRIPTEN_OPTIONS "FULL_ES3=1")
  list(APPEND EMSCRIPTEN_OPTIONS "GL_ASSERTIONS=1")
  set(OPENGL_LIBRARIES "-s USE_WEBGL2=1")
else()
  find_package(spirv_cross_c_shared REQUIRED)
  set(SPIRV_CROSS_LIBRARIES "spirv-cross-c-shared")
  find_package(SDL2 REQUIRED COMPONENTS SDL2::SDL2)
  set(SDL2_LIBRARIES SDL2::SDL2)
endif()

if(SCEGFX_SUPPORTS_WEBGPU)
  find_package(wgpu REQUIRED)
  set(WEBGPU_LIBRARIES wgpu::wgpu)
  list(APPEND SOURCE_FILES ${WEBGPU_SOURCE_FILES})
endif()

if(SCEGFX_SUPPORTS_VULKAN)
  find_package(Vulkan REQUIRED)
  set(VULKAN_LIBRARIES Vulkan::Vulkan)
  list(APPEND SOURCE_FILES ${VULKAN_SOURCE_FILES})
endif()

add_library(scegfx ${LIB_TYPE} ${SOURCE_FILES})
define_file_basename_for_sources(scegfx)

if(MSVC)
  target_compile_options(scegfx PRIVATE /W4 /WX)
else(MSVC)
  target_compile_options(
    scegfx
    PRIVATE
      -Wall
      -Wextra
      -pedantic
      -Werror
  )
endif(MSVC)
set_property(TARGET scegfx PROPERTY C_STANDARD 11)

if(SCEGFX_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

target_include_directories(
  scegfx
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

add_compile_definitions(scegfx PRIVATE GL_GLEXT_PROTOTYPES)
target_link_libraries(
  scegfx
  PRIVATE
    ${SDL2_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${WEBGPU_LIBRARIES}
    ${VULKAN_LIBRARIES}
    ${SPIRV_CROSS_LIBRARIES}
)
if(EMSCRIPTEN)
  add_compile_definitions(scegfx PRIVATE SCEGFX_SUPPORTS_VULKAN=0)
  list(APPEND EMSCRIPTEN_OPTIONS "WASM=1")
  foreach(OPT ${EMSCRIPTEN_OPTIONS})
    set(EMSCRIPTEN_OPTION_STR "${EMSCRIPTEN_OPTION_STR} -s ${OPT}")
  endforeach()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 ${EMSCRIPTEN_OPTION_STR}")
  set(
    CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} ${EMSCRIPTEN_OPTION_STR}"
  )
  add_compile_definitions(scegfx PUBLIC EMSCRIPTEN=1)
else()
  if(SCEGFX_SUPPORTS_VULKAN)
    add_compile_definitions(scegfx PUBLIC SCEGFX_SUPPORTS_VULKAN=1)
  else()
    add_compile_definitions(scegfx PUBLIC SCEGFX_SUPPORTS_VULKAN=0)
  endif()
  if(SCEGFX_SUPPORTS_WEBGPU)
    add_compile_definitions(scegfx PUBLIC SCEGFX_SUPPORTS_WEBGPU=1)
  else()
    add_compile_definitions(scegfx PUBLIC SCEGFX_SUPPORTS_WEBGPU=0)
  endif()
  add_compile_definitions(scegfx PUBLIC SCEGFX_VALIDATION=1)
endif()

install(
  TARGETS scegfx
  EXPORT scegfx-config
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
  EXPORT scegfx-config
  NAMESPACE scegfx::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/scegfx
)

install(
  DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/scegfx
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
