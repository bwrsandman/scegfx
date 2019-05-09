//
// Created by sandy on 25/06/19.
//

#include <assert.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include <scegfx/command_buffer.h>
#include <scegfx/context.h>
#include <scegfx/fence.h>
#include <scegfx/framebuffer.h>
#include <scegfx/image_view.h>
#include <scegfx/pipeline.h>
#include <scegfx/pipeline_layout.h>
#include <scegfx/render_pass.h>
#include <scegfx/semaphore.h>
#include <scegfx/shader_module.h>
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
  scegfx_fence_t* acquire_fence;
  scegfx_fence_t* present_fence;
  scegfx_semaphore_t* acquire_semaphore;
  scegfx_semaphore_t* render_semaphore;
  scegfx_image_t** swapchain_image;
  scegfx_image_view_t** swapchain_image_view;
  scegfx_framebuffer_t** framebuffer;
  scegfx_command_buffer_t** cmd;
  scegfx_render_pass_t* render_pass;
  scegfx_pipeline_layout_t* pipeline_layout;
  scegfx_pipeline_t* pipeline;
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

  app.swapchain_image =
    malloc(sizeof(scegfx_image_t*) * app.swapchain->image_count);
  app.swapchain_image_view =
    malloc(sizeof(scegfx_image_view_t*) * app.swapchain->image_count);

  for (uint32_t i = 0; i < app.swapchain->image_count; ++i) {
    app.swapchain_image_view[i] =
      app.context->api_vtable->create_image_view(app.context, NULL);
    app.swapchain_image[i] =
      app.swapchain->api_vtable->get_image(app.swapchain, i);
    app.swapchain_image_view[i]->api_vtable->initialize(
      app.swapchain_image_view[i],
      app.swapchain_image[i],
      scegfx_image_aspect_color);
  }
}

void
init_framebuffer()
{
  app.framebuffer =
    malloc(sizeof(scegfx_framebuffer_t*) * app.swapchain->image_count);
  for (uint32_t i = 0; i < app.swapchain->image_count; ++i) {
    app.framebuffer[i] =
      app.context->api_vtable->create_framebuffer(app.context, NULL);
    {
      scegfx_framebuffer_create_info_t info = {
        .render_pass = app.render_pass,
        .attachment_count = 1,
        .attachments = &app.swapchain_image_view[i],
        .width = app.swapchain->extent.width,
        .height = app.swapchain->extent.height,
      };
      app.framebuffer[i]->api_vtable->initialize(app.framebuffer[i], &info);
    }
  }
}

void
init_synchronization_primitives()
{
  app.acquire_fence = app.context->api_vtable->create_fence(app.context, NULL);
  app.acquire_fence->api_vtable->initialize(app.acquire_fence, true);
  app.present_fence = app.context->api_vtable->create_fence(app.context, NULL);
  app.present_fence->api_vtable->initialize(app.present_fence, true);
  app.acquire_semaphore =
      app.context->api_vtable->create_semaphore(app.context, NULL);
  app.acquire_semaphore->api_vtable->initialize(app.acquire_semaphore);
  app.render_semaphore =
      app.context->api_vtable->create_semaphore(app.context, NULL);
  app.render_semaphore->api_vtable->initialize(app.render_semaphore);
}

void
init_render_pass()
{
  app.render_pass =
    app.context->api_vtable->create_render_pass(app.context, NULL);
  app.render_pass->api_vtable->initialize(app.render_pass);
}

