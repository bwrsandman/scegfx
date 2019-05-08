//
// Created by sandy on 05/05/19.
//

#include "commands_opengl.h"

#include <SDL_opengl.h>
#include <assert.h>

#include "command_buffer_opengl.h"
#include "framebuffer_opengl.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

void
scegfx_command_begin_render_pass_opengl(const scegfx_command_arg_t* arg)
{
  const scegfx_render_pass_begin_info_t* info = &arg->render_pass_begin_info;
  const scegfx_framebuffer_opengl_t* framebuffer =
    (const scegfx_framebuffer_opengl_t*)info->framebuffer;

  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->handle);

  // TODO: find an equivalent, not glViewport
  // info->render_area

  glClearBufferfv(GL_COLOR, 0, info->clear_values.color);
  glClearBufferfv(GL_DEPTH, 0, &info->clear_values.depth);
}

void
scegfx_command_end_render_pass_opengl(const scegfx_command_arg_t* arg)
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#pragma clang diagnostic pop

void
scegfx_command_opengl_bind_pipeline(const scegfx_command_arg_t* arg)
{
  if (arg->bind_pipeline.type == scegfx_pipeline_type_graphics) {
    if (arg->bind_pipeline.graphics.cull_face) {
      glEnable(GL_CULL_FACE);
      glCullFace(arg->bind_pipeline.graphics.cull_face);
    } else {
      glDisable(GL_CULL_FACE);
    }
    glFrontFace(arg->bind_pipeline.graphics.front_face);
    glLineWidth(arg->bind_pipeline.graphics.line_width);
  }
  assert(glIsProgram(arg->bind_pipeline.program));
  glUseProgram(arg->bind_pipeline.program);
}
