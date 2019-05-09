//
// Created by sandy on 05/05/19.
//

#include "commands_opengl.h"

#include <assert.h>

#include <SDL_opengl.h>
#include <assert.h>

#include "buffer_opengl.h"
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
  glBindBuffer(GL_COPY_READ_BUFFER, 0);
  glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
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
scegfx_command_opengl_copy_buffer(const scegfx_command_arg_t* arg)
{
  glBindBuffer(arg->copy_buffer.src_target, arg->copy_buffer.src_buffer_handle);
  glBindBuffer(arg->copy_buffer.dst_target, arg->copy_buffer.dst_buffer_handle);
  glCopyBufferSubData(arg->copy_buffer.src_target,
                      arg->copy_buffer.dst_target,
                      arg->copy_buffer.region.src_offset,
                      arg->copy_buffer.region.dst_offset,
                      arg->copy_buffer.region.size);
}

#if !defined(EMSCRIPTEN)
void
scegfx_command_opengl_copy_buffer_to_image_1d(const scegfx_command_arg_t* arg)
{
  glBindTexture(arg->copy_buffer_to_image.image_target,
                arg->copy_buffer_to_image.image_handle);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, arg->copy_buffer_to_image.buffer_handle);
  glTexSubImage1D(
    arg->copy_buffer_to_image.image_target,
    arg->copy_buffer_to_image.region.image_subresource.mip_level,
    arg->copy_buffer_to_image.region.image_offset.x,
    arg->copy_buffer_to_image.region.image_extent.width,
    arg->copy_buffer_to_image.image_format,
    arg->copy_buffer_to_image.image_type,
    (void*)(uintptr_t)arg->copy_buffer_to_image.region.buffer_offset);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}
#endif // !defined(EMSCRIPTEN)

void
scegfx_command_opengl_copy_buffer_to_image_2d(const scegfx_command_arg_t* arg)
{
  glBindTexture(arg->copy_buffer_to_image.image_target,
                arg->copy_buffer_to_image.image_handle);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, arg->copy_buffer_to_image.buffer_handle);
  glTexSubImage2D(
    arg->copy_buffer_to_image.image_target,
    arg->copy_buffer_to_image.region.image_subresource.mip_level,
    arg->copy_buffer_to_image.region.image_offset.x,
    arg->copy_buffer_to_image.region.image_offset.y,
    arg->copy_buffer_to_image.region.image_extent.width,
    arg->copy_buffer_to_image.region.image_extent.height,
    arg->copy_buffer_to_image.image_format,
    arg->copy_buffer_to_image.image_type,
    (void*)(uintptr_t)arg->copy_buffer_to_image.region.buffer_offset);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void
scegfx_command_opengl_copy_buffer_to_image_3d(const scegfx_command_arg_t* arg)
{
  glBindTexture(arg->copy_buffer_to_image.image_target,
                arg->copy_buffer_to_image.image_handle);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, arg->copy_buffer_to_image.buffer_handle);
  glTexSubImage3D(
    arg->copy_buffer_to_image.image_target,
    arg->copy_buffer_to_image.region.image_subresource.mip_level,
    arg->copy_buffer_to_image.region.image_offset.x,
    arg->copy_buffer_to_image.region.image_offset.y,
    arg->copy_buffer_to_image.region.image_offset.z,
    arg->copy_buffer_to_image.region.image_extent.width,
    arg->copy_buffer_to_image.region.image_extent.height,
    arg->copy_buffer_to_image.region.image_extent.depth,
    arg->copy_buffer_to_image.image_format,
    arg->copy_buffer_to_image.image_type,
    (void*)(uintptr_t)arg->copy_buffer_to_image.region.buffer_offset);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void
scegfx_command_opengl_copy_image_to_buffer(const scegfx_command_arg_t* arg)
{
  assert(arg);
  assert(false);
}

