#pragma once

#include <pthread.h>

#include <atomic>
#include <mutex>

#include "common/common.h"
#include "common/const.h"
#include "common/segment_mutex.h"
#include "common/shared_mutex.h"
#include "core/algorithm/murmur_hash2.h"

#include "db.h"
#include "mmap_file.h"

namespace yas {

     uint64_t primary_hash(std::string key,uint64_t buckets){
      MurmurHash2 mh2;
    return (mh2.hash64(key,20181220))%buckets;
};

    struct record_header{
        int  type_;
        uint64_t child_offset_;
        size_t key_size_;
        size_t value_size_;
    };
class HashDB : public DB {
 public:
  static constexpr  char META_MAGIC_DATA[8] = "YASHDB\n";
  static constexpr int32_t METADATA_SIZE = 128;
  static constexpr int32_t META_OFFSET_PKG_MAJOR_VERSION = 8;
  static constexpr int32_t META_OFFSET_PKG_MINOR_VERSION = 9;
  static constexpr int32_t META_OFFSET_STATIC_FLAGS = 10;
  static constexpr int32_t META_OFFSET_OFFSET_WIDTH = 11;
  static constexpr int32_t META_OFFSET_ALIGN_POW = 12;
  static constexpr int32_t META_OFFSET_CLOSURE_FLAGS = 13;
  static constexpr int32_t META_OFFSET_NUM_BUCKETS = 14;
  static constexpr int32_t META_OFFSET_NUM_RECORDS = 22;
  static constexpr int32_t META_OFFSET_EFF_DATA_SIZE = 30;
  static constexpr int32_t META_OFFSET_FILE_SIZE = 38;
  static constexpr int32_t META_OFFSET_MOD_TIME = 46;
  static constexpr int32_t META_OFFSET_DB_TYPE = 54;
  static constexpr int32_t META_OFFSET_OPAQUE = 62;
  static constexpr int32_t FBP_SECTION_SIZE = 1006;
  static constexpr int32_t RECORD_BASE_HEADER_SIZE = 16;
  static constexpr int32_t RECHEAD_OFFSET_OFFSET_WIDTH = 10;
  static constexpr int32_t RECHEAD_OFFSET_ALIGN_POW = 11;
  static constexpr int32_t RECORD_MUTEX_NUM_SLOTS = 128;
  static constexpr int32_t RECORD_BASE_ALIGN = 4096;
  static constexpr int32_t MIN_OFFSET_WIDTH = 3;
  static constexpr int32_t MAX_OFFSET_WIDTH = 6;
  static constexpr int32_t MAX_ALIGN_POW = 16;
  static constexpr int64_t MAX_NUM_BUCKETS = 1099511627689LL;
  static constexpr int32_t REBUILD_NONBLOCKING_MAX_TRIES = 3;
  static constexpr int64_t REBUILD_BLOCKING_ALLOWANCE = 65536;
  static constexpr int64_t DEFAULT_NUM_BUCKETS = 1048583;
  enum UpdateMode {
    /** To do replace writing. */
    UPDATE_REPLACE = 0,
    /** To do appending writing. */
    UPDATE_APPENDING = 1,
  };

  enum ClosureFlag : uint8_t {
    CLOSURE_FLAG_NONE = 0,
    CLOSURE_FLAG_CLOSE = 1 << 0,
  };

  HashDB();
  int open(const std::string& path) override;
  int close() override;
  int get(const std::string& key, std::string& value) override;
  int set(const std::string& key, const std::string& value) override;
  int del(const std::string& key) override;
  size_t size() override;
  ~HashDB();

 private:
  int init();
  uint64_t read_bucket_index(int );
  int write_bucket_index(int index,uint64_t offset);
  int write_record(char type,const std::string&,const std::string& value,uint64_t* offset);
  int delete_record(const std::string& key,uint64_t old_offset,uint64_t parent_offset);
  int find_record(uint64_t bottom_offset,const std::string& key,uint64_t& parent_offset,uint64_t& current_offset,uint64_t& child_offset,std::string* value);
  int write_child_fffset(uint64_t offset, uint64_t child_offset);
  int save_meta(bool);
  int load_meta();
 
  std::string path_;
  std::unique_ptr<File> file_;
  bool open_;
  bool writable_;
  bool healthy_;
  uint8_t major_version_;
  uint8_t minor_version_;
  uint8_t static_flags_;
  int32_t offset_width_;
  int32_t align_pow_;
  uint8_t closure_flags_;
  int64_t num_buckets_;
  std::atomic<int64_t> num_records_;
  std::atomic<int64_t> eff_data_size_;
  int64_t file_size_;
  int64_t mod_time_;
  uint32_t db_type_;
  std::string opaque_;
  int64_t record_base_;
  bool lock_mem_buckets_;
  SharedMutex mutex_;
  SegmentHashSharedMutex<SharedMutex,std::string> record_mutex_;
  std::mutex file_mutex_;
};
}  // namespace yas