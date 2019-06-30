#ifndef __CONSTANTS__
#define __CONSTANTS__

#include <stdint.h>
namespace polar_race {
const uint32_t HASH_SIZE = 1<<24;
const uint32_t LOG_SIZE = 9*(1<<24);
const uint32_t TOT_VALUE_TYPE = 102400;
const uint32_t MAX_KEY_ID = 1<<24;
const uint32_t LOCK_MASK = MAX_KEY_ID - 1;
const uint32_t BUCKET_TOTAL_SIZE = 1<<24;
const uint32_t BUCKET_PER_SIZE = 1<<8;
const uint32_t BUFFER_SIZE = 1<<22;
}
#endif