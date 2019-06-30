#include <assert.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include "include/engine.h"
#include "test_util.h"

using namespace polar_race;

#define KV_CNT 10000

struct Answer {
    int cnt;
    std::vector<int> key_len;
    std::vector<int> value_len;
    std::vector<char*> key;
    std::vector<char*> value;
};


void output(const Answer &a) {
    printf("tot: %d\n", a.cnt);
    for (int i=0; i<a.cnt; i++) {
        for (int j=0; j<a.key_len[i]; j++)
            printf("%x ", a.key[i][j]);
        printf("\n");
        for (int j=0; j<a.value_len[i]; j++)
            putchar(a.value[i][j]);
        printf("\n");
    }
}

bool checkEqual(const std::vector<char*> &a_st, const std::vector<int> & a_len,
                const std::vector<char*> &b_st, const std::vector<int> & b_len) {
    if (a_len.size() != b_len.size())
        return 0;
    for (unsigned i=0; i<a_len.size(); i++) {
        if (a_len[i] != b_len[i])
            return 0;
        for (int j=0; j<a_len[i]; j++)
            if (a_st[i][j] != b_st[i][j])
                return 0;
    }
    return 1;
}

bool operator == (const Answer &a, const Answer &b) {
    // output(a);
    // output(b);
    if (a.cnt != b.cnt)
        return 0;
    for (int i=0; i<a.cnt; i++) {
        if (!checkEqual(a.key, a.key_len, b.key, b.key_len))
            return 0;
        if (!checkEqual(a.value, a.value_len, b.value, b.value_len))
            return 0;
    }
    return 1;    
}

class MyVisitor : public Visitor {
 public:
  virtual ~MyVisitor() {
      for (int i=0; i<ans.cnt; i++) {
          delete[] ans.key[i];
          delete[] ans.value[i];
      } 
  }

  virtual void Visit(const PolarString &key, const PolarString &value) {
      char* buf_key = new char[key.size()];
      memcpy(buf_key, key.data(), key.size());
      char* buf_value = new char[value.size()];
      memcpy(buf_value, value.data(), value.size());
      ans.key.push_back(buf_key);
      ans.value.push_back(buf_value);
      ans.key_len.push_back(key.size());
      ans.value_len.push_back(value.size());
      ans.cnt++;
  }
  Answer ans;
};

struct Item {
    char key[8];
    char value[1050];
};


bool operator < (const Item &a, const Item &b) {
    for (int i=0; i<8; i++)
        if (a.key[i] < b.key[i])
            return 1;
        else if (a.key[i] > b.key[i])
            return 0;
    return 0;
}

Answer ans;
// char k[1024];
// char v[9024];
// std::string ks[KV_CNT];
// std::string vs_1[KV_CNT];
// std::string vs_2[KV_CNT];
int main() {

    Engine *engine = NULL;
    printf_(
        "======================= range test "
        "============================");
    std::string engine_path =
        std::string("./data/test-") + std::to_string(asm_rdtsc());
    RetCode ret = Engine::Open(engine_path, &engine);
    assert(ret == kSucc);
    printf("open engine_path: %s\n", engine_path.c_str());

    /////////////////////////////////
    
    Item a[10];
    for (int i=0; i<10; i++) {
        uint64_t x = i*100;
        memcpy(a[i].key, &x, 8);
        gen_random(a[i].value, 1024);
        engine->Write(PolarString(a[i].key, 8), PolarString(a[i].value, 1024));
    }
    std::sort(a, a+10);
    // printf("after sort\n");
    // for (int i=0; i<10; i++) {
    //     for (int j=0; j<8; j++)
    //         printf("%x ", a[i].key[j]);
    //     printf("\n");
    //     printf("%s\n", a[i].value);
    // }
    for (int i=0; i<10; i++)
        for (int j=i+1; j<10; j++) {
            MyVisitor vis1, vis2;
            vis1.ans.cnt = vis2.ans.cnt = 0;
            for (int k=i; k<j; k++)
                vis1.Visit(PolarString(a[k].key, 8), PolarString(a[k].value, 1024));
            engine->Range(PolarString(a[i].key, 8), PolarString(a[j].key, 8), vis2);
            assert(vis1.ans == vis2.ans);
        }
    printf_(
        "======================= range test pass :) "
        "======================");

    return 0;
}
