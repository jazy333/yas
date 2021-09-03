#include "hash_db.h"

#include <chrono>
#include <cstring>
#include <ctime>
#include <iostream>

#include "common/shared_lock.h"

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
  cout << "save num records:" << num_records << endl;
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
  if (ret != METADATA_SIZE) {
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
  int64_t num_records = 0;
  memcpy(&num_records, meta + META_OFFSET_NUM_RECORDS, sizeof(int64_t));
  cout << "num_records:" << num_records << endl;
  num_records_.store(num_records);
  int64_t eff_data_size = 0;
  memcpy(&eff_data_size, meta + META_OFFSET_EFF_DATA_SIZE, sizeof(int64_t));
  eff_data_size_.store(eff_data_size);
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

void HashDB::set_record_base() {
  const int32_t align = std::max(RECORD_BASE_ALIGN, 1 << align_pow_);
  record_base_ = METADATA_SIZE + num_buckets_ * offset_width_ +
                 FBP_SECTION_SIZE + RECORD_BASE_HEADER_SIZE;
  record_base_ = round(record_base_, align);
}

int HashDB::init() {
  set_record_base();
  int64_t offset = METADATA_SIZE;

  int64_t size = record_base_ - offset;
  int ret = file_->truncate(record_base_);

  string empty(PAGE_SIZE, 0);
  while (size > 0) {
    const int64_t write_size = std::min<int64_t>(size, PAGE_SIZE);
    ret = file_->write(offset, empty.data(), write_size);
    if (ret <= 0) {
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
  LOG_ERROR_IF(ret < 0, "read offset");
  if (ret < 0) return ret;
  int64_t new_offset = 0;
  memcpy(&new_offset, buf.data(), offset_width_);
  return new_offset;
}

int HashDB::read_bucket_keys(int index, std::set<string>& keys) {
  int status = 0;

  if (index >= num_buckets_) return -1;
  ScopedSegmentHashSharedMutex<SegmentHashSharedMutex<SharedMutex, string>>
      scoped_lock(record_mutex_, index, false);
  int64_t current_offset = read_bucket_index(index);

  if (current_offset > 0) {
    while (current_offset > 0) {
      string buf;
      status = file_->read(current_offset, buf, sizeof(record_header));
      if (status <= 0) return status;
      const record_header* rh = (record_header*)(const_cast<char*>(buf.data()));
      cout << "rh keysize:" << rh->key_size_
           << ",child offset:" << rh->child_offset_ << endl;
      string current_key;
      status = file_->read(current_offset + sizeof(record_header), current_key,
                           rh->key_size_);
      LOG_ERROR_IF(status <= 0, "read current key error");
      int type = rh->type_;
      switch (type) {
        case 0:
          keys.insert(current_key);
          break;
        case 1:
          break;
        default:
          break;
      }
      current_offset = rh->child_offset_;
    }
  }

  return status;
}

int HashDB::write_bucket_index(int index, int64_t offset) {
  int64_t bucket_offset = METADATA_SIZE + index * offset_width_;
  return file_->write(bucket_offset, &offset, offset_width_);
}

int HashDB::write_child_offset(int64_t offset, int64_t child_offset) {
  offset += sizeof(int);
  return file_->write(offset, &child_offset, offset_width_);
}

int HashDB::find_record(int64_t bottom_offset, const string& key,
                        int64_t& parent_offset, int64_t& current_offset,
                        int64_t& child_offset, string* value,
                        size_t& old_value_size, bool& deleted) {
  current_offset = bottom_offset;
  int status = 0;
  // cout << "bottom offset:" << bottom_offset << endl;
  while (current_offset > 0) {
    string buf;
    status = file_->read(current_offset, buf, sizeof(record_header));
    if (status <= 0) return status;
    const record_header* rh = (record_header*)(const_cast<char*>(buf.data()));
    cout << "rh keysize:" << rh->key_size_
         << ",child offset:" << rh->child_offset_ << endl;
    int type = rh->type_;
    if (key.size() == rh->key_size_) {
      cout << "key size equals" << endl;
      string current_key;
      status = file_->read(current_offset + sizeof(record_header), current_key,
                           rh->key_size_);
      if (status <= 0) {
        LOG_ERROR("read key error");
        return status;
      }
      cout << "current_key:" << current_key << endl;
      if (key == current_key) {
        child_offset = rh->child_offset_;
        if (type == 1) {
          deleted = true;
          current_offset = 0;
          return status;
        }
        old_value_size = rh->value_size_;
        if (value) {
          cout << "rh valuesize:" << rh->value_size_ << endl;
          status = file_->read(
              current_offset + sizeof(record_header) + rh->key_size_, *value,
              rh->value_size_);
        }
        return status;
      }
      parent_offset = current_offset;
    }
    current_offset = rh->child_offset_;
  }

  return status;
}

int HashDB::append_record(char type, const std::string& key,
                          const std::string& value, int64_t* offset) {
  size_t base_size = sizeof(int) + offset_width_ + 2 * sizeof(int64_t) +
                     key.size() + value.size();
  const int32_t align = 1 << align_pow_;
  size_t real_size = round(base_size, align);
  unique_ptr<char[]> arr(new char[real_size]());
  char* buf = arr.get();
  record_header* rh = (record_header*)buf;
  rh->type_ = type;
  rh->child_offset_ = *offset;

  size_t key_size = key.size();
  rh->key_size_ = key_size;
  size_t value_size = value.size();
  rh->value_size_ = value_size;
  buf += sizeof(record_header);
  mempcpy(buf, key.c_str(), key_size);
  buf += key_size;
  mempcpy(buf, value.c_str(), value_size);
  buf += value_size;

  int status = file_->append(arr.get(), real_size, offset);
  return status;
}

int HashDB::delete_record(const std::string& key, int64_t old_offset,
                          int64_t parent_offset, int64_t& current_offset,
                          size_t& old_value_size) {
  int64_t child_offset = 0;
  bool deleted = false;
  int status = find_record(old_offset, key, parent_offset, current_offset,
                           child_offset, nullptr, old_value_size, deleted);
  if (status >= 0 && current_offset != 0) {
    status = write_child_offset(parent_offset, child_offset);
  }
  return status;
}

HashDB::HashDB()
    : file_(unique_ptr<File>(new MMapFile)),
      open_(false),
      writable_(false),
      healthy_(false),
      major_version_(0),
      minor_version_(1),
      static_flags_(UPDATE_APPENDING),
      offset_width_(4),
      align_pow_(8),
      closure_flags_(CLOSURE_FLAG_NONE),
      num_buckets_(DEFAULT_NUM_BUCKETS),
      num_records_(0),
      eff_data_size_(0),
      file_size_(0),
      mod_time_(0),
      db_type_(0),
      record_base_(0),
      lock_mem_buckets_(false),
      record_mutex_(1, 128, primary_hash) {}

int HashDB::open(const std::string& path) {
  std::cout << "db path:" << path << std::endl;
  file_->open(path, true);
  path_ = path;
  if (file_->size() == 0) {
    init();
  }
  load_meta();
  bool healthy = closure_flags_ & CLOSURE_FLAG_CLOSE;
  if (file_size_ != file_->size()) {
    healthy = false;
  }
  set_record_base();
  open_ = true;
  healthy_ = healthy;
  return 0;
}

int HashDB::close() {
  std::lock_guard<SharedMutex> lock(mutex_);
  if (open_) {
    file_size_ = file_->size();
    mod_time_ = time(nullptr);
    save_meta(true);
    file_->close();
  }
  cancel_iterators();
  open_ = false;
  return 0;
}

int HashDB::do_process(int type, const std::string& key,
                       const std::string& value, bool override) {
  ScopedSegmentHashSharedMutex<SegmentHashSharedMutex<SharedMutex, string>>
      scoped_lock(record_mutex_, key, true);
  int bucket_index = scoped_lock.get_bucket_index();
  int64_t offset = read_bucket_index(bucket_index);
  int64_t old_offset = offset;
  int64_t parent_offset = 0;

  int status = append_record(type, key, value, &offset);
  //std::cout << "status:" << status << ",offset:" << offset
    //      << ",old offset:" << old_offset << ",bucket_index:" << bucket_index
      //  << std::endl;
  int64_t current_offset = -1;
  size_t old_value_size = 0;

  if (status > 0) {
    // update the bucket index,the offset is the last record's initial size
    status = write_bucket_index(bucket_index, offset);
    if (status > 0) {
      // delete from the list when update previously
      status = delete_record(key, old_offset, offset, current_offset,
                             old_value_size);
    } else {
      LOG_ERROR("write bucket index error");
      return -1;
    }
  } else {
    LOG_ERROR("append_record error");
    return -1;
  }

  // std::cout << "current_offset after do process:" << current_offset
  //        << ",old_value_size:" << old_value_size << endl;
  if (status >= 0) {
    switch (type) {
      case 0: {
        if (current_offset == 0) {
          eff_data_size_ += key.size() + value.size();
          num_records_.fetch_add(1);
        } else {//exists
          eff_data_size_ += value.size() - old_value_size;
        }
        break;
      }
      case 1: {
        if (current_offset > 0) {
          num_records_.fetch_sub(1);
          eff_data_size_ -= (key.size() + old_value_size);
        };
        break;
      }
      default:
        break;
    }
  } else {
    return -1;
  }
  return 0;
}

int HashDB::do_read(int type, const std::string& key, std::string& value) {
  int64_t parent_offset = 0, current_offset = 0, child_offset = 0;
  ScopedSegmentHashSharedMutex<SegmentHashSharedMutex<SharedMutex, string>>
      scoped_lock(record_mutex_, key, false);
  int bucket_index = scoped_lock.get_bucket_index();
  int64_t offset = read_bucket_index(bucket_index);
  int64_t old_offset = offset;
  size_t old_value_size;

  int status = 0;
  bool deleted = false;
  switch (type) {
    case 2: {
      status = find_record(old_offset, key, parent_offset, current_offset,
                           child_offset, &value, old_value_size, deleted);
      break;
    }
    case 3: {
      status = find_record(old_offset, key, parent_offset, current_offset,
                           child_offset, nullptr, old_value_size, deleted);
      break;
    }
    default:
      break;
  }
  std::cout << "status:" << status << ",current_offset" << current_offset
            << ",offset:" << offset << ",bucket_index:" << bucket_index
            << std::endl;

  if (status < 0) return status;

  if (current_offset == 0) {
    if (deleted)
      return 2;
    else
      return 1;
  }
  return 0;
}

int HashDB::get(const std::string& key, std::string& value) {
  return do_read(2, key, value);
}

/*
*0:exsists
*1:not exsists
*2:delete
*/
int HashDB::test(const std::string& key) {
  string value;
  return do_read(3, key, value);
}

int HashDB::set(const std::string& key, const std::string& value,
                bool override) {
  return do_process(0, key, value, override);
}

int HashDB::del(const std::string& key) {
  string value;
  return do_process(1, key, value, true);
}

size_t HashDB::size() { return num_records_.load(); }

int HashDB::sync() {
  std::lock_guard<SharedMutex> lock(mutex_);
  int status = 0;
  if (!open_) {
    return -1;
  }
  if (!writable_) {
    return -1;
  }
  if (!healthy_) {
    return -1;
  }
  file_size_ = file_->size();
  mod_time_ = time(nullptr);
  status = save_meta(false);
  status = file_->sync();
  return status;
}

int HashDB::do_rebuild(HashDB* hdb, int64_t start_offset,
                       int64_t last_valid_offset) {
  const int32_t align = 1 << align_pow_;
  int64_t next_offset = last_valid_offset - align;
  size_t base_size = sizeof(record_header);
  while (next_offset > start_offset) {
    int status = 0;
    string buf;
    status = file_->read(next_offset, buf, base_size);
    record_header* rh = (record_header*)const_cast<char*>(buf.data());
    switch (rh->type_) {
      case 0: {
        // if (rh->child_offset_ < next_offset) {
        size_t real_size = rh->key_size_ + rh->value_size_ + base_size;
        real_size = round(real_size, align);
        if (last_valid_offset - next_offset >= real_size) {
          last_valid_offset = next_offset;
          string key, value;
          file_->read(next_offset + base_size, key, rh->key_size_);
          file_->read(next_offset + base_size + rh->key_size_, value,
                      rh->value_size_);
          hdb->set(key, value, false);
        }
        //}
        break;
      }
      case 1: {
        // if (rh->child_offset_ < next_offset) {
        size_t real_size = rh->key_size_ + rh->value_size_ + base_size;
        real_size = round(real_size, align);
        if (last_valid_offset - next_offset >= real_size) {
          last_valid_offset = next_offset;
          string key;
          file_->read(next_offset + base_size, key, rh->key_size_);
          if (hdb->test(key) == 1) {
            hdb->del(key);
          }
        }
        //}
        break;
      }
      default: {
        break;
      }
    }
    next_offset -= align;
  }
  return 0;
}

int HashDB::rebuild() {
  SharedLock<SharedMutex> lock(mutex_);
  int status = 0;
  HashDB hdb;
  string tmp_path = path_ + ".rebuild";
  hdb.open(tmp_path);
  const int32_t align = 1 << align_pow_;

  int64_t start_offset = record_base_;
  std::lock_guard<std::mutex> file_lock(file_mutex_);
  int count = 0;
  do {
    int64_t end_offset = file_->size();
    int64_t last_valid_offset = end_offset;
    if ((last_valid_offset - record_base_) % align != 0) {
      last_valid_offset = record_base_ + (last_valid_offset / align) * align;
      end_offset = last_valid_offset;
    }
    if (count == 3) {
      // lock all
      ScopedSegmentHashSharedMutex<SegmentHashSharedMutex<SharedMutex, string>>
          scoped_lock(record_mutex_, true);
      do_rebuild(&hdb, start_offset, last_valid_offset);

      break;
    } else {
      do_rebuild(&hdb, start_offset, last_valid_offset);
    }
    start_offset = end_offset;
    count++;
  } while (start_offset != file_->size());
  hdb.close();
  file_->close();
  rename(tmp_path.c_str(), path_.c_str());
  open(path_);
  return 0;
}

void HashDB::cancel_iterators() {
  for (auto iter : iters) {
    iter->reset_bucket_index();
  }
}

std::string HashDB::path() { return path_; }

bool HashDB::should_be_rebuilt() { return true; }

std::string HashDB::status() { return ""; }

HashDB::~HashDB() { close(); }

std::unique_ptr<DB::Iterator> HashDB::make_iterator() {
  return std::unique_ptr<Iterator>(new Iterator(this));
}

HashDB::Iterator::Iterator(HashDB* db) : db_(db), bucket_index_(-1) {
  std::lock_guard<SharedMutex> lock(db_->mutex_);
  db_->iters.emplace_back(this);
}

HashDB::Iterator::~Iterator() {
  std::lock_guard<SharedMutex> lock(db_->mutex_);
  if (db_) {
    db_->iters.remove(this);
  }
}

int HashDB::Iterator::read_keys() {
  if (bucket_index_ < 0) {
    return -1;
  }

  if (keys_.size() > 0) return 0;

  int status = -1;
  while (true) {
    status = db_->read_bucket_keys(bucket_index_, keys_);
    if (status < 0) return status;
    bucket_index_++;
    if (keys_.size() > 0) break;
  }
  return status;
}

int HashDB::Iterator::first() {
  SharedLock<SharedMutex> scope_shared_lock(db_->mutex_);
  keys_.clear();
  bucket_index_.store(0);
  read_keys();
  return 0;
}

int HashDB::Iterator::next() {
  SharedLock<SharedMutex> scope_shared_lock(db_->mutex_);
  keys_.erase(keys_.begin());
  int status = read_keys();
  return status;
}

int HashDB::Iterator::last() { return 0; }

int HashDB::Iterator::get(std::string& key, std::string& value) {
  if (keys_.empty()) return -1;
  auto it = keys_.begin();
  key = *it;
  int status = db_->get(key, value);
  return status;
}

void HashDB::Iterator::reset_bucket_index() { bucket_index_.store(-1); }

}  // namespace yas