#ifndef __ST_ARRAY__
#define __ST_ARRAY__

#include <string>
#include "constants.h"
namespace polar_race {
struct StArray {
    std::string* st;
    void init() {
        st = new std::string[TOT_VALUE_TYPE];
    }
    void insert(uint32_t valueID, const char* st) {
        this->st[valueID] = std::string(st);
    }
    void extract(uint32_t valueID, std::string* target) {
        *target = st[valueID];
    }
};
}

#endif