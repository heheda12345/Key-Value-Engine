#ifndef __DATABASE__
#define __DATABASE__

#include <queue>
#include <string>
#include <mutex>
#include <unistd.h>
#include "include/polar_string.h"
#include "util.h"
#include "constants.h"
#include "file_group.h"

namespace polar_race {

class Database: public FileGroup {
public:
    Database(): FileGroup(".dab", DAB_MAX_SIZE) {};
    Location insert(const PolarString &key, const PolarString &value);
    bool get(uint64_t mini_key, const PolarString &key, Location loc, std::string &value);
};

class Logger: public FileGroup {
public:
    Logger(): FileGroup(".log", LOG_MAX_SIZE) {};
    Location insert(int time, const PolarString& key, Location loc, uint64_t mini_key);
};

}

#endif //__DATABASE__