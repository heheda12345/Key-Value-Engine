#ifndef __HOT_TABLE__
#define __HOT_TABLE__

#include <atomic>
#include <map>
#include <unordered_map>
#include <mutex>

#include "include/polar_string.h"
#include "constants.h"
#include "non_copy_c_string.h"

namespace polar_race {
class HotTable {
public:
    HotTable(uint32_t arraySize = HOT_TABLE_SIZE);
    ~HotTable();

    bool insert(int tm, uint64_t mini_key, const PolarString* key, const PolarString* value);
    bool get(uint64_t mini_key, const PolarString* key, std::string &value);

private:
    std::unordered_map<uint64_t, int> mp;
    struct Node {
        uint64_t mini_key;
        int tm;
        NonCopyCString* key;
        NonCopyCString* value;
        std::mutex mtx;
    };
    Node **lst;
    int forward(int x);
    uint32_t m_arraySize, sz;
    std::atomic<int> min_tm;
    std::mutex mtx;
};
}
#endif //__HOT_TABLE__