#if defined(EMSCRIPTEN)
void
scegfx_command_opengl_copy_texture_2d(const scegfx_command_arg_t* arg)
{
  assert(arg->copy_image.region.src_subresource.mip_level ==
         arg->copy_image.region.dst_subresource.mip_level);
  assert(arg->copy_image.region.src_offset.x ==
           arg->copy_image.region.dst_offset.x ||
         arg->copy_image.region.src_offset.y ==
           arg->copy_image.region.dst_offset.y);
  glBindTexture(GL_TEXTURE_2D, arg->copy_image.src_image_handle);
  glCopyTexSubImage2D(arg->copy_image.dst_image_handle,
                      arg->copy_image.region.src_subresource.mip_level,
                      0,
                      0,
                      arg->copy_image.region.src_offset.x,
                      arg->copy_image.region.src_offset.y,
                      arg->copy_image.region.extent.width,
                      arg->copy_image.region.extent.height);
}

void
scegfx_command_opengl_copy_texture_3d(const scegfx_command_arg_t* arg)
{
  assert(arg->copy_image.region.src_subresource.mip_level ==
         arg->copy_image.region.dst_subresource.mip_level);
  assert(arg->copy_image.region.src_offset.x ==
           arg->copy_image.region.dst_offset.x ||
         arg->copy_image.region.src_offset.y ==
           arg->copy_image.region.dst_offset.y ||
         arg->copy_image.region.src_offset.z ==
           arg->copy_image.region.dst_offset.z);
  glBindTexture(GL_TEXTURE_3D, arg->copy_image.src_image_handle);
  glCopyTexSubImage3D(arg->copy_image.dst_image_handle,
                      arg->copy_image.region.src_subresource.mip_level,
                      0,
                      0,
                      arg->copy_image.region.src_offset.x,
                      arg->copy_image.region.src_offset.y,
                      // arg->copy_image.region.src_offset.z,
                      arg->copy_image.region.extent.width,
                      arg->copy_image.region.extent.height,
                      arg->copy_image.region.extent.depth);
}

#else
void
scegfx_command_opengl_copy_image(const scegfx_command_arg_t* arg)
{
  assert(arg->copy_image.region.src_subresource.mip_level ==
         arg->copy_image.region.dst_subresource.mip_level);
  assert(arg->copy_image.region.src_offset.x ==
           arg->copy_image.region.dst_offset.x ||
         arg->copy_image.region.src_offset.y ==
           arg->copy_image.region.dst_offset.y);
  glBindTexture(GL_TEXTURE_BUFFER, arg->copy_image.src_image_handle);
  glBindTexture(arg->copy_image.dst_image_format,
                arg->copy_image.dst_image_handle);
  glCopyTexSubImage2D(arg->copy_image.dst_image_handle,
                      arg->copy_image.region.src_subresource.mip_level,
                      0,
                      0,
                      arg->copy_image.region.src_offset.x,
                      arg->copy_image.region.src_offset.y,
                      arg->copy_image.region.extent.width,
                      arg->copy_image.region.extent.height);

  //  glCopyImageSubData(arg->copy_image.src_image_handle,
  //                     arg->copy_image.src_image_format,
  //                     arg->copy_image.region.src_subresource.mip_level,
  //                     arg->copy_image.region.src_offset.x,
  //                     arg->copy_image.region.src_offset.y,
  //                     arg->copy_image.region.src_offset.z,
  //                     arg->copy_image.dst_image_handle,
  //                     arg->copy_image.dst_image_format,
  //                     arg->copy_image.region.dst_subresource.mip_level,
  //                     arg->copy_image.region.dst_offset.x,
  //                     arg->copy_image.region.dst_offset.y,
  //                     arg->copy_image.region.dst_offset.z,
  //                     arg->copy_image.region.extent.width,
  //                     arg->copy_image.region.extent.height,
  //                     arg->copy_image.region.extent.depth);
}
#endif // define(EMSCRIPTEN)

void
scegfx_command_opengl_memory_barrier(const scegfx_command_arg_t* arg)
{
  glMemoryBarrier(arg->memory_barrier.barriers);
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
