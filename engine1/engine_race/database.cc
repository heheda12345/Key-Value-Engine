#include "database.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>
namespace polar_race{

Database::Location Database::insert(const PolarString &key, const PolarString &value) {
    int x = getFreeFile();

    Location ret = (Location){id[x], offset[x]};
    uint32_t key_len = key.size();
    Write(fd[x], &key_len, sizeof(key_len), offset[x]);
    uint32_t value_len = value.size();
    Write(fd[x], &value_len, sizeof(value_len), offset[x]);
    Write(fd[x], key.data(), key_len, offset[x]);
    // printf("write_key %llu\n", *(key.data()));
    Write(fd[x], value.data(), value_len, offset[x]);
    
    returnFile(x);
    return ret;
}

bool Database::get(uint64_t mini_key, const PolarString &key, Location loc, std::string &value) {
    int fd = open(getFileName(loc.fileID).c_str(), O_RDONLY);
    // printf("fd %s %d\n", getFileName(loc.fileID).c_str(), fd);
    uint32_t key_len, value_len;
    lseek(fd, loc.offset, SEEK_SET);
    bool ret;
    char* key_get;
    ret = Read(fd, &key_len, sizeof(key_len));
    if (!ret) {
        printf("fail to read key_len at %d %d\n", loc.fileID, loc.offset);
        goto RETURN_FALSE;
    }
    // printf("keylen %u\n", key_len);
    ret = Read(fd, &value_len, sizeof(value_len));
    if (!ret) {
        printf("fail to read value_len\n");
        goto RETURN_FALSE;
    }
    // printf("keylen %u\n", value_len);
    key_get = new char[key_len+1];
    ret = Read(fd, key_get, key_len);
    if (!ret) {
        printf("fail to read key\n");
        goto RETURN_FALSE;
    }
    key_get[key_len] = '\0';
    if (memcmp(key.data(), key_get, key_len) == 0) {
        char* value_get = new char[value_len+1];
        ret = read(fd, value_get, value_len);
        if (!ret) {
            printf("fail to read value\n");
            delete[] value_get;
            goto RETURN_FALSE;
        }
        value_get[value_len] = '\0';
        value = PolarString(value_get, value_len).ToString();
        delete[] value_get;
        goto RETURN_TRUE;
    }
    printf("fd %d len %d\n", fd, key.size());
    printf("%llu\n", *(key.data()));
    printf("%llu\n", *(key_get));
    printf("key conflict %llu %llu\n", get_mini_key(key), get_mini_key(PolarString(key_get, key_len)));
    goto RETURN_FALSE;

RETURN_TRUE:
    close(fd);
    delete[] key_get;
    return true;
RETURN_FALSE:
    close(fd);
    delete[] key_get;
    return false;
}

Logger::Location Logger::insert(int time, const PolarString& key, Logger::Location loc, uint64_t mini_key) {
    int x = getFreeFile();
    
    Location ret = (Location){id[x], offset[x]};

    Write(fd[x], &time, sizeof(time), offset[x]);
    uint32_t key_len = key.size();
    Write(fd[x], &key_len, sizeof(key_len), offset[x]);
    Write(fd[x], &loc.fileID, sizeof(loc.fileID), offset[x]);
    Write(fd[x], &loc.offset, sizeof(loc.offset), offset[x]);
    Write(fd[x], &mini_key, sizeof(mini_key), offset[x]);
    Write(fd[x], key.data(), key_len, offset[x]);

    returnFile(x);
    return ret;

}


}