void
encode_commands()
{
  app.cmd =
    malloc(sizeof(scegfx_command_buffer_t*) * app.swapchain->image_count);

  for (uint32_t i = 0; i < app.swapchain->image_count; ++i) {
    app.cmd[i] =
      app.context->api_vtable->create_command_buffer(app.context, NULL);

    app.cmd[i]->api_vtable->initialize(app.cmd[i]);
    app.cmd[i]->api_vtable->begin(app.cmd[i], true);
    {
      scegfx_render_pass_begin_info_t info = {
          .render_area = {
              .offset = {
                  .x = 0,
                  .y = 0,
              },
              .extent = app.swapchain->extent,
          },
          .clear_values = {
              .color = {
                  [0] = 0.3f,
                  [1] = 0.4f,
                  [2] = 0.4f,
                  [3] = 1.0f,
              },
          },
          .render_pass = app.render_pass,
          .framebuffer = app.framebuffer[i],
      };
      app.cmd[i]->api_vtable->begin_render_pass(app.cmd[i], &info);
    }
    app.cmd[i]->api_vtable->bind_pipeline(
      app.cmd[i], scegfx_pipeline_type_graphics, app.pipeline);
    app.cmd[i]->api_vtable->draw(app.cmd[i], 3, 1, 0, 0);
    app.cmd[i]->api_vtable->end_render_pass(app.cmd[i]);
    app.cmd[i]->api_vtable->end(app.cmd[i]);
  }
}

