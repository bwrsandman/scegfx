//
// Created by sandy on 25/06/19.
//

#include <assert.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include <scegfx/context.h>
#include <scegfx/swapchain.h>

#if defined(EMSCRIPTEN)
#include <emscripten/emscripten.h>
#endif // defined(EMSCRIPTEN)

typedef struct args_t
{
  enum
  {
    args_backend_opengl,
    args_backend_vulkan,
    args_backend_webgpu,
  } backend;
  int32_t width;
  int32_t height;
  bool run_frames;
  uint32_t run_frames_count;
} args_t;

void
clean_up();

void
run_app();

void
run_loop();

struct app_t {
  args_t args;
  bool quit;
  uint32_t frame_count;
  SDL_Window* window;
  scegfx_context_t* context;
  scegfx_swapchain_t* swapchain;
} app = {};

args_t default_args = {
  .backend = args_backend_opengl,
  .width = 640,
  .height = 480,
  .run_frames = false,
  .run_frames_count = 0,
};
const char* usage_string =
  "Usage: %s [OPTIONS]\n"
  "\n"
  "Options:\n"
  "    -h, --help          Display this message\n"
  "    --width WIDTH       Window width of size WIDTH\n"
  "    --height HEIGHT     Window height of size HEIGHT\n"
  "    --backend [opengl|vulkan|webgpu]\n"
  "                        Specify which backend renderer to use.\n"
  "    --run-frames NUM    Run application for only NUM frames.\n"
  "\n";

void
debug_callback(scegfx_debug_severity_t level,
               uint32_t msg_code,
               const char* source,
               const char* message)
{
  char* level_str = "";
  switch (level) {
    case scegfx_debug_severity_debug:
      level_str = "debug";
      break;
    case scegfx_debug_severity_info:
      level_str = "info";
      break;
    case scegfx_debug_severity_warning:
      level_str = "warning";
      break;
    case scegfx_debug_severity_performance_warning:
      level_str = "performance_warning";
      break;
    case scegfx_debug_severity_error:
      level_str = "error";
      break;
  }
  printf("[%s] (%d) %s: %s\n", level_str, msg_code, source, message);
}

