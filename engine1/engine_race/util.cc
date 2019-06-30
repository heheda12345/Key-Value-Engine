#include "include/polar_string.h"
#include "util.h"

#include <unistd.h>
#include <atomic>
#include <dirent.h>
#include <cstdio>

namespace polar_race {

uint64_t get_mini_key(const PolarString& st)
{
    int l = st.size();
    uint64_t key = 131;
    for (int i=0; i<l; i++)
        key = key * 2333 + st[i];
    if (key == 0)
        key = 2333333;
    if (key == 1)
        key = 2333333333333ull;
    return key;
}

PolarString* deepCopy(const PolarString& st) {
    char* buffer = new char[st.size() + 1];
    memcpy(buffer, st.data(), st.size());
    PolarString* ret = new PolarString(buffer, st.size());
    return ret;
}

bool Read(int fd, void* buffer, size_t size) {
    int retCode = read(fd, buffer, size);
    // if (retCode != size) {
    //     printf("ret %d %d\n", retCode, errno);
    // }
    return retCode == size;
}

void Write(int fd,const void* buffer, size_t size, int& offset) {
    int ret = write(fd, buffer, size);
    offset += ret;
}

void Mkdir(std::string dir) {
    if (access(dir.c_str(), F_OK) != 0) {
        int ret = mkdir(dir.c_str(), 0755);
        if (ret != 0) {
            printf("mkdir fail\n");
        }
  }
}
FileNameArranger* FileNameArranger::instance = 0;

FileNameArranger* FileNameArranger::Instance() {
    if (instance == 0) {
        instance = new FileNameArranger();
    }
    return instance;
}

void FileNameArranger::init(std::string dir) {
    DIR* ds;
    dirent* dep;
    int max_num = 0;
    ds = opendir((dir+"/log").c_str());
    while ((dep = readdir(ds)) != NULL) {
        char *name = new char[256];
        strcpy(name, dep->d_name);
        name[strlen(dep->d_name) - 4] = '\0';
        int x = atoi(name);
        max_num = std::max(max_num, x);
        delete[] name;
    }
    closedir(ds);
    ds = opendir((dir+"/dab").c_str());
    while ((dep = readdir(ds)) != NULL) {
        char *name = new char[256];
        strcpy(name, dep->d_name);
        name[strlen(dep->d_name) - 4] = '\0';
        int x = atoi(name);
        max_num = std::max(max_num, x);
        delete[] name;
    }
    closedir(ds);
    used.store(max_num);
    // printf("max_num %d\n", max_num);
}

void FileNameArranger::init(uint32_t used_) {
    used.store(used_);
}

int FileNameArranger::arrange(uint32_t need) {
    int last = used.fetch_add(need);
    return last+1;
}

}