bool
init_pipeline()
{
  const char* base_path = SDL_GetBasePath();
  scegfx_shader_module_t* vert_module =
    app.context->api_vtable->create_shader_module(app.context, NULL);
  {
    char filename[FILENAME_MAX];
    sprintf(filename, "%shello_scegfx.vert.glsl.spv", base_path);
    FILE* file = fopen(filename, "r");
    fseek(file, 0L, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    char* shader_src = malloc(size);
    fread(shader_src, size, size, file);
    fclose(file);
    scegfx_shader_module_create_info_t info = {
      .type = scegfx_stage_type_vertex,
      .fix_vertex_y = true,
      .entry_point = "main",
      .size = size,
      .shader_src = shader_src,
    };
    bool initialized = vert_module->api_vtable->initialize(vert_module, &info);
    free(shader_src);
    if (!initialized) {
      fprintf(stderr, "error: could initialize shader for %s\n", filename);
      assert(initialized);
      return false;
    }
  }
  scegfx_shader_module_t* frag_module =
    app.context->api_vtable->create_shader_module(app.context, NULL);
  {
    char filename[FILENAME_MAX];
    sprintf(filename, "%shello_scegfx.frag.glsl.spv", base_path);
    FILE* file = fopen(filename, "r");
    fseek(file, 0L, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    char* shader_src = malloc(size);
    fread(shader_src, size, size, file);
    fclose(file);
    scegfx_shader_module_create_info_t info = {
      .type = scegfx_stage_type_fragment,
      .entry_point = "main",
      .size = size,
      .shader_src = shader_src,
    };
    bool initialized = frag_module->api_vtable->initialize(frag_module, &info);
    free(shader_src);
    if (!initialized) {
      fprintf(stderr, "error: could initialize shader for %s\n", filename);
      assert(initialized);
      return false;
    }
  }
  SDL_free((void*)base_path);

  app.pipeline_layout =
    app.context->api_vtable->create_pipeline_layout(app.context, NULL);
  app.pipeline_layout->api_vtable->initialize(app.pipeline_layout);
  app.pipeline = app.context->api_vtable->create_pipeline(app.context, NULL);
  {
    scegfx_pipeline_create_info_t info = {
        .type = scegfx_pipeline_type_graphics,
        .layout = app.pipeline_layout,
        .graphics = {
            .stages = {
                .vertex = vert_module,
                .fragment = frag_module,
            },
            .raster_state = {
                .cull_mode = scegfx_raster_state_cull_mode_back,
                .front_face = scegfx_raster_state_front_face_counter_clockwise,
                .line_width = 1.0f,
            },
            .render_pass = app.render_pass,
        },
    };
    app.pipeline->api_vtable->initialize(app.pipeline, &info);
  }

  vert_module->api_vtable->terminate(vert_module);
  frag_module->api_vtable->terminate(frag_module);
  app.context->api_vtable->destroy_shader_module(
    app.context, vert_module, NULL);
  app.context->api_vtable->destroy_shader_module(
    app.context, frag_module, NULL);

  return true;
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

  uint32_t image_index = 0;
  app.acquire_fence->api_vtable->wait(app.acquire_fence, app.acquire_fence->max_wait_timeout);
  app.acquire_fence->api_vtable->reset(app.acquire_fence);
  bool wait_result =
    app.swapchain->api_vtable->acquire_next_image(app.swapchain,
                                                  UINT64_MAX,
                                                  app.acquire_semaphore,
                                                  app.acquire_fence,
                                                  &image_index);
  assert(wait_result);
  app.present_fence->api_vtable->wait(app.present_fence, app.present_fence->max_wait_timeout);
  app.present_fence->api_vtable->reset(app.present_fence);
  {
    scegfx_submit_info_t info = {
      .command_buffer = app.cmd[image_index],
      .wait_semaphore = app.acquire_semaphore,
      .signal_semaphore = app.render_semaphore,
    };
    app.context->api_vtable->submit_to_queue(
      app.context, &info, app.present_fence);
  }
  {
    scegfx_present_info_t info = {
      .wait_semaphore = app.render_semaphore,
      .swapchain = app.swapchain,
      .image_index = image_index,
    };
    bool presented = app.context->api_vtable->present(app.context, &info);
    assert(presented);
  }

  ++app.frame_count;
  if (app.args.run_frames && app.frame_count >= app.args.run_frames_count) {
    app.quit = true;
  }
}

void
clean_up()
{
  uint32_t image_count = app.swapchain->image_count;
  app.acquire_fence->api_vtable->terminate(app.acquire_fence);
  app.present_fence->api_vtable->terminate(app.present_fence);
  app.acquire_semaphore->api_vtable->terminate(app.acquire_semaphore);
  app.render_semaphore->api_vtable->terminate(app.render_semaphore);
  app.pipeline_layout->api_vtable->terminate(app.pipeline_layout);
  app.pipeline->api_vtable->terminate(app.pipeline);
  for (uint32_t i = 0; i < image_count; ++i) {
    app.swapchain_image_view[i]->api_vtable->terminate(
      app.swapchain_image_view[i]);
    app.framebuffer[i]->api_vtable->terminate(app.framebuffer[i]);
    app.cmd[i]->api_vtable->terminate(app.cmd[i]);
  }
  app.render_pass->api_vtable->terminate(app.render_pass);

  app.swapchain->api_vtable->terminate(app.swapchain);

  app.context->api_vtable->destroy_fence(app.context, app.acquire_fence, NULL);
  app.context->api_vtable->destroy_fence(app.context, app.present_fence, NULL);
  app.context->api_vtable->destroy_semaphore(
    app.context, app.acquire_semaphore, NULL);
  app.context->api_vtable->destroy_semaphore(
    app.context, app.render_semaphore, NULL);
  app.context->api_vtable->destroy_pipeline_layout(
    app.context, app.pipeline_layout, NULL);
  app.context->api_vtable->destroy_pipeline(app.context, app.pipeline, NULL);
  for (uint32_t i = 0; i < image_count; ++i) {
    app.context->api_vtable->destroy_image_view(
      app.context, app.swapchain_image_view[i], NULL);
    app.context->api_vtable->destroy_framebuffer(
      app.context, app.framebuffer[i], NULL);
    app.context->api_vtable->destroy_command_buffer(
      app.context, app.cmd[i], NULL);
  }
  app.context->api_vtable->destroy_render_pass(
    app.context, app.render_pass, NULL);

  app.context->api_vtable->destroy_swapchain(app.context, app.swapchain, NULL);

  app.context->api_vtable->terminate(app.context);
  scegfx_context_destroy(app.context, NULL);

  SDL_DestroyWindow(app.window);
  SDL_Quit();

  free(app.swapchain_image);
  free(app.swapchain_image_view);
  free(app.framebuffer);
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
  init_synchronization_primitives();
  init_render_pass();
  init_framebuffer();
  if (!init_pipeline()) {
    return 1;
  }
  encode_commands();

  run_app();

  clean_up();

  return return_code;
}
