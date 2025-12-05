#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

  struct block_state {
    double destroyed_time;
  };

  struct ball_state {
    float ball_x;
    float ball_y;
    float ball_dx;
    float ball_dy;
  };

  #define MAX_BALLS 20

  struct game_state {
    struct block_state blocks[28 * 13];
    const uint8_t * level;
    const uint8_t * pal;

    float paddle_x;
    float paddle_y;

    struct ball_state balls[MAX_BALLS];
    int balls_launched;

    double start_time;
    double time;
    double remaining;
  };

#ifdef __cplusplus
}
#endif
