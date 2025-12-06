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

static uint64_t xorshift_state = 0x1234567812345678;

uint64_t xorshift64(uint64_t x)
{
  x ^= x << 13;
  x ^= x >> 7;
  x ^= x << 17;
  return x;
}

void launch_ball(struct game_state * state, double time)
{
  if (state->balls_launched >= MAX_BALLS)
    return;

  if (xorshift_state == 0x1234567812345678) {
    double timed = time;
    uint64_t timei = *((uint64_t *)&timed);
    xorshift_state = xorshift64(xorshift_state + timei);
  }
  xorshift_state = xorshift64(xorshift_state);

  uint64_t rx = (xorshift_state >> 0) & 0xffffffff;
  double orientation = rx;
  double x = cos(orientation) - sin(orientation);
  double y = sin(orientation) + cos(orientation);
  vec2 d = normalize(vec2(x, y)) * 0.2f;
  if (d.y < 0.1)
    d.y = 0.1;
  if (fabsf(d.x) > 0.05)
    d.x = 0.05f * fabsf(d.x) / d.x;

  struct ball_state& ball = state->balls[state->balls_launched];
  ball.ball_x = state->paddle_x;
  ball.ball_y = 25.0f;
  ball.ball_dx = d.x;
  ball.ball_dy = -d.y;
  ball.launch_time = time;

  state->balls_launched += 1;
}

void reset_level(struct game_state * state)
{
  state->paddle_x = 0.0;
  state->paddle_y = 26.0;

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

static inline void ball_collision_response(struct ball_state& ball,
                                           struct collision_data& cd)
{
  ball.ball_x = cd.escape_position.x / 4.0f;
  ball.ball_y = -cd.escape_position.y / 2.0f;
  vec3 vel = reflect(vec3(ball.ball_dx, ball.ball_dy, 0), cd.bounds_normal);
  ball.ball_dx = vel.x;
  ball.ball_dy = vel.y;
}

void update_ball(struct game_state * state, struct ball_state& ball, double time)
{
  vec3 ball_position = vec3(ball.ball_x * 4.0f, -ball.ball_y * 2.0f, 0.0);

  //////////////////////////////////////////////////////////////////////
  // block collision
  //////////////////////////////////////////////////////////////////////
  for (int y = 0; y < 28; y++) {
    for (int x = 0; x < 13; x++) {
      int block_ix = y * 13 + x;
      char tile = state->level[block_ix];
      if (tile == 0)
        continue;
      if (state->blocks[block_ix].destroyed_time != 0.0)
        continue;

      vec3 block_position = vec3(x * 4.0f, -y * 2.0f, 0.0f);

      // paddle 6.0
      // block 4.0
      const vec3 block_bounds = vec3(2, 1, 0);
      struct collision_data cd;
      bool collided = aabb_circle_collision(block_position, ball_position, block_bounds, &cd);
      if (collided) {
        ball_collision_response(ball, cd);
        state->blocks[block_ix].destroyed_time = time;
      }
    }
  }

  vec3 paddle_position = vec3(state->paddle_x * 4.0f, -state->paddle_y * 2.0f, 0.0);

  //////////////////////////////////////////////////////////////////////
  // paddle collision
  //////////////////////////////////////////////////////////////////////
  {
    const vec3 paddle_bounds = vec3(3, 1, 0);
    struct collision_data cd;
    bool collided = aabb_circle_collision(paddle_position, ball_position, paddle_bounds, &cd);
    if (collided) {
      ball_collision_response(ball, cd);
      ball.ball_y = 25.0f;
      ball.ball_dy = -fabsf(ball.ball_dy);
    }
  }

  //////////////////////////////////////////////////////////////////////
  // arena collision
  //////////////////////////////////////////////////////////////////////
  if ((ball.ball_x + ball.ball_dx * 0.4) > 12.25f) {
    ball.ball_x = 12.25f;
    ball.ball_dx = -ball.ball_dx;
  } else if ((ball.ball_x + ball.ball_dx * 0.4) < -0.25f) {
    ball.ball_x = -0.25f;
    ball.ball_dx = -ball.ball_dx;
  }

  if ((ball.ball_y + ball.ball_dy * 0.4) > 27.0f) {
    //ball.ball_y = 27.0f;
    //ball.ball_dy = -ball.ball_dy;
  } else if ((ball.ball_y + ball.ball_dy * 0.4) < 0.0f) {
    ball.ball_y = 0.0f;
    ball.ball_dy = -ball.ball_dy;
  }

  ball.ball_x += ball.ball_dx;
  ball.ball_y += ball.ball_dy;
}

void update(struct game_state * state, double time)
{
  for (int i = 0; i < state->balls_launched; i++) {
    if (state->balls[i].ball_y > 30.0f)
      continue;

    update_ball(state, state->balls[i], time);
  }

  if (state->balls_launched == 0) {
    state->start_time = time;
  }

  state->time = time;
  state->remaining = 20.0 - (time - state->start_time);
}
