#ifndef __META_DATA__
#define __META_DATA__

#include <atomic>
#include <unistd.h>
#include <string>


namespace polar_race {

class MetaData {
public:
    MetaData() {}
    bool init(const std::string &dir);
    int getTm();

private:
    int fd;
    std::atomic<int> tm;
    void *ptr;
    int *metaTm;
};

}

#endif //__META_DATA__