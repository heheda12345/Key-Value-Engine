#ifndef __IO__
#define __IO__

#include <string>
#include <mutex>
#include <atomic>

#include "bucket.h"
#include "hash.h"
#include "st_array.h"

namespace polar_race {

class IO {
private:
    static IO* instance;
    IO() : tag('#') {};
    const char tag;

public:
    static IO* Instance();
    void setTarget(Hash* hash_, Bucket* bucket_, StArray* stArray_) {
        hash = hash_;
        bucket = bucket_;
        stArray = stArray_;
    }
    void init(const std::string& dir);
    uint32_t newValue(const char* st, uint32_t len);
    void newKey(uint32_t keyID, uint32_t valueID);
    Hash* hash;
    Bucket* bucket;
    StArray* stArray;
    char* buffer;
    char* logPtr;
    int fd_hash, fd_log;
    std::mutex mtx_hash, mtx_log;
    std::atomic<int> valueTotal;
    std::atomic<int> logCnt;
};

}
#endif