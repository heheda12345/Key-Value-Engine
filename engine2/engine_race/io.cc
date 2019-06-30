#include "io.h"

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "constants.h"
#include "util.h"
namespace polar_race {

IO* IO::instance = 0;
IO* IO::Instance() {
    if (instance == 0) {
        instance = new IO();
    }
    return instance;
}

bool openOrCreate(const std::string& name, int& fd, int fileSize = 0, char** ptr=nullptr) {
    fd = open(name.c_str(), O_RDWR);
    if (fd == -1) {
        fd = open(name.c_str(), O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
        if (fileSize) {
            int x = ftruncate(fd, fileSize);
            if (x == -1) {
                printf("warning: %s resize error", name.c_str());
            }
            if (ptr != nullptr)
                *ptr=(char*)mmap(NULL, fileSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        }
        return false;
    } else {
        if (fileSize && ptr != nullptr)
            *ptr=(char*)mmap(NULL, fileSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        return true;
    }
}

//hash
//1024 * (4B(length) + 4096B(content)) = 2^10 * 2^13
//log
//2^6*2^18*7B/2^20 = 2^4*7 = 112M
//6B: 2B value>0 4B key
void IO::init(const std::string &dir) {
    bool fd_hash_exist = openOrCreate(dir + "/hash.txt", fd_hash);
    buffer = new char[BUFFER_SIZE];
    int maxValueID = 0;
    if (fd_hash_exist) {
        printf("dir exist!\n");
        valueTotal = 0;
        uint32_t len;
        while (1) {
            if (!utilRead(fd_hash, &len, 4))
                break;
            if (!utilRead(fd_hash, buffer, len+5))
                break;
            if (buffer[len+4] != tag)
                break;
            int valueID = *(int*)(buffer+len);
            // printf("valueID %d\n", valueID);
            stArray->st[valueID].assign(buffer, len);
            // printf("value %s\n", stArray->st[valueID].c_str());
            uint64_t valueSample = sampleValue(buffer, len);
            hash->insert(valueSample, valueID);
            maxValueID = std::max(valueID, maxValueID);
        }
    }
    valueTotal.store(maxValueID);
    bool fd_log_exist = openOrCreate(dir + "/log.txt", fd_log, LOG_SIZE, &logPtr);
    logCnt.store(0);
    if (fd_log_exist) {
        while (1) {
            int cur = logCnt.fetch_add(1);
            char st[10];
            memcpy(st, logPtr+cur*9, 9);
            // st[9] = '\0'; printf("cur %d get %s\n", cur, st);
            if (st[8] != '#') {
                logCnt.fetch_sub(1);
                break;
            }
            uint32_t keyID = *(uint32_t*)st;
            uint32_t valueID = *(uint32_t*)(st+4);
            bucket->insert(keyID, valueID);
            // printf("bucket insert %u %u\n", keyID, valueID);
       }
    }
    delete buffer;
}

uint32_t IO::newValue(const char* st, uint32_t len) {
    int valueID = valueTotal.fetch_add(1)+1;
    char buffer[len+9];
    memcpy(buffer, &len, 4);
    memcpy(buffer+4, st, len);
    memcpy(buffer+len+4, &valueID, 4);
    memcpy(buffer+len+8, &tag, 1);
    utilWrite(fd_hash, buffer, len+9);
    return valueID;
}

void IO::newKey(uint32_t keyID, uint32_t valueID) {
    int cur = logCnt.fetch_add(1);
    char buffer[9];
    memcpy(buffer, &keyID, 4);
    memcpy(buffer+4, &valueID, 4);
    memcpy(buffer+8, &tag, 1);
    memcpy(logPtr+9*cur, buffer, 9);
    // printf("logPtr %x cur %d buf %s\n", logPtr, cur, buffer);
}

}