#pragma once

#include "state.h"

#ifdef __cplusplus
extern "C" {
#endif

  typedef unsigned int uint;

  struct mesh {
    uint vtx;
    uint idx;
    uint length;
  };

  void render_blocks(struct mesh block_mesh,
                     uint attrib_position,
                     uint attrib_texture,
                     uint attrib_normal,
                     uint uniform_trans,
                     uint uniform_normal_trans,
                     uint uniform_base_color,
                     uint uniform_light_pos,
                     uint uniform_time,
                     struct game_state * state);

  void render_balls(struct mesh ball_mesh,
                    uint attrib_position,
                    uint attrib_texture,
                    uint attrib_normal,
                    uint uniform_trans,
                    uint uniform_normal_trans,
                    uint uniform_base_color,
                    uint uniform_light_pos,
                    struct game_state * state);

  void render_font(struct mesh plane_mesh,
                   uint attrib_position,
                   uint attrib_texture,
                   uint uniform_trans,
                   uint uniform_texture_trans,
                   uint uniform_texture0,
                   uint uniform_base_color,
                   uint uniform_time,
                   struct game_state * state);

  void render_background(struct mesh plane_mesh,
                         uint attrib_position,
                         uint uniform_resolution,
                         uint uniform_trans,
                         uint uniform_texture1,
                         uint uniform_time,
                         uint uniform_palette,
                         uint uniform_aspect,
                         struct game_state * state);

  void render_paddle(struct mesh paddle_mesh,
                     uint attrib_position,
                     uint attrib_texture,
                     uint attrib_normal,
                     uint uniform_trans,
                     uint uniform_normal_trans,
                     uint uniform_light_pos,
                     uint uniform_time,
                     struct game_state * state);

#ifdef __cplusplus
}
#endif
