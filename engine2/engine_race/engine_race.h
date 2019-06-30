// Copyright [2018] Alibaba Cloud All rights reserved
#ifndef ENGINE_RACE_ENGINE_RACE_H_
#define ENGINE_RACE_ENGINE_RACE_H_
#include <string>
#include <mutex>
#include "include/engine.h"
#include "bucket.h"
#include "hash.h"
#include "st_array.h"

namespace polar_race {

class EngineRace : public Engine  {
 public:
  static RetCode Open(const std::string& name, Engine** eptr);

  explicit EngineRace(const std::string& dir) {
  }

  ~EngineRace();

  RetCode Write(const PolarString& key,
      const PolarString& value) override;

  RetCode Read(const PolarString& key,
      std::string* value) override;

  /*
   * NOTICE: Implement 'Range' in quarter-final,
   *         you can skip it in preliminary.
   * WARNING: Only support key in bench
   */
  RetCode Range(const PolarString& lower,
      const PolarString& upper,
      Visitor &visitor) override;

 private: 
    Bucket bucket;
    Hash hash;
    StArray stArray;
    std::mutex* mtx;

};

}  // namespace polar_race

#endif  // ENGINE_RACE_ENGINE_RACE_H_
