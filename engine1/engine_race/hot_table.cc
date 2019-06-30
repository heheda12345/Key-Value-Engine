#include "hot_table.h"
#include <assert.h>

#include "include/polar_string.h"
#include "util.h"

namespace polar_race {

HotTable::HotTable(uint32_t arraySize) : m_arraySize(arraySize), min_tm(0) {
    lst = new Node* [arraySize];
    memset(lst, 0, sizeof(Node*) * m_arraySize);
    sz = 0;
}

HotTable::~HotTable()
{
    for (int i=0; i<m_arraySize; i++)
        delete lst[i];
    delete[] lst;
}

/*
std::map<uint64_t, int> mp;
    struct Node {
        uint64_t mini_key;
        int tm;
        PolarString* key;
        PolarString* value;
    };
    */

int HotTable::forward(int x) {
    while (x > 0 && lst[x-1]->tm < lst[x]->tm) {
        mp[lst[x-1]->mini_key] = x+1;
        std::swap(lst[x-1], lst[x]);
        x--;
    }
    return x;

}

bool HotTable::insert(int tm, uint64_t mini_key, const PolarString* key, const PolarString* value)
{
    if (tm <= min_tm.load())
        return false;
    
    NonCopyCString* key_cp = new NonCopyCString(key->size(), key->data());
    NonCopyCString* value_cp = new NonCopyCString(value->size(), value->data());

    mtx.lock();
    int x = mp[mini_key];
    x--;
    NonCopyCString* key_del = nullptr;
    NonCopyCString* value_del = nullptr;
    if (x < 0) {
        if (sz == m_arraySize) {
            x = sz-1;
            key_del = lst[x]->key;
            value_del = lst[x]->value;
            mp.erase(lst[x]->mini_key);
        } else {
            x = sz++;
            lst[x] = new Node();
        }
    } else {
        key_del = lst[x]->key;
        value_del = lst[x]->value;
    }
    

    lst[x]->tm = tm;
    lst[x]->mini_key = mini_key;
    lst[x]->key = key_cp;
    lst[x]->value = value_cp;
    int y = forward(x);
    mp[mini_key] = y+1;
    min_tm = lst[sz-1]->tm; 
    mtx.unlock();

    lst[y]->mtx.lock();
    lst[y]->mtx.unlock();
    // printf("insert to hot table %d\n", y);

    delete key_del;
    delete value_del;
    return 1;
}

bool HotTable::get(uint64_t mini_key, const PolarString* key, std::string &value)
{
    assert(mini_key != 0);
    mtx.lock();
    int x = mp[mini_key];
    x--;
    bool find = 0;
    NonCopyCString *key_cp, *value_cp;
    std::mutex* mtx_item = nullptr;
    if (x >= 0) {
        find = 1;
        key_cp = lst[x]->key;
        value_cp = lst[x]->value;
        mtx_item =  &lst[x]->mtx;
        mtx_item->lock();
    }
    mtx.unlock();
    if (find) {
        int key_equal = memcmp(key_cp->st, key->data(), key->size());
        if (key_equal == 0) {
            value = value_cp->st;
        } else {
            find = 0;
        }
        mtx_item->unlock();
    }
    return find;
}

}