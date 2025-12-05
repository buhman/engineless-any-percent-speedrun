#pragma once

#ifdef __cplusplus
extern "C" {
#endif

  struct game_state {
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
