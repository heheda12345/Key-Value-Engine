// Copyright [2018] Alibaba Cloud All rights reserved
#ifndef ENGINE_RACE_ENGINE_RACE_H_
#define ENGINE_RACE_ENGINE_RACE_H_
#include <string>
#include "include/engine.h"
#include "hash_table.h"
#include "database.h"
#include "constants.h"
#include "meta_data.h"
#include "hash_table.h"
#include "hot_table.h"

namespace polar_race {

class EngineRace : public Engine  {
 public:
  static RetCode Open(const std::string& name, Engine** eptr);

  explicit EngineRace(const std::string& dir)  {
  }

  RetCode Write(const PolarString& key_,
      const PolarString& value_) override;

  RetCode Read(const PolarString& key,
      std::string* value) override;

  /*
   * NOTICE: Implement 'Range' in quarter-final,
   *         you can skip it in preliminary.
   */
  RetCode Range(const PolarString& lower,
      const PolarString& upper,
      Visitor &visitor) override;

 private: 
    std::string dir, root;

    // HashTable memTable;
    // WriteAheadLog wal;
    Database db;
    Logger logger;
    std::mutex mtx[1<<LOCK_GROUP_LEN];
    MetaData mtd;
    HashTable htb;
    HotTable hot;
    
};

}  // namespace polar_race

#endif  // ENGINE_RACE_ENGINE_RACE_H_
