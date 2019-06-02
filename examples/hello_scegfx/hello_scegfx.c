//
// Created by sandy on 25/06/19.
//

#include <assert.h>
#include <stdlib.h>

#include <time.h>

#include <SDL2/SDL.h>

#include <scegfx/buffer.h>
#include <scegfx/command_buffer.h>
#include <scegfx/context.h>
#include <scegfx/descriptor_set_layout.h>
#include <scegfx/device_memory.h>
#include <scegfx/fence.h>
#include <scegfx/framebuffer.h>
#include <scegfx/image_view.h>
#include <scegfx/pipeline.h>
#include <scegfx/pipeline_layout.h>
#include <scegfx/render_pass.h>
#include <scegfx/semaphore.h>
#include <scegfx/shader_module.h>
#include <scegfx/swapchain.h>

#include "bridging_header.h"

#if defined(EMSCRIPTEN)
#include <emscripten/emscripten.h>
#endif // defined(EMSCRIPTEN)

#define countof(x) (sizeof((x)) / sizeof((x)[0]))

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

typedef struct
{
  vec4 position;
  vec4 color;
} vertex_t;

const float fov_y = 110.0f;
const float near = 0.001f;
const float far = 1000.0f;

vertex_t vertices[8] = {
    [0] = {
        .position = { -1.0f, -1.0f, 1.0f, 1.0f },
        .color = { 0.0f, 1.0f, 1.0f, 1.0f },
    },
    [1] = {
        .position = { -1.0f, 1.0f, 1.0f, 1.0f },
        .color = { 1.0f, 1.0f, 1.0f, 1.0f },
    },
    [2] = {
        .position = { 1.0f, 1.0f, 1.0f, 1.0f },
        .color = { 1.0f, 1.0f, 0.0f, 1.0f },
    },
    [3] = {
        .position = { 1.0f, -1.0f, 1.0f, 1.0f },
        .color = { 1.0f, 0.0f, 0.0f, 1.0f },
    },
    [4] = {
        .position = { 1.0f, -1.0f, -1.0f, 1.0f },
        .color = { 1.0f, 0.0f, 1.0f, 1.0f },
    },
    [5] = {
        .position = { 1.0f, 1.0f, -1.0f, 1.0f },
        .color = { 0.0f, 0.0f, 1.0f, 1.0f },
    },
    [6] = {
        .position = { -1.0f, 1.0f, -1.0f, 1.0f },
        .color = { 0.0f, 0.0f, 0.0f, 1.0f },
    },
    [7] = {
        .position = { -1.0f, -1.0f, -1.0f, 1.0f },
        .color = { 0.0f, 1.0f, 0.0f, 1.0f },
    },
};

