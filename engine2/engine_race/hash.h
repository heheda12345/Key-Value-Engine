#ifndef __HASH__
#define __HASH__

#include <atomic>

namespace polar_race {

class Hash {
public:
    ~Hash();
    void init();
    void insert(uint64_t valueSample, uint32_t valueID);
    bool exist(uint64_t valueSample, uint32_t &valueID);

private:
    struct Node {
        std::atomic<uint64_t> valueSample; // key
        std::atomic<uint32_t> valueID; // value
    };
    Node *a;
    int arraySize;

};

}
#endif