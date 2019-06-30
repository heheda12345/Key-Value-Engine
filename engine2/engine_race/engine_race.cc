// Copyright [2018] Alibaba Cloud All rights reserved
#include "engine_race.h"

#include <thread>

#include "io.h"
#include "util.h"
#include "constants.h"
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
  *eptr = NULL;
  EngineRace *engine_race = new EngineRace(name);

  utilMkdir(name);
  engine_race->bucket.init();
  engine_race->hash.init();
  engine_race->stArray.init();
  engine_race->mtx = new std::mutex[MAX_KEY_ID];
  IO::Instance()->setTarget(&engine_race->hash, &engine_race->bucket, &engine_race->stArray);
  IO::Instance()->init(name);
  printf("open end\n");
  *eptr = engine_race;
  return kSucc;
}

// 2. Close engine
EngineRace::~EngineRace() {
  delete[] mtx;
}

// 3. Write a key-value pair into engine
RetCode EngineRace::Write(const PolarString& key, const PolarString& value) {
  // printf("write\n");
  uint32_t valueID;
  uint64_t valueSample = sampleValue(value.data(), value.size());
  // printf("2\n");
  uint32_t keyID = keyToInt(key.data(), key.size());
  // printf("keyID %u\n", keyID);
  // printf("3\n");
  if (!hash.exist(valueSample, valueID)) {
    // printf("3-1\n");
    valueID = IO::Instance()->newValue(value.data(), value.size());
    // printf("3-2 %d\n", valueID);
    stArray.insert(valueID, value.data());
    // printf("3-3\n");
    hash.insert(valueSample, valueID);
  }
  // printf("4\n");
  mtx[keyID&LOCK_MASK].lock();
  // printf("5\n");
  IO::Instance()->newKey(keyID, valueID);
  // printf("6\n");
  bucket.insert(keyID, valueID);
  // printf("7\n");
  mtx[keyID&LOCK_MASK].unlock();
  // printf("write end %u %u\n", keyID, valueID);
  return kSucc;
}

// 4. Read value of a key
RetCode EngineRace::Read(const PolarString& key, std::string* value) {
  // printf("read %s\n", key.data());
  // printf("9\n");
  uint32_t keyID = keyToInt(key.data(), key.size());
  // printf("10\n");
  uint32_t valueID = bucket.find(keyID);
  // printf("valueID : %u\n", valueID);
  if (!valueID)
    return kNotFound;
  else
    stArray.extract(valueID, value);
  return kSucc;
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
  uint32_t lowerKeyID = lower=="" ? 0 : keyToInt(lower.data(), lower.size());
  uint32_t upperKeyID = upper=="" ? MAX_KEY_ID : keyToInt(upper.data(), upper.size());
  bucket.traverse(lowerKeyID, upperKeyID, &stArray, visitor);
  return kSucc;
}

}  // namespace polar_race
