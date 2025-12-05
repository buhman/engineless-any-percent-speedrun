#include <math.h>
#include <stdio.h>
#include <assert.h>

#include "update.hpp"
#include "collision2.hpp"
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

  state->ball_dx = 0.1;
  state->ball_dy = 0.1;

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

void update(struct game_state * state, double time)
{
  for (int y = 0; y < 28; y++) {
    for (int x = 0; x < 13; x++) {
      int block_ix = y * 13 + x;
      char tile = state->level[block_ix];
      if (tile == 0)
        continue;
      if (state->blocks[block_ix].destroyed_time != 0.0)
        continue;

      vec3 block_position = vec3(x * 4.0f, -y * 2.0f, 0.0f);
      vec3 ball_position = vec3(state->ball_x * 4.0f, -state->ball_y * 2.0f, 0.0);

      // paddle 6.0
      // block 4.0
      // const vec3 paddle_bounds = vec3(3, 1, 0);
      const vec3 block_bounds = vec3(2, 1, 0);
      struct collision_data cd;
      bool collided = aabb_circle_collision(block_position, ball_position, block_bounds, &cd);
      if (collided) {
        state->ball_x = cd.escape_position.x / 4.0f;
        state->ball_y = -cd.escape_position.y / 2.0f;
        vec3 vel = reflect(vec3(state->ball_dx, state->ball_dy, 0), cd.bounds_normal);
        state->ball_dx = vel.x;
        state->ball_dy = vel.y;

        state->blocks[block_ix].destroyed_time = time;
      }
    }
  }

  state->ball_x += state->ball_dx;
  state->ball_y += state->ball_dy;

  state->time = time;
  state->remaining = 20.0 - (time - state->start_time);
}
