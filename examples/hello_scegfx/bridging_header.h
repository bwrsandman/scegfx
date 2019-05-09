//
// Created by sandy on 24/07/19.
//

#ifndef SCEGFX_EXAMPLE_HELLO_SCEGFX_BRIDGING_HEADER_H
#define SCEGFX_EXAMPLE_HELLO_SCEGFX_BRIDGING_HEADER_H

#if defined(SCEGFX_C_SOURCE)
typedef float vec4[4];
#endif // defined(SCE_C_SOURCE)

// Vertex shader inputs
#define V_POSITION_LOCATION 0
#define V_COLOR_LOCATION 1
// Fragment shader inputs
#define F_COLOR_LOCATION 0
// Framebuffer inputs
#define FB_COLOR_LOCATION 0

#endif // SCEGFX_EXAMPLE_HELLO_SCEGFX_BRIDGING_HEADER_H
