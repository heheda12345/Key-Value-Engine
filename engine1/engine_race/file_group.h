#ifndef __FILE_GROUP__
#define __FILE_GROUP__

#include <queue>
#include <string>
#include <mutex>
#include <unistd.h>
#include "include/polar_string.h"
#include "util.h"
#include "constants.h"

namespace polar_race {
class FileGroup {
public:
    FileGroup(std::string suffName_, int maxFileSize_) :
        suffName(suffName_), maxFileSize(maxFileSize_) {};
    ~FileGroup();
    struct Location {
        int fileID, offset;
        bool operator == (const Location& b) {
            return fileID == b.fileID && offset == b.offset;
        }
    };
    void init(std::string dir);

protected:
    void openNew(int x);
    int getFreeFile();
    void returnFile(int x);
    std::string getFileName(int id) {
        return dir + '/' + std::to_string(id) + suffName;
    }
    std::queue<int> q;
    std::mutex q_mux;
    int fd[MAX_THREAD];
    int id[MAX_THREAD];
    int offset[MAX_THREAD];
    std::string suffName, dir;
    int maxFileSize;
};
}
#endif //__FILE_GROUP__