#pragma once

#include "state.h"

#ifdef __cplusplus
extern "C" {
#endif

  void launch_ball(struct game_state * state);
  void reset_level(struct game_state * state);
  void update(struct game_state * state, double time);

#ifdef __cplusplus
}
#endif
