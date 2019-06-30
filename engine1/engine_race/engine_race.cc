// Copyright [2018] Alibaba Cloud All rights reserved
#include "engine_race.h"

#include "util.h"

namespace polar_race {

RetCode Engine::Open(const std::string& name, Engine** eptr) {
  return EngineRace::Open(name, eptr);
}

Engine::~Engine() {
}

/*
 * Complete the functions below to implement you own engine
 */

// 1. Open engine
RetCode EngineRace::Open(const std::string& name, Engine** eptr) {
  // printf("dir %s\n", name.c_str());
  // printf("==============================================================================\n");
  Mkdir(name);
  Mkdir(name+"/log");
  Mkdir(name+"/dab");
  *eptr = NULL;
  EngineRace *engine_race = new EngineRace(name);
  FileNameArranger::Instance()->init(name);
  bool isOldEngine = engine_race -> mtd.init(name);
  if (isOldEngine) {
    // printf("isOld!\n");
    engine_race -> htb.Load(name+"/log");
  }
  engine_race -> db.init(name+"/dab");
  engine_race -> logger.init(name+"/log");
  *eptr = engine_race;
  return kSucc;
}

// 2. Close engine
// EngineRace::~EngineRace() {
// }

// 3. Write a key-value pair into engine
RetCode EngineRace::Write(const PolarString& key, const PolarString& value) {
    // printf("write %s %s\n", key.data(), value.data());
    // PolarString *key = deepCopy(key_); // maybe I can delete it?
    // PolarString *value = deepCopy(value_);
    uint64_t mini_key = get_mini_key(key);
    // printf("minikey %llu\n", mini_key);
    Database::Location loc = db.insert(key, value);
    // printf("write to loc %d %d\n", loc.fileID, loc.offset);
    int group = (mini_key >> LOCK_GROUP_BIAS) & ((1 << LOCK_GROUP_LEN) - 1);
    int timeStamp = mtd.getTm();
    mtx[group].lock();
    logger.insert(timeStamp, key, loc, mini_key);
    int tm = htb.SetItem(mini_key, loc);
    // printf("tm %d\n", tm);
    hot.insert(tm, mini_key, &key, &value);
    mtx[group].unlock();

  return kSucc;
}

// 4. Read value of a key
RetCode EngineRace::Read(const PolarString& key, std::string* value) {
  // printf("read %s\n", key.data());
  uint64_t mini_key = get_mini_key(key);
  // printf("minikey %llu\n", mini_key);
  if (hot.get(mini_key, &key, *value)) {
    return kSucc;
  }
  Database::Location loc;
  if (!htb.GetItem(mini_key, loc)) {
    return kNotFound;
  }
  // printf("loc %d %d\n", loc.fileID, loc.offset);
  if (db.get(mini_key, key, loc, *value)) {
    // printf("ret: %s\n", value->c_str());
    return kSucc;
  }
  return kNotFound;
}

/*
 * NOTICE: Implement 'Range' in quarter-final,
 *         you can skip it in preliminary.
 */
// 5. Applies the given Vistor::Visit function to the result
// of every key-value pair in the key range [first, last),
// in order
// lower=="" is treated as a key before all keys in the database.
// upper=="" is treated as a key after all keys in the database.
// Therefore the following call will traverse the entire database:
//   Range("", "", visitor)
RetCode EngineRace::Range(const PolarString& lower, const PolarString& upper,
    Visitor &visitor) {
  return kSucc;
}

}  // namespace polar_race
