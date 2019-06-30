#include "include/polar_string.h"
#include "util.h"
#include "constants.h"

#include <unistd.h>
#include <atomic>
#include <dirent.h>
#include <cstdio>
#include <map>
#include <thread>

namespace polar_race {

bool utilRead(int fd, void* buffer, size_t size) {
    int retCode = read(fd, buffer, size);
    return retCode == int(size);
}

void utilWrite(int fd,const void* buffer, size_t size) {
    int ret = write(fd, buffer, size);
    // printf("(%d) %d : ret %d require %d\n", std::this_thread::get_id(), ret, int(size));
    if (ret != int(size)) {
        printf("error in write\n");
    }
}

void utilMkdir(const std::string &dir) {
    if (access(dir.c_str(), F_OK) != 0) {
        int ret = mkdir(dir.c_str(), 0755);
        if (ret != 0) {
            printf("mkdir fail\n");
        }
  }
}

uint64_t sampleValue(const char* value, uint32_t len) {
    if (len >= 8)
        return *(const uint64_t*)(value);
    else {
        char *st = new char[8]();
        memcpy(st, value, len);
        return *(uint64_t*)(st);
    }
}

uint32_t fetchData(char x, int offset, int mask=7) {
    return (x & mask) << offset;
}

uint32_t char_to_int(char c) {
    if (c >= '0' && c <= '9')
        return c-'0'+1;
    if (c >= 'a' && c <= 'z')
        return c-'a'+11;
    if (c >= 'A' && c <= 'Z')
        return c-'A'+37;
    return 63;
}

std::map<uint32_t, std::string> xx;

uint32_t keyToInt(const char* value, uint32_t len) {
    if (len == 8 && value[7] == 0) {
        return ((uint32_t(value[0]+128) << 16) | (uint32_t(value[1]+128) << 8) 
                | uint32_t(value[2]+128)) +1;
    }
    uint32_t ret = 233;
    for (uint32_t i=0; i<len; i++)
        ret = ret * 2333 + value[i];
    return ret == 0 ? 1 : ret;
}

PolarString toPolarKey(uint32_t key) {
    key--;
    char st[8];
    memset(st, 0, sizeof(st));
    st[0] = ((key>>16) & ((1<<8)-1)) - 128;
    st[1] = ((key>>8) & ((1<<8)-1)) - 128;
    st[2] = (key & ((1<<8)-1)) - 128;
    std::string ret;
    ret.assign(st, 8);
    // printf("toPolar %u: ", key);
    // for (int i=0; i<8; i++) printf("%x ", st[i]); printf("\n");
    return PolarString(ret);
}

}