#include "hash_table.h"
#include <assert.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <thread>

#include "include/polar_string.h"
#include "util.h"
#include "file_group.h"

namespace polar_race {

HashTable::HashTable(uint32_t arraySize)
{
    // Initialize cells
    assert((arraySize & (arraySize - 1)) == 0);   // Must be a power of 2
    m_arraySize = arraySize;
    m_entries = new Node[arraySize];
    Clear();
}

HashTable::~HashTable()
{
    // Delete cells
    delete[] m_entries;
}

int HashTable::SetItem(uint64_t mini_key, const FileGroup::Location loc)
{

    for (uint64_t idx = mini_key; ; idx++)
    {
        idx &= m_arraySize-1;
        uint64_t probed = m_entries[idx].key.load();
        if (probed != mini_key)
        {
            if (probed != 0)
                continue;
            uint64_t zero = 0;
            
            if (!m_entries[idx].key.compare_exchange_strong(zero, mini_key, 
                            std::memory_order_release,  std::memory_order_relaxed))
                continue;
                
        }
        // printf("htb %llu: %llu\n", mini_key, idx);
        m_entries[idx].increase();
        m_entries[idx].value.store(loc);
        return m_entries[idx].tm.load();
    }
}

bool HashTable::GetItem(uint64_t mini_key, FileGroup::Location& loc)
{
    assert(mini_key != 0);
    for (uint64_t idx = mini_key;; idx++)
    {
        idx &= m_arraySize - 1;

        uint64_t probedKey = m_entries[idx].key.load();
        // printf("probed %llu\n", probedKey);
        if (probedKey == mini_key) {
            // printf("htb %llu: %llu\n", mini_key, idx);
            loc = m_entries[idx].value.load();
            return loc.fileID != 0; //可能在set了key，没set value的时候访问
        }
        if (probedKey == 0)
            return 0;
    }
}

void HashTable::Clear()
{
    memset(m_entries, 0, sizeof(Node) * m_arraySize);
}

HashTable* htb;

void thread_load(int thread_id) {
    // printf("loading %d\n", thread_id);
    int fd = open(htb->forLoad.name_list[thread_id].c_str(), O_RDONLY);
    // printf("fd: %d\n", fd);
    int tm;
    uint32_t keyLen;
    FileGroup::Location loc;
    uint64_t mini_key;

    while (Read(fd, &tm, sizeof(tm))) {
        if (!Read(fd, &keyLen, sizeof(keyLen))) {
            return;
        }
        if (!Read(fd, &loc.fileID, sizeof(loc.fileID))) {
            return;
        }
        if (!Read(fd, &loc.offset, sizeof(loc.offset))) {
            return;
        }
        if (!Read(fd, &mini_key, sizeof(mini_key))) {
            return;
        }
        lseek(fd, keyLen, SEEK_CUR);

        // insert to hash table
        for (uint64_t idx = mini_key; ; idx++)
        {
            idx &= htb->m_arraySize-1;
            uint64_t probed = htb->m_entries[idx].key.load();
            if (probed != mini_key)
            {
                if (probed != 0)
                    continue;
                uint64_t zero = 0;
                
                if (!htb->m_entries[idx].key.compare_exchange_strong(zero, mini_key, 
                                std::memory_order_release,  std::memory_order_relaxed))
                    continue;
                    
            }
            htb->forLoad.mtx_list[idx].lock();
            // printf("htb %llu: %llu\n", mini_key, idx);
            if (tm > htb->forLoad.time_list[idx]) {
                htb->m_entries[idx].value.store(loc);
                htb->forLoad.time_list[idx] = tm; 
            }
            htb->forLoad.mtx_list[idx].unlock();
            break;
        }
    }
}

void HashTable::Load(std::string log_dir) {
    DIR* streamp;
    dirent *dep;
    streamp = opendir(log_dir.c_str());
    forLoad.time_list = new int[m_arraySize];
    forLoad.mtx_list = new std::mutex[m_arraySize];
    errno = 0;
    while ((dep = readdir(streamp)) != NULL) {
        // printf("old %s\n", (log_dir + '/' + std::string(dep->d_name)).c_str());
        forLoad.name_list.push_back(log_dir + '/' + std::string(dep->d_name));
    }
    int n_th = forLoad.name_list.size();
    htb = this;
    std::thread th[n_th];
    
    for (int i=0; i<n_th; i++)
        th[i] = std::thread(thread_load, i);

    for (int i=0; i<n_th; i++)
        th[i].join();
    
    delete forLoad.time_list;
    delete forLoad.mtx_list;
    std::vector<std::string>().swap(forLoad.name_list);
}

}