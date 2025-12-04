#pragma once

#ifdef __cplusplus
extern "C" {
#endif

  typedef unsigned int uint;

void render(uint vtx, uint idx,
            uint attrib_position,
            uint attrib_normal,
            uint uniform_trans,
            uint uniform_normal_trans,
            uint uniform_base_color,
            uint uniform_light_pos,
            int length);
#ifdef __cplusplus
}
#endif
