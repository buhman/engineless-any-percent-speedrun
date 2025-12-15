#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t _binary_src_shader_vertex_color_vp_glsl_start __asm("_binary_src_shader_vertex_color_vp_glsl_start");
extern uint32_t _binary_src_shader_vertex_color_vp_glsl_end __asm("_binary_src_shader_vertex_color_vp_glsl_end");

#define src_shader_vertex_color_vp_glsl_start ((const char *)&_binary_src_shader_vertex_color_vp_glsl_start)
#define src_shader_vertex_color_vp_glsl_end ((const char *)&_binary_src_shader_vertex_color_vp_glsl_end)
#define src_shader_vertex_color_vp_glsl_size (src_shader_vertex_color_vp_glsl_end - src_shader_vertex_color_vp_glsl_start)

#ifdef __cplusplus
}
#endif
