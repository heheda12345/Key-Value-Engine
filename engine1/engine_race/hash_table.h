#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include <atomic>
#include <mutex>
#include "file_group.h"
#include "include/polar_string.h"
#include "constants.h"

// ref: https://github.com/mintomic/samples/blob/master/common/hashtable1.h
// NOTE: can not assign any value to key = 0
namespace polar_race {

class HashTable
{
public:
    HashTable(uint32_t arraySize = HASH_SIZE);
    ~HashTable();

    // Basic operations
    int SetItem(uint64_t mini_key, const FileGroup::Location loc);
    bool GetItem(uint64_t mini_key, FileGroup::Location &loc);
    void Load(std::string log_dir);
    void Clear();
    friend void thread_load(int thread_id);
private:
    struct Node
    {
        std::atomic<uint64_t> key;
        std::atomic<uint8_t> tm;
        std::atomic<FileGroup::Location> value;
        void increase() {
            // 不是绝对安全的，但够用了。
            uint8_t x = tm.load();
            if (x < 255)
                tm.compare_exchange_strong(x, x+1, 
                    std::memory_order_release,  std::memory_order_relaxed);
        }
    };
    struct ForLoad {
        std::mutex* mtx_list;
        int* time_list;
        std::vector<std::string> name_list;
    } forLoad;
    Node *m_entries;
    uint32_t m_arraySize;
};


}
#endif // __HASHTABLE_H__
