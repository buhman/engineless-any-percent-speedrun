#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t _binary_src_texture_noise_data_start __asm("_binary_src_texture_noise_data_start");
extern uint32_t _binary_src_texture_noise_data_end __asm("_binary_src_texture_noise_data_end");

#define src_texture_noise_data_start ((const char *)&_binary_src_texture_noise_data_start)
#define src_texture_noise_data_end ((const char *)&_binary_src_texture_noise_data_end)
#define src_texture_noise_data_size (src_texture_noise_data_end - src_texture_noise_data_start)

#ifdef __cplusplus
}
#endif
