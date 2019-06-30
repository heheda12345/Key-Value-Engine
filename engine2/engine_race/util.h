#ifndef __UTIL__
#define __UTIL__
#include <atomic>
#include <cstdio>
#include <string>
#include <sys/stat.h>
#include <stdint.h>
#include "include/polar_string.h"

namespace polar_race {

bool utilRead(int fd, void* buffer, size_t size);
void utilWrite(int fd, const void* buffer, size_t size);
void utilMkdir(const std::string &dir);
 
uint64_t sampleValue(const char* value, uint32_t len); // val > 0
uint32_t keyToInt(const char* value, uint32_t len);
PolarString toPolarKey(uint32_t key);
}
#endif //__UTIL__