uint32_t indices[36] = {
  0, 1, 2, 2, 3, 0,

  4, 5, 6, 6, 7, 4,

  6, 1, 0, 0, 7, 6,

  2, 5, 4, 4, 3, 2,

  5, 2, 1, 1, 6, 5,

  7, 0, 3, 3, 4, 7,
};

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
  scegfx_buffer_t* vertex_buffer;
  scegfx_device_memory_t* vertex_buffer_memory;
  scegfx_buffer_t* index_buffer;
  scegfx_device_memory_t* index_buffer_memory;
  struct uniform_t uniform;
  scegfx_buffer_t** uniform_buffers;
  scegfx_device_memory_t** uniform_buffers_memory;
  scegfx_descriptor_set_t** descriptor_sets;
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
    app.cmd[i]->api_vtable->bind_vertex_buffer(
      app.cmd[i], app.vertex_buffer, 0);
    app.cmd[i]->api_vtable->bind_index_buffer(
      app.cmd[i], app.index_buffer, 0, scegfx_index_type_u32);
    app.cmd[i]->api_vtable->bind_descriptor_set(app.cmd[i],
                                                scegfx_pipeline_type_graphics,
                                                app.pipeline_layout,
                                                app.descriptor_sets[i]);
    app.cmd[i]->api_vtable->draw_indexed(
      app.cmd[i], countof(indices), 1, 0, 0, 0);
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
  {
    scegfx_descriptor_set_layout_t* set_layout =
      app.context->api_vtable->create_descriptor_set_layout(app.context, NULL);
    {
      scegfx_descriptor_set_binding_t set_bindings[1] = {
          [0] = {
              .binding = V_UNIFORM_LOCATION,
              .descriptor_type = scegfx_descriptor_type_uniform_buffer,
              .descriptor_count = 1,
              .stage = scegfx_stage_type_vertex,
          },
      };
      scegfx_descriptor_set_layout_create_info_t info = {
        .binding_count = countof(set_bindings),
        .bindings = set_bindings,
      };
      set_layout->api_vtable->initialize(set_layout, &info);
    }
    {
      scegfx_pipeline_layout_create_info_t info = {
        .set_layout_count = 1,
        .set_layouts = &set_layout,
      };
      app.pipeline_layout->api_vtable->initialize(app.pipeline_layout, &info);
    }

    app.descriptor_sets =
      malloc(sizeof(scegfx_descriptor_set_t*) * app.swapchain->image_count);
    for (uint32_t i = 0; i < app.swapchain->image_count; ++i) {
      app.descriptor_sets[i] =
        app.swapchain->api_vtable->allocate_descriptor_set(
          app.swapchain, set_layout, NULL);
      scegfx_descriptor_buffer_info_t buffer_info = {
        .buffer = app.uniform_buffers[i],
        .offset = 0,
        .range = sizeof(struct uniform_t),
      };
      scegfx_write_descriptor_set_t descriptor_writes[1] = {
          [0] = {
              .dst_set = app.descriptor_sets[i],
              .dst_binding = V_UNIFORM_LOCATION,
              .descriptor_count = 1,
              .descriptor_type = scegfx_descriptor_type_uniform_buffer,
              .buffer_info = &buffer_info,
          },
      };
      app.context->api_vtable->update_descriptor_sets(
        app.context, countof(descriptor_writes), descriptor_writes);
    }

    set_layout->api_vtable->terminate(set_layout);
    app.context->api_vtable->destroy_descriptor_set_layout(
      app.context, set_layout, NULL);
  }
  app.pipeline = app.context->api_vtable->create_pipeline(app.context, NULL);
  {
    scegfx_vertex_input_attribute_description_t attrs[2] = {
        [0] = {
            .location = V_POSITION_LOCATION,
            .format = scegfx_format_r32g32b32a32_sfloat,
            .offset = offsetof(vertex_t, position),
        },
        [1] = {
            .location = V_COLOR_LOCATION,
            .format = scegfx_format_r32g32b32a32_sfloat,
            .offset = offsetof(vertex_t, color),
        },
    };
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
            .vertex_input_state = {
                .binding_description = {
                    .stride = sizeof(vertex_t),
                },
                .attribute_description_count = countof(attrs),
                .attribute_descriptions = attrs,
            },
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
init_geometry_buffer(scegfx_device_size_t buffer_size,
                     scegfx_buffer_usage_t usage,
                     void* data,
                     scegfx_buffer_t** out_buffer,
                     scegfx_device_memory_t** out_buffer_memory)
{
  scegfx_buffer_t* buffer =
    app.context->api_vtable->create_buffer(app.context, NULL);
  buffer->api_vtable->initialize(
    buffer, buffer_size, usage | scegfx_buffer_usage_transfer_dst);
  scegfx_device_memory_requirements_t memory_requirements = {};
  app.context->api_vtable->get_buffer_memory_requirements(
    app.context, buffer, &memory_requirements);
  uint32_t memory_type_index = app.context->api_vtable->get_memory_type(
    app.context,
    memory_requirements.memory_type_bits,
    scegfx_memory_properties_device_local);
  scegfx_device_memory_allocate_info_t info = {
    .allocation_size = memory_requirements.size,
    .memory_type_index = memory_type_index,
  };
  scegfx_device_memory_t* buffer_memory =
    app.context->api_vtable->allocate_memory(app.context, &info, NULL);
  app.context->api_vtable->bind_buffer_memory(
    app.context, buffer, buffer_memory, 0);

  scegfx_buffer_t* staging_buffer =
    app.context->api_vtable->create_buffer(app.context, NULL);
  staging_buffer->api_vtable->initialize(
    staging_buffer, buffer_size, usage | scegfx_buffer_usage_transfer_src);

  app.context->api_vtable->get_buffer_memory_requirements(
    app.context, staging_buffer, &memory_requirements);
  uint32_t memory_type = app.context->api_vtable->get_memory_type(
    app.context,
    memory_requirements.memory_type_bits,
    scegfx_memory_properties_host_visible |
      scegfx_memory_properties_host_coherent);
  scegfx_device_memory_allocate_info_t memory_info = {
    .allocation_size = memory_requirements.size,
    .memory_type_index = memory_type,
  };
  scegfx_device_memory_t* staging_memory =
    app.context->api_vtable->allocate_memory(app.context, &memory_info, NULL);

  void* mapped = NULL;
  app.context->api_vtable->map_memory(
    app.context, staging_memory, 0, buffer_size, &mapped);
  memcpy(mapped, data, buffer_size);
  app.context->api_vtable->unmap_memory(app.context, staging_memory);

  app.context->api_vtable->bind_buffer_memory(
    app.context, staging_buffer, staging_memory, 0);

  scegfx_command_buffer_t* staging_command_buffer =
    app.context->api_vtable->create_command_buffer(app.context, NULL);
  staging_command_buffer->api_vtable->initialize(staging_command_buffer);
  staging_command_buffer->api_vtable->begin(staging_command_buffer, false);
  scegfx_buffer_copy_t region = {
    .src_offset = 0,
    .dst_offset = 0,
    .size = buffer_size,
  };
  staging_command_buffer->api_vtable->copy_buffer(
    staging_command_buffer, staging_buffer, buffer, &region);
  staging_command_buffer->api_vtable->end(staging_command_buffer);
  scegfx_submit_info_t submit_info = {
    .command_buffer = staging_command_buffer,
  };
  scegfx_fence_t* staging_fence =
    app.context->api_vtable->create_fence(app.context, NULL);
  staging_fence->api_vtable->initialize(staging_fence, false);
  app.context->api_vtable->submit_to_queue(
    app.context, &submit_info, staging_fence);
  if (!staging_fence->api_vtable->wait(staging_fence, UINT64_MAX)) {
    fprintf(stderr, "error: could not wait on staging fence\n");
  }
  staging_command_buffer->api_vtable->terminate(staging_command_buffer);
  app.context->api_vtable->destroy_command_buffer(
    app.context, staging_command_buffer, NULL);
  staging_fence->api_vtable->terminate(staging_fence);
  app.context->api_vtable->destroy_fence(app.context, staging_fence, NULL);
  app.context->api_vtable->free_memory(app.context, staging_memory, NULL);
  staging_buffer->api_vtable->terminate(staging_buffer);
  app.context->api_vtable->destroy_buffer(app.context, staging_buffer, NULL);

  *out_buffer = buffer;
  *out_buffer_memory = buffer_memory;
}

void
init_geometry()
{
  init_geometry_buffer(sizeof(vertices),
                       scegfx_buffer_usage_vertex_buffer,
                       vertices,
                       &app.vertex_buffer,
                       &app.vertex_buffer_memory);
  init_geometry_buffer(sizeof(indices),
                       scegfx_buffer_usage_index_buffer,
                       indices,
                       &app.index_buffer,
                       &app.index_buffer_memory);
}

void
init_uniforms()
{
  app.uniform_buffers =
    malloc(sizeof(scegfx_buffer_t*) * app.swapchain->image_count);
  app.uniform_buffers_memory =
    malloc(sizeof(scegfx_device_memory_t*) * app.swapchain->image_count);
  for (uint32_t i = 0; i < app.swapchain->image_count; ++i) {
    app.uniform_buffers[i] =
      app.context->api_vtable->create_buffer(app.context, NULL);
    app.uniform_buffers[i]->api_vtable->initialize(
      app.uniform_buffers[i],
      sizeof(struct uniform_t),
      scegfx_buffer_usage_uniform_buffer);
    scegfx_device_memory_requirements_t memory_requirements = {};
    app.context->api_vtable->get_buffer_memory_requirements(
      app.context, app.uniform_buffers[i], &memory_requirements);
    {
      uint32_t memory_type_index = app.context->api_vtable->get_memory_type(
        app.context,
        memory_requirements.memory_type_bits,
        scegfx_memory_properties_host_visible |
          scegfx_memory_properties_host_coherent);
      scegfx_device_memory_allocate_info_t info = {
        .allocation_size = memory_requirements.size,
        .memory_type_index = memory_type_index,
      };
      app.uniform_buffers_memory[i] =
        app.context->api_vtable->allocate_memory(app.context, &info, NULL);
      app.context->api_vtable->bind_buffer_memory(
        app.context, app.uniform_buffers[i], app.uniform_buffers_memory[i], 0);
    }
  }
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

float
get_delta_t()
{
  static struct timespec last_time = {
    .tv_sec = 0,
  };
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);

  float delta_t = now.tv_sec - last_time.tv_sec +
                  0.000000001f * (now.tv_nsec - last_time.tv_nsec);
  if (last_time.tv_sec == 0 && last_time.tv_nsec == 0) {
    delta_t = 0;
  }
  last_time = now;

  return delta_t;
}

void
init_matrices()
{
  app.uniform.model[0] = 1.0f;
  app.uniform.model[1] = 0.0f;
  app.uniform.model[2] = 0.0f;
  app.uniform.model[3] = 0.0f;
  app.uniform.model[4] = 0.0f;
  app.uniform.model[5] = 1.0f;
  app.uniform.model[6] = 0.0f;
  app.uniform.model[7] = 0.0f;
  app.uniform.model[8] = 0.0f;
  app.uniform.model[9] = 0.0f;
  app.uniform.model[10] = 1.0f;
  app.uniform.model[11] = 0.0f;
  app.uniform.model[12] = 0.0f;
  app.uniform.model[13] = 0.0f;
  app.uniform.model[14] = 0.0f;
  app.uniform.model[15] = 1.0f;

  app.uniform.view[0] = 1.0f;
  app.uniform.view[1] = 0.0f;
  app.uniform.view[2] = 0.0f;
  app.uniform.view[3] = 0.0f;
  app.uniform.view[4] = 0.0f;
  app.uniform.view[5] = 1.0f;
  app.uniform.view[6] = 0.0f;
  app.uniform.view[7] = 0.0f;
  app.uniform.view[8] = 0.0f;
  app.uniform.view[9] = 0.0f;
  app.uniform.view[10] = 1.0f;
  app.uniform.view[11] = 0.0f;
  app.uniform.view[12] = 0.0f;
  app.uniform.view[13] = 0.0f;
  app.uniform.view[14] = -10.0f;
  app.uniform.view[15] = 1.0f;

  float half_tan = tanf(0.5f * fov_y * M_PI / 180.0f);
  float aspect =
    app.swapchain->extent.height / (float)app.swapchain->extent.width;
  float z_range_inv = 1.0f / (far - near);

  app.uniform.projection[0] = half_tan * aspect;
  app.uniform.projection[1] = 0.0f;
  app.uniform.projection[2] = 0.0f;
  app.uniform.projection[3] = 0.0f;
  app.uniform.projection[4] = 0.0f;
  app.uniform.projection[5] = half_tan * (app.swapchain->flip_y ? -1.0f : 1.0f);
  app.uniform.projection[6] = 0.0f;
  app.uniform.projection[7] = 0.0f;
  app.uniform.projection[8] = 0.0f;
  app.uniform.projection[9] = 0.0f;
  app.uniform.projection[10] = -(far + near) * z_range_inv;
  app.uniform.projection[11] = -1.0f;
  app.uniform.projection[12] = 0.0f;
  app.uniform.projection[13] = 0.0f;
  app.uniform.projection[14] = -2.0f * far * near * z_range_inv;
  app.uniform.projection[15] = 0.0f;
}

void
rotate_model(float theta)
{
  app.uniform.model[0] = cosf(theta);
  app.uniform.model[1] = 0.0f;
  app.uniform.model[2] = sinf(theta);
  app.uniform.model[3] = 0.0f;

  app.uniform.model[4] = 0.0f;
  app.uniform.model[5] = 1.0f;
  app.uniform.model[6] = 0.0f;
  app.uniform.model[7] = 0.0f;

  app.uniform.model[8] = -sinf(theta);
  app.uniform.model[9] = 0.0f;
  app.uniform.model[10] = cosf(theta);
  app.uniform.model[11] = 0.0f;

  app.uniform.model[12] = 0.0f;
  app.uniform.model[13] = 5.0f * sinf(theta * 3.1f);
  app.uniform.model[14] = 0.0f;
  app.uniform.model[15] = 1.0f;
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

  float delta_t = get_delta_t();

  static float total_time_elapsed = 0.0f;
  total_time_elapsed += delta_t;

  handle_keyboard();
  rotate_model(total_time_elapsed);

  {
    scegfx_device_memory_t* uniform_memory =
      app.uniform_buffers_memory[app.frame_count % app.swapchain->image_count];
    void* mapped;
    app.context->api_vtable->map_memory(
      app.context, uniform_memory, 0, sizeof(app.uniform), &mapped);
    memcpy(mapped, &app.uniform, sizeof(app.uniform));
    app.context->api_vtable->unmap_memory(app.context, uniform_memory);
  }

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
    app.context->api_vtable->free_memory(
      app.context, app.uniform_buffers_memory[i], NULL);
    app.uniform_buffers[i]->api_vtable->terminate(app.uniform_buffers[i]);
    app.swapchain_image_view[i]->api_vtable->terminate(
      app.swapchain_image_view[i]);
    app.framebuffer[i]->api_vtable->terminate(app.framebuffer[i]);
    app.cmd[i]->api_vtable->terminate(app.cmd[i]);
  }
  app.render_pass->api_vtable->terminate(app.render_pass);

  app.context->api_vtable->free_memory(
    app.context, app.vertex_buffer_memory, NULL);
  app.vertex_buffer->api_vtable->terminate(app.vertex_buffer);
  app.context->api_vtable->free_memory(
    app.context, app.index_buffer_memory, NULL);
  app.index_buffer->api_vtable->terminate(app.index_buffer);

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
    app.context->api_vtable->destroy_buffer(
      app.context, app.uniform_buffers[i], NULL);
    app.context->api_vtable->destroy_image_view(
      app.context, app.swapchain_image_view[i], NULL);
    app.context->api_vtable->destroy_framebuffer(
      app.context, app.framebuffer[i], NULL);
    app.context->api_vtable->destroy_command_buffer(
      app.context, app.cmd[i], NULL);
  }
  app.context->api_vtable->destroy_render_pass(
    app.context, app.render_pass, NULL);

  app.context->api_vtable->destroy_buffer(app.context, app.vertex_buffer, NULL);
  app.context->api_vtable->destroy_buffer(app.context, app.index_buffer, NULL);

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
  init_uniforms();
  if (!init_pipeline()) {
    return 1;
  }
  init_geometry();
  init_matrices();
  encode_commands();

  run_app();

  clean_up();

  return return_code;
}
