#ifndef __CONSTANTS__
#define __CONSTANTS__
const size_t HASH_SIZE = (1<<22);
const int MAX_THREAD = 64;
const int DAB_MAX_SIZE = 1<<26; // 128M
const int LOCK_GROUP_LEN = 16; // engine_race.h里分组的位数
const int LOCK_GROUP_BIAS = 4; // engine_race.h里分组的偏移量
const int LOG_MAX_SIZE = 1<<22; // 4M
const int META_LEN = 1<<12; // 4K
const int HOT_TABLE_SIZE = 512;
const int HOT_TABLE_ITEM = 256;
const int MAX_KEY_LEN = 1<<12; // 4K

#endif //__CONSTANTS__