#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

  struct block_state {
    float destroyed_time;
  };

  struct game_state {
    struct block_state blocks[28 * 13];
    const uint8_t * level;
    const uint8_t * pal;

    float paddle_x;
    float paddle_y;

    float ball_x;
    float ball_y;
    float ball_dx;
    float ball_dy;

    double start_time;
    double remaining;
  };

#ifdef __cplusplus
}
#endif
