#include <cstdio>
#include "zipf.h"

using namespace std;

int main() {
    zipf_gen_state state;
    mehcached_zipf_init(&state, 10, 0.99, 0);
    for (int i=0; i<100; i++) {
        uint64_t x = mehcached_zipf_next(&state);
        printf("%llu ", x);
    }
}