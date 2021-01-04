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
constexpr int32_t HashDB::RECORD_BASE_ALIGN;
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
  record_mutex_.set_num_buckets(num_buckets_);
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

int64_t HashDB::read_bucket_index(int index) {
  int offset = METADATA_SIZE + index * offset_width_;
  string buf;
  int ret = file_->read(offset, buf, offset_width_);
  int64_t new_offset = 0;
  memcpy(&new_offset, buf.data(), offset_width_);
  return new_offset;
}
int HashDB::write_bucket_index(int index, int64_t offset) {
  int64_t bucket_offset = METADATA_SIZE + index * offset_width_;
  return file_->write(bucket_offset, &offset, offset_width_);
}

int HashDB::find_record(int64_t bottom_offset, const string& key,
                        int64_t& parent_offset, int64_t& current_offset,
                        int64_t& child_offset, string* value) {
  current_offset = bottom_offset;
  int status = 0;
  cout << "bottom offset:" << bottom_offset << endl;
  while (current_offset > 0) {
    string buf;
    status = file_->read(current_offset, buf, sizeof(record_header));
    if (status != 0) return status;
    const record_header* rh = (record_header*)(const_cast<char*>(buf.data()));
    cout << "rh keysize:" << rh->key_size_
         << ",child offset:" << rh->child_offset_ << endl;
    if (key.size() == rh->key_size_) {
      cout << "key size equals" << endl;
      string current_key;
      status = file_->read(current_offset + sizeof(record_header), current_key,
                           rh->key_size_);
      if (status != 0) {
        return status;
      }
      cout << "current_key:" << current_key << endl;
      if (key == current_key) {
        child_offset = rh->child_offset_;
        if (value) {
          cout << "rh valuesize:" << rh->value_size_ << endl;
          status = file_->read(
              current_offset + sizeof(record_header) + rh->key_size_, *value,
              rh->value_size_);
          cout << "value:" << *value << endl;
        }

        return status;
      }
      parent_offset = current_offset;
    }

    current_offset = rh->child_offset_;
  }

  return status;
}

int HashDB::write_record(char type, const std::string& key,
                         const std::string& value, int64_t* offset) {
  size_t base_size = sizeof(int) + offset_width_ + 2 * sizeof(int64_t) +
                     key.size() + value.size();
  const int32_t align = 1 << align_pow_;
  size_t real_size = round(base_size, align);
  char* buf = new char[real_size];
  cout << "base_size:" << base_size << ",real_size:" << real_size << endl;
  char* start = buf;
  record_header* rh = (record_header*)buf;
  rh->type_ = type;
  rh->child_offset_ = *offset;

  size_t key_size = key.size();
  rh->key_size_ = key_size;
  cout << "key size:" << key_size << endl;
  size_t value_size = value.size();
  rh->value_size_ = value_size;
  buf += sizeof(record_header);
  mempcpy(buf, key.c_str(), key_size);
  buf += key_size;
  mempcpy(buf, value.c_str(), value_size);
  buf += value_size;

  int status = file_->append(start, real_size, offset);
  delete[] start;
  return status;
}

int HashDB::delete_record(const std::string& key, int64_t old_offset,
                          int64_t parent_offset) {
  int64_t current_offset = 0, child_offset = 0;
  int status = find_record(old_offset, key, parent_offset, current_offset,
                           child_offset, nullptr);
  if (status == 0 && current_offset != 0) {
    status = write_child_fffset(parent_offset, child_offset);
  }
  return status;
}

int HashDB::write_child_fffset(int64_t offset, int64_t child_offset) {
  offset += sizeof(int);
  return file_->write(offset, &child_offset, offset_width_);
}

HashDB::HashDB()
    : file_(unique_ptr<File>(new MMapFile)),
      num_buckets_(DEFAULT_NUM_BUCKETS),
      offset_width_(8),
      align_pow_(8),
      record_mutex_(1, 128, primary_hash) {}
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

int HashDB::get(const std::string& key, std::string& value) {
  int64_t parent_offset = 0, current_offset = 0, child_offset = 0;
  ScopedSegmentHashSharedMutex<SegmentHashSharedMutex<SharedMutex, string>>
      scoped_lock(record_mutex_, key, false);
  int bucket_index = scoped_lock.get_bucket_index();
  int64_t offset = read_bucket_index(bucket_index);
  int64_t old_offset = offset;

  int status = find_record(old_offset, key, parent_offset, current_offset,
                           child_offset, &value);
  std::cout << "status:" << status << ",current_offset" << current_offset
            << ",offset:" << offset << ",bucket_index:" << bucket_index
            << std::endl;
  if (current_offset == 0) {
    return 1;
  }
  return status;
}

int HashDB::set(const std::string& key, const std::string& value) {
  ScopedSegmentHashSharedMutex<SegmentHashSharedMutex<SharedMutex, string>>
      scoped_lock(record_mutex_, key, true);
  int bucket_index = scoped_lock.get_bucket_index();
  int64_t offset = read_bucket_index(bucket_index);
  int64_t old_offset = offset;
  int64_t parent_offset = 0;

  int status = write_record(0, key, value, &offset);
  std::cout << "status:" << status << ",offset:" << offset
            << ",old offset:" << old_offset << ",bucket_index:" << bucket_index
            << std::endl;
  if (status == 0) {
    status = write_bucket_index(bucket_index, offset);
    if (status == 0) status = delete_record(key, old_offset, offset);
  }
  return status;
}

int HashDB::del(const std::string& key) { return 0; }
size_t HashDB::size() { return num_records_.load(); }
HashDB::~HashDB() { close(); }
}  // namespace yas