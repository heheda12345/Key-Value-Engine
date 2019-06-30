#ifndef __UTIL__
#define __UTIL__
#include <atomic>
#include <cstdio>
#include <string>
#include <sys/stat.h>
#include "include/polar_string.h"

namespace polar_race {

uint64_t get_mini_key(const polar_race::PolarString& st);

polar_race::PolarString* deepCopy(const polar_race::PolarString& st);

void Mkdir(std::string dir);
bool Read(int fd, void* buffer, size_t size);
void Write(int fd, const void* buffer, size_t size, int& offset);

class FileNameArranger {
private:
    static FileNameArranger* instance;
    FileNameArranger() = default;
    std::atomic<uint32_t> used;
    
public:
    static FileNameArranger* Instance();
    int arrange(uint32_t need = 1);
    void init(std::string dir);
    void init(uint32_t used_);
};


}
#endif //__UTIL__