#include "meta_data.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "constants.h"
#include "util.h"
namespace polar_race {

bool MetaData::init(const std::string &dir) {
    bool exist = (fd = open((dir + "/000.mtd").c_str(), O_RDWR)) != -1;
    if (!exist) {
        fd = open((dir + "/000.mtd").c_str(), O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
        int x = ftruncate(fd, META_LEN);
        if (x == -1) {
            printf("warning: metaData open error");
        }
    }
    ptr = mmap(NULL, META_LEN, PROT_READ|PROT_WRITE, MAP_SHARED , fd , 0);
    metaTm = static_cast<int*>(ptr);
    if (exist) {
        int x = *metaTm;
        *metaTm = x + 1050;
        tm.store(x+1050);
    } else {
        *metaTm = 1;
        tm.store(1);
    }
    return exist;
}

int MetaData::getTm() {
    int x = tm.fetch_add(1);
    if (x % 1000 == 0) {
        *metaTm = x;
    }
    return x;
}

}