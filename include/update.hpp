#pragma once

#include "state.h"

#ifdef __cplusplus
extern "C" {
#endif

  void reset_level(struct game_state * state);
  void update(struct game_state * state);

#ifdef __cplusplus
}
#endif
