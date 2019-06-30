#ifndef __BUCKET__
#define __BUCKET__

#include <string.h>
#include <atomic>
#include <unordered_map>

#include "include/engine.h"
#include "st_array.h"
#include "rw_lock.h"

namespace polar_race {

class Bucket {
public:
    ~Bucket();
    void init();
    void insert(uint32_t keyID, uint32_t valueID);
    uint32_t find(uint32_t keyID);
    void traverse(uint32_t lowerKeyID, uint32_t upperKeyID, StArray* stArray, Visitor& visitor);

private:
    std::mutex* mtx, mtx_xx;
    uint32_t* bucket;
    std::atomic<uint32_t>* valueID;
    std::unordered_map<uint32_t, uint32_t> xx;
    int* cnt;
    int n_bucket;
    int n_item;
    RWLock rwLock;
};
}

#endif