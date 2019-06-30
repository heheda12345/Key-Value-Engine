#include "file_group.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>
namespace polar_race{

void FileGroup::init(std::string dir_) {
    dir = dir_;
    int startID = FileNameArranger::Instance() -> arrange(MAX_THREAD);
    for (int i=0; i<MAX_THREAD; i++) {
        id[i] = i + startID;
        std::string fileName = getFileName(id[i]);
        fd[i] = open(fileName.c_str(), O_CREAT|O_RDWR|O_APPEND, S_IRUSR|S_IWUSR);
        offset[i] = 0;
        q.push(i);
    }    
}

FileGroup::~FileGroup() {
    for (int i=0; i<MAX_THREAD; i++)
        close(fd[i]);
}

void FileGroup::openNew(int x) {
    close(fd[x]);
    id[x] = FileNameArranger::Instance() -> arrange(1);
    fd[x] = open(getFileName(id[x]).c_str(), O_CREAT|O_RDWR|O_APPEND, S_IRUSR|S_IWUSR);
    offset[x] = 0;
}

int FileGroup::getFreeFile() {
    q_mux.lock();
    int x = q.front();
    q.pop();
    q_mux.unlock();
    return x;
}

void FileGroup::returnFile(int x) {
    if (offset[x] > maxFileSize)
        openNew(x);
    q_mux.lock();
    q.push(x);
    q_mux.unlock();
}

}