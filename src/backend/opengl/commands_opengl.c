//
// Created by sandy on 05/05/19.
//

#include "commands_opengl.h"

#include <assert.h>

#include <SDL_opengl.h>
#include <assert.h>

#include "command_buffer_opengl.h"
#include "framebuffer_opengl.h"
#include "pipeline_opengl.h"

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
  glBindVertexArray(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

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

void
scegfx_command_opengl_bind_vao(const scegfx_command_arg_t* arg)
{
  assert(glIsVertexArray(arg->bind_vao.vertex_array_object));
  glBindVertexArray(arg->bind_vao.vertex_array_object);
}

void
scegfx_command_opengl_draw(const scegfx_command_arg_t* arg)
{
  glDrawArraysInstanced(arg->draw.mode,
                        arg->draw.first_vertex,
                        arg->draw.vertex_count,
                        arg->draw.instance_count);
}

void
scegfx_command_opengl_draw_indexed(const scegfx_command_arg_t* arg)
{
  glDrawElementsInstanced(arg->draw_indexed.mode,
                          arg->draw_indexed.index_count,
                          arg->draw_indexed.index_type,
                          (const void*)arg->draw_indexed.first_index,
                          arg->draw_indexed.index_count);
}

void
scegfx_command_opengl_debug_marker_begin(const scegfx_command_arg_t* arg)
{
  glPushDebugGroup(
    GL_DEBUG_SOURCE_APPLICATION, 0, -1, arg->debug_maker_info.name);
}

void
scegfx_command_opengl_debug_marker_end(const scegfx_command_arg_t* arg)
{
  glPopDebugGroup();
}

#pragma clang diagnostic pop
