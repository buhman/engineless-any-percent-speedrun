#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t _binary_src_level_level1_data_pal_start __asm("_binary_src_level_level1_data_pal_start");
extern uint32_t _binary_src_level_level1_data_pal_end __asm("_binary_src_level_level1_data_pal_end");
extern uint32_t _binary_src_level_level1_data_pal_size __asm("_binary_src_level_level1_data_pal_size");

#define src_level_level1_data_pal_start ((const char *)&_binary_src_level_level1_data_pal_start)
#define src_level_level1_data_pal_end ((const char *)&_binary_src_level_level1_data_pal_end)
#define src_level_level1_data_pal_size (src_level_level1_data_pal_end - src_level_level1_data_pal_start)

#ifdef __cplusplus
}
#endif
