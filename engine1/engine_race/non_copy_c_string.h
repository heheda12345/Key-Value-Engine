#ifndef __NON_COPY_C_STRING__
#define __NON_COPY_C_STRING__

#include <cstring>
namespace polar_race {
struct NonCopyCString {
    NonCopyCString(unsigned size_, const char* st_): size(size_){
        st = new char[size+1];
        memcpy(st, st_, size);
        st[size] = '\0';
    }

    NonCopyCString(const NonCopyCString&) = delete;
    NonCopyCString& operator = (const NonCopyCString&) = delete;

    ~NonCopyCString() {
        delete[] st;
    }

    unsigned size;
    char* st;
};
}
#endif //__NON_COPY_C_STRING__