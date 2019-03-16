add_executable(hello_scegfx hello_scegfx.c)

if(EMSCRIPTEN)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_SDL=2 --emrun")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  add_compile_definitions(hello_scegfx PUBLIC EMSCRIPTEN=1)
else()
  find_package(SDL2 REQUIRED)
  set(SDL_LIBRARIES SDL2::SDL2)
endif()

target_link_libraries(hello_scegfx PRIVATE ${SDL_LIBRARIES} scegfx)
