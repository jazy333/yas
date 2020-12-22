#include "hash_db.h"

#include <chrono>
#include <cstring>
#include <ctime>
#include <iostream>

using namespace std;

namespace yas {

constexpr char HashDB::META_MAGIC_DATA[8];
constexpr int32_t HashDB::METADATA_SIZE;
constexpr int32_t HashDB::META_OFFSET_PKG_MAJOR_VERSION;
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
constexpr int32_t HashDB::RECORD_BASE_ALIGN ;
static constexpr int32_t MIN_OFFSET_WIDTH = 3;
static constexpr int32_t MAX_OFFSET_WIDTH = 6;
static constexpr int32_t MAX_ALIGN_POW = 16;
static constexpr int64_t MAX_NUM_BUCKETS = 1099511627689LL;
static constexpr int32_t REBUILD_NONBLOCKING_MAX_TRIES = 3;
static constexpr int64_t REBUILD_BLOCKING_ALLOWANCE = 65536;

int HashDB::save_meta(bool finish) {
  char meta[METADATA_SIZE];
  std::memset(meta, 0, METADATA_SIZE);
  std::memcpy(meta, META_MAGIC_DATA, sizeof(META_MAGIC_DATA));
  memcpy(meta + META_OFFSET_PKG_MAJOR_VERSION, &major_version_, 1);
  memcpy(meta + META_OFFSET_PKG_MINOR_VERSION, &minor_version_, 1);
  memcpy(meta + META_OFFSET_STATIC_FLAGS, &static_flags_, 1);
  memcpy(meta + META_OFFSET_OFFSET_WIDTH, &offset_width_, 1);
  memcpy(meta + META_OFFSET_ALIGN_POW, &align_pow_, 1);
  uint8_t closure_flags = CLOSURE_FLAG_NONE;
  if ((closure_flags_ & CLOSURE_FLAG_CLOSE) && finish) {
    closure_flags |= CLOSURE_FLAG_CLOSE;
  }
  memcpy(meta + META_OFFSET_CLOSURE_FLAGS, &closure_flags, 1);
  memcpy(meta + META_OFFSET_NUM_BUCKETS, &num_buckets_, 8);
  int64_t num_records = num_records_.load();
  memcpy(meta + META_OFFSET_NUM_RECORDS, &num_records, 8);
  int64_t eff_data_size = eff_data_size_.load();
  memcpy(meta + META_OFFSET_EFF_DATA_SIZE, &eff_data_size, 8);
  memcpy(meta + META_OFFSET_FILE_SIZE, &file_size_, 8);
  memcpy(meta + META_OFFSET_MOD_TIME, &mod_time_, 8);
  memcpy(meta + META_OFFSET_DB_TYPE, &db_type_, 4);
  const int32_t opaque_size =
      std::min<int32_t>(opaque_.size(), METADATA_SIZE - META_OFFSET_OPAQUE);
  memcpy(meta + META_OFFSET_OPAQUE, opaque_.data(), opaque_size);
  return file_->write(0, meta, METADATA_SIZE);
}

int HashDB::load_meta() {
  char meta[METADATA_SIZE];
  int ret = file_->read(0, meta, METADATA_SIZE);
  if (ret != 0) {
    return ret;
  }
  if (std::memcmp(meta, META_MAGIC_DATA, sizeof(META_MAGIC_DATA)) != 0) {
    return -1;
  }
  memcpy(&major_version_, meta + META_OFFSET_PKG_MAJOR_VERSION, 1);
  memcpy(&minor_version_, meta + META_OFFSET_PKG_MINOR_VERSION, 1);
  memcpy(&static_flags_, meta + META_OFFSET_STATIC_FLAGS, 1);
  memcpy(&offset_width_, meta + META_OFFSET_OFFSET_WIDTH, 1);
  memcpy(&align_pow_, meta + META_OFFSET_ALIGN_POW, 1);
  memcpy(&closure_flags_, meta + META_OFFSET_CLOSURE_FLAGS, 1);
  memcpy(&num_buckets_, meta + META_OFFSET_NUM_BUCKETS, 8);
  /// num_records_.store(ReadFixNum(meta + META_OFFSET_NUM_RECORDS, 8));
  // eff_data_size_.store(ReadFixNum(meta + META_OFFSET_EFF_DATA_SIZE, 8));
  memcpy(&file_size_, meta + META_OFFSET_FILE_SIZE, 8);
  memcpy(&mod_time_, meta + META_OFFSET_MOD_TIME, 8);
  memcpy(&db_type_, meta + META_OFFSET_DB_TYPE, 4);
  opaque_ = std::string(meta + META_OFFSET_OPAQUE,
                        METADATA_SIZE - META_OFFSET_OPAQUE);
  if (major_version_ < 1 && minor_version_ < 1) {
    return -1;
  }
  if (!(static_flags_ & UPDATE_REPLACE) &&
      !(static_flags_ & UPDATE_APPENDING)) {
    return -1;
  }
  if (offset_width_ < MIN_OFFSET_WIDTH || offset_width_ > MAX_OFFSET_WIDTH) {
    return -1;
  }
  if (align_pow_ > MAX_ALIGN_POW) {
    return -1;
  }
  if (num_records_.load() < 0) {
    return -1;
  }
  if (eff_data_size_.load() < 0) {
    return -1;
  }
  if (file_size_ < METADATA_SIZE) {
    return -1;
  }
  if (num_buckets_ < 1 || num_buckets_ > MAX_NUM_BUCKETS) {
    return -1;
  }

  return 0;
}

int HashDB::init() {
  const int32_t align = std::max(RECORD_BASE_ALIGN, 1 << align_pow_);
  record_base_ = METADATA_SIZE + num_buckets_ * offset_width_ +
                 FBP_SECTION_SIZE + RECORD_BASE_HEADER_SIZE;
  record_base_ = round(record_base_, align);
  int64_t offset = METADATA_SIZE;
  int64_t size = record_base_ - offset;
  int ret = file_->truncate(record_base_);

  string empty(PAGE_SIZE, 0);
  while (size > 0) {
    const int64_t write_size = std::min<int64_t>(size, PAGE_SIZE);
    ret = file_->write(offset, empty);
    if (ret != 0) {
      break;
    }
    offset += write_size;
    size -= write_size;
  }

  closure_flags_ |= CLOSURE_FLAG_CLOSE;
  file_size_ = file_->size();
  mod_time_ = time(nullptr);
  ret = save_meta(true);
  return ret;
}

HashDB::HashDB() : file_(unique_ptr<File>(new MMapFile)) {}
int HashDB::open(const std::string& path) {
  std::cout << "db path:" << path << std::endl;
  file_->open(path, true);
  path_ = path;
  if (file_->size() == 0) {
    init();
  }

  load_meta();
  return 0;
}

int HashDB::close() {
  file_->close();
  return 0;
}

int HashDB::get(const std::string& key, std::string& value) { return 0; }
int HashDB::set(const std::string& key, const std::string& value) { return 0; }
int HashDB::del(const std::string& key) { return 0; }
size_t HashDB::size() { return 0; }
HashDB::~HashDB() { close(); }
}  // namespace yas