bool
parse_args(int argc, char* const argv[], args_t* args, int* return_code)
{
  *args = default_args;
  for (int i = 1; i < argc; ++i) {
    const char* arg = argv[i];
    if (strcmp(arg, "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      printf(usage_string, argv[0]);
      *return_code = 0;
      return false;
    } else if (strcmp(arg, "--width") == 0) {
      if (argc > i + 1) {
        ++i;
        const char* width_str = argv[i];
        unsigned long width = strtoul(width_str, NULL, 10);
        if (width > 0 && width < INT32_MAX) {
          args->width = width;
        } else {
          fprintf(stderr,
                  "error: argument for %s must be a valid positive 32 bit "
                  "signed integer. (instead was %s)\n",
                  arg,
                  width_str);
          *return_code = 1;
          return false;
        }
      } else {
        fprintf(stderr, "error: Argument option for %s not supplied\n", arg);
        *return_code = 1;
        return false;
      }
    } else if (strcmp(arg, "--height") == 0) {
      if (argc > i + 1) {
        ++i;
        const char* height_str = argv[i];
        unsigned long height = strtoul(height_str, NULL, 10);
        if (height > 0 && height < INT32_MAX) {
          args->height = height;
        } else {
          fprintf(stderr,
                  "error: argument for %s must be a valid positive 32 bit "
                  "signed integer. (instead was %s)\n",
                  arg,
                  height_str);
          *return_code = 1;
          return false;
        }
      } else {
        fprintf(stderr, "error: Argument option for %s not supplied\n", arg);
        *return_code = 1;
        return false;
      }
    } else if (strcmp(arg, "--backend") == 0) {
      if (argc > i + 1) {
        ++i;
        const char* backend_str = argv[i];
        if (strcmp(backend_str, "opengl") == 0) {
          args->backend = args_backend_opengl;
        } else if (strcmp(backend_str, "vulkan") == 0) {
          args->backend = args_backend_vulkan;
        } else if (strcmp(backend_str, "webgpu") == 0) {
          args->backend = args_backend_webgpu;
        } else {
          fprintf(stderr,
                  "error: argument for %s must be one of: "
                  "opengl|vulkan|webgpu. (instead was %s)\n",
                  arg,
                  backend_str);
          *return_code = 1;
          return false;
        }
      } else {
        fprintf(stderr, "error: Argument option for %s not supplied\n", arg);
        *return_code = 1;
        return false;
      }
    } else if (strcmp(arg, "--run-frames") == 0) {
      if (argc > i + 1) {
        ++i;
        const char* frame_count_str = argv[i];
        unsigned long frame_count = strtoul(frame_count_str, NULL, 10);
        if ((frame_count > 0 && frame_count < UINT32_MAX) ||
            strcmp(frame_count_str, "0") == 0) {
          args->run_frames = true;
          args->run_frames_count = frame_count;
        } else {
          fprintf(stderr,
                  "error: argument for %s must be a valid 32 bit unsigned"
                  "integer. (instead was %s)\n",
                  arg,
                  frame_count_str);
          *return_code = 1;
          return false;
        }
      } else {
        fprintf(stderr, "error: Argument option for %s not supplied\n", arg);
        *return_code = 1;
        return false;
      }
    } else {
      fprintf(stderr, "error: Unrecognized option %s\n", arg);
      *return_code = 1;
      return false;
    }
  }
  return true;
}

void
create_window()
{
  uint32_t sdl_window_flags = 0;
  switch (app.args.backend) {
    case args_backend_opengl:
      sdl_window_flags = SDL_WINDOW_OPENGL;
      break;
    case args_backend_vulkan:
      sdl_window_flags = SDL_WINDOW_VULKAN;
      break;
    case args_backend_webgpu:
      break;
    default:
      assert(false);
      break;
  }
  app.window = SDL_CreateWindow("hello_scegfx",
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                app.args.width,
                                app.args.height,
                                sdl_window_flags);
  assert(app.window);
}

void
init_context()
{
  scegfx_backend_api_t backend;
  switch (app.args.backend) {
    case args_backend_opengl:
      backend = scegfx_backend_api_opengl;
      break;
    case args_backend_vulkan:
      backend = scegfx_backend_api_vulkan;
      break;
    case args_backend_webgpu:
      backend = scegfx_backend_api_webgpu;
      break;
    default:
      assert(false);
      break;
  }
  app.context =
    scegfx_context_create(backend, NULL, app.window, debug_callback);
  app.context->api_vtable->initialize(app.context);
}

void
init_swapchain()
{
  app.swapchain = app.context->api_vtable->create_swapchain(app.context, NULL);
  app.swapchain->api_vtable->initialize(app.swapchain);
}

void
run_app()
{
  app.frame_count = 0;
  app.quit = app.args.run_frames && app.args.run_frames_count == 0;
#if defined(EMSCRIPTEN)
  emscripten_set_main_loop(run_loop, 0, true);
#else
  while (!app.quit) {
    run_loop();
  }
#endif // defined(EMSCRIPTEN)
}

void
handle_keyboard()
{
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        app.quit = true;
        break;
      case SDL_KEYDOWN: {
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            app.quit = true;
            break;
        }
      } break;
    }
  }
}

void
run_loop()
{
#if defined(EMSCRIPTEN)
  if (app.quit) {
    clean_up();
    emscripten_cancel_main_loop();
  }
#endif

  handle_keyboard();

  ++app.frame_count;
  if (app.args.run_frames && app.frame_count >= app.args.run_frames_count) {
    app.quit = true;
  }
}

void
clean_up()
{
  app.swapchain->api_vtable->terminate(app.swapchain);
  app.context->api_vtable->destroy_swapchain(app.context, app.swapchain, NULL);
  app.context->api_vtable->terminate(app.context);
  scegfx_context_destroy(app.context, NULL);

  SDL_DestroyWindow(app.window);
  SDL_Quit();
}

int
main(int argc, char* argv[])
{
  int return_code = EXIT_SUCCESS;
  {
    args_t args = {};
    if (!parse_args(argc, argv, &args, &return_code)) {
      return return_code;
    }
    app.args = args;
  }

  SDL_Init(SDL_INIT_VIDEO);
  create_window();
  init_context();
  init_swapchain();

  run_app();

  clean_up();

  return return_code;
}
