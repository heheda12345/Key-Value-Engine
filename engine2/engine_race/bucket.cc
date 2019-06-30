#include "bucket.h"

#include <mutex>
#include <assert.h>
#include "constants.h"
#include "util.h"
namespace polar_race {

Bucket::~Bucket() {
    delete[] mtx;
    delete[] cnt;
    delete[] valueID;
    delete[] bucket;
}

void Bucket::init() {
    n_bucket = BUCKET_TOTAL_SIZE/BUCKET_PER_SIZE;
    mtx = new std::mutex[n_bucket];
    cnt = new int[n_bucket];
    memset(cnt, 0, sizeof(int)*n_bucket);

    n_item = BUCKET_TOTAL_SIZE;
    valueID = new std::atomic<uint32_t>[n_item](); 
    bucket = new uint32_t[n_item]();
}

void Bucket::insert(uint32_t keyID, uint32_t valueID) {
    // printf("lock get\n");
    if (keyID >= MAX_KEY_ID) {
        assert(false);
        mtx_xx.lock();
        xx[keyID] = valueID;
        mtx_xx.unlock();
        return;
    }
    uint32_t pre = this->valueID[keyID].exchange(valueID);
    // uncomment the following code if range search is needed
    if (pre) {
        return;
    }
    rwLock.lock_write();
    int bk = keyID/BUCKET_PER_SIZE;
    int bkStart = bk * BUCKET_PER_SIZE;
    mtx[bk].lock();
    int i = cnt[bk]-1;
    for (; i>=0; i--) {
        if (bucket[i+bkStart] > keyID) {
            bucket[i+bkStart+1] = bucket[i+bkStart];
        } else {
            break;
        }
    }
    bucket[i+bkStart+1] = keyID;
    cnt[bk]++;
    mtx[bk].unlock();
    rwLock.release_write();
}

uint32_t Bucket::find(uint32_t keyID) {
    if (keyID >= MAX_KEY_ID) {
        assert(false);
        mtx_xx.lock();
        uint32_t ret = xx[keyID];
        mtx_xx.unlock();
        return ret;
    }
    uint32_t valueID = this->valueID[keyID].load();
    return valueID;
}

void Bucket::traverse(uint32_t lowerKeyID, uint32_t upperKeyID, StArray* stArray, Visitor& visitor) {
    rwLock.lock_read();
    int bkLower = lowerKeyID / BUCKET_PER_SIZE, bkUpper = upperKeyID / BUCKET_PER_SIZE;
    if (bkLower == bkUpper) {
        int bkStart = bkLower * BUCKET_PER_SIZE, bkEnd = bkStart + cnt[bkLower];
        for (int j=bkStart; j<bkEnd; j++) {
            uint32_t keyID = bucket[j];
            if (bucket[j] >= lowerKeyID && bucket[j] < upperKeyID)
                visitor.Visit(toPolarKey(keyID), PolarString(stArray->st[valueID[keyID]]));
        }
    } else {
        int bkStart = bkLower * BUCKET_PER_SIZE, bkEnd = bkStart + cnt[bkLower];
        for (int j=bkStart; j<bkEnd; j++) {
            uint32_t keyID = bucket[j];
            if (bucket[j] >= lowerKeyID)
                visitor.Visit(toPolarKey(keyID), PolarString(stArray->st[valueID[keyID].load()]));
        }
        for (int i=bkLower+1; i<bkUpper; i++) {
            bkStart = i * BUCKET_PER_SIZE, bkEnd = bkStart + cnt[i];
            for (int j=bkStart; j<bkEnd; j++) {
                uint32_t keyID = bucket[j];
                visitor.Visit(toPolarKey(keyID), PolarString(stArray->st[valueID[keyID].load()]));
            }
        }
        bkStart = bkUpper * BUCKET_PER_SIZE, bkEnd = bkStart + cnt[bkUpper];
        for (int j=bkStart; j<bkEnd; j++) {
            uint32_t keyID = bucket[j];
            if (bucket[j] < upperKeyID)
                visitor.Visit(toPolarKey(keyID), PolarString(stArray->st[valueID[keyID].load()]));
        }
    }
    rwLock.release_read();
}



   
}