#include "hash.h"

#include <assert.h>
#include <cstring>
#include <cstdio>

#include "constants.h"

namespace polar_race {

Hash::~Hash() {
    delete[] a;
}

void Hash::init() {
    assert((HASH_SIZE & (HASH_SIZE-1)) == 0);
    arraySize = HASH_SIZE;
    a = new Node[arraySize];
    memset(a, 0, sizeof(Node) * arraySize);
}

void Hash::insert(uint64_t valueSample, uint32_t valueID) {
    for (uint64_t idx = valueSample; ; idx++) {
        idx &= (arraySize-1);
        uint64_t probed = a[idx].valueSample.load();
        if (probed != valueSample) { // a key may be saved twice
            if (probed != 0)
                continue;
            if (!a[idx].valueSample.compare_exchange_strong(probed, valueSample, 
                std::memory_order_release,  std::memory_order_relaxed))
                continue;
        }
        // printf("to hash %lld\n", idx);
        a[idx].valueID.store(valueID);
        break;
    }
}


bool Hash::exist(uint64_t valueSample, uint32_t &valueID) {
    for (uint64_t idx = valueSample; ; idx++) {
        idx &= (arraySize-1);
        uint64_t probed = a[idx].valueSample.load();
        // printf("cur %lld\n", idx);
        if (probed == valueSample) {
            valueID = a[idx].valueID.load();
            return valueID != 0;
        }
        if (probed == 0)
            return 0;
    }
}

}

