#pragma once

#ifdef __cplusplus
extern "C" {
#endif

  typedef unsigned int uint;

  struct mesh {
    uint vtx;
    uint idx;
    uint length;
  };

  void render(struct mesh paddle_mesh,
              struct mesh brick_mesh,
              struct mesh ball_mesh,
              uint attrib_position,
              uint attrib_texture,
              uint attrib_normal,
              uint uniform_trans,
              uint uniform_normal_trans,
              uint uniform_base_color,
              uint uniform_light_pos,
              float paddle_x);

#ifdef __cplusplus
}
#endif
