#include <math.h>
#include <stdio.h>
#include <assert.h>

#include "update.hpp"
#include "collision.hpp"
#include "state.h"

#include "level/level1.data.h"
#include "level/level1.data.pal.h"

#include "math/float_types.hpp"
#include "math/transform.hpp"

void reset_level(struct game_state * state)
{
  state->paddle_x = 0.0;
  state->paddle_y = 26.0;

  state->ball_x = 0.0;
  state->ball_y = 25.0;

  state->ball_dx = 0.01;
  state->ball_dy = 0.01;

  state->start_time = 0.0;

  assert(src_level_level1_data_size == 13 * 28);
  const uint8_t * level = (const uint8_t *)src_level_level1_data_start;
  const uint8_t * pal = (const uint8_t *)src_level_level1_data_pal_start;

  state->level = level;
  state->pal = pal;
  for (int i = 0; i < 28 * 13; i++) {
    state->blocks[i].destroyed_time = 0.0f;
  }
}

void update(struct game_state * state)
{
  state->ball_x += state->ball_dx;
  state->ball_y += state->ball_dy;
}
