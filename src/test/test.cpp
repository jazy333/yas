//#include "murmur_hash2.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <x86intrin.h>

#include "bkd_tree.h"
#include "common.h"
#include "fnv_hash.h"
#include "hash_db.h"
#include "intro_sorter.h"
#include "memory_points.h"
#include "mmap_file.h"
#include "murmur_hash2.h"
#include "point.h"
#include "segment_mutex.h"
#include "shared_mutex.h"
#include "sortable_bytes.h"
#include "sorter.h"

using namespace yas;
using namespace std;

// for construct complete binary tree
int get_number_left_leaves(int number) {
  int last_level = 31 - __builtin_clz(number);
  int full_level = 1 << last_level;
  cout << "full:" << full_level << endl;
  int num_left = full_level / 2;
  int unbalanced = number - full_level;
  num_left += min(num_left, unbalanced);
  return num_left;
}

class TestSorter : public Sorter {
 public:
  TestSorter(initializer_list<int> v) : _data(v) {}

  void sort(void* cookie) { sort(0, _data.size(), cookie); }

  void select(int from, int to, int k, void* cookie) {}

  void print() {
    copy(_data.begin(), _data.end(), ostream_iterator<int>(cout, ","));
    cout << endl;
  }

  virtual void sort(int i, int j, void* cookie) override {
    binary_sort(i, j, cookie);
    // heap_sort(i,j);
  }
  virtual void swap(int i, int j) override {
    int tmp = _data[i];
    _data[i] = _data[j];
    _data[j] = tmp;
  }
  virtual bool compare(int i, int j, void* cookie) override {
    return _data[i] < _data[j];
  }

  int byte_at(int i, int pos, void* cookie) override { return 0; }

  int max_length() override { return sizeof(int); }

 private:
  vector<int> _data;
};

class TestIntroSorter : public IntroSorter {
 public:
  TestIntroSorter(initializer_list<int> v) : _data(v) {}

  void sort(void* cookie) { IntroSorter::sort(0, _data.size(), cookie); }

  void print() {
    copy(_data.begin(), _data.end(), ostream_iterator<int>(cout, ","));
    cout << endl;
  }

  virtual void swap(int i, int j) override {
    int tmp = _data[i];
    _data[i] = _data[j];
    _data[j] = tmp;
  }
  virtual bool compare(int i, int j, void* cookie) override {
    return _data[i] < _data[j];
  }

  int byte_at(int i, int pos, void* cookie) override { return 0; }

  int max_length() { return sizeof(int); }

 private:
  vector<int> _data;
};

int main(int argc, char* argv[]) {
  // MurmurHash2 mh2;
  // mh2.hash64("test",20181220);

  long l1 = 123123131, l2 = 123123123123, l3 = 9983838838, l4 = 999913123131;
  int i1 = -1, i2 = 1, i3 = 3, i4 = -3;
  double d1 = 0.11111134141, d2 = -0.3333333334242, d3 = -1.34324242,
         d4 = 9090909.1313131;
  float f1 = 3.34, f2 = 4.321, f3 = -0.89, f4 = -1000.0;
  u_char c4[4];
  u_char c8[8];
  sortable_bytes_encode(i1, c4);
  for (int i = 0; i < 4; ++i) {
    // cout<<"index "<<i<<":"<<"byte:"<<hex<<bytes[i]<<endl;
    printf("i:%d,v:%x\n", i, c4[i] & 0xff);
  }

  int i11;
  sortable_bytes_decode(c4, i11);
  cout << "i11:" << i11 << endl;

  sortable_bytes_encode(i2, c4);
  for (int i = 0; i < 4; ++i) {
    // cout<<"index "<<i<<":"<<"byte:"<<hex<<bytes[i]<<endl;
    printf("i:%d,v:%x\n", i, c4[i] & 0xff);
  }

  sortable_bytes_decode(c4, i11);
  cout << "i12:" << i11 << endl;

  sortable_bytes_encode(i3, c4);
  for (int i = 0; i < 4; ++i) {
    // cout<<"index "<<i<<":"<<"byte:"<<hex<<bytes[i]<<endl;
    printf("i:%d,v:%x\n", i, c4[i] & 0xff);
  }
  sortable_bytes_decode(c4, i11);
  cout << "i13:" << i11 << endl;

  sortable_bytes_encode(i4, c4);
  for (int i = 0; i < 4; ++i) {
    // cout<<"index "<<i<<":"<<"byte:"<<hex<<bytes[i]<<endl;
    printf("i:%d,v:%x\n", i, c4[i] & 0xff);
  }
  sortable_bytes_decode(c4, i11);
  cout << "i14:" << i11 << endl;

  sortable_bytes_encode(f1, c4);
  for (int i = 0; i < 4; ++i) {
    // cout<<"index "<<i<<":"<<"byte:"<<hex<<bytes[i]<<endl;
    printf("i:%d,v:%x\n", i, c4[i] & 0xff);
  }
  float f11;
  sortable_bytes_decode(c4, f11);
  cout << "f11:" << f11 << endl;

  sortable_bytes_encode(f2, c4);
  for (int i = 0; i < 4; ++i) {
    // cout<<"index "<<i<<":"<<"byte:"<<hex<<bytes[i]<<endl;
    printf("i:%d,v:%x\n", i, c4[i] & 0xff);
  }

  sortable_bytes_decode(c4, f11);
  cout << "f12:" << f11 << endl;

  sortable_bytes_encode(f3, c4);
  for (int i = 0; i < 4; ++i) {
    // cout<<"index "<<i<<":"<<"byte:"<<hex<<bytes[i]<<endl;
    printf("i:%d,v:%x\n", i, c4[i] & 0xff);
  }

  sortable_bytes_decode(c4, f11);
  cout << "f13:" << f11 << endl;

  sortable_bytes_encode(l1, c8);
  for (int i = 0; i < 8; ++i) {
    // cout<<"index "<<i<<":"<<"byte:"<<hex<<bytes[i]<<endl;
    printf("i:%d,v:%x\n", i, c8[i] & 0xff);
  }
  long l11;
  sortable_bytes_decode(c8, l11);
  cout << "l1:" << l1 << endl;
  cout << "l11:" << l11 << endl;

  sortable_bytes_encode(l2, c8);
  for (int i = 0; i < 8; ++i) {
    // cout<<"index "<<i<<":"<<"byte:"<<hex<<bytes[i]<<endl;
    printf("i:%d,v:%x\n", i, c8[i] & 0xff);
  }
  l11;
  sortable_bytes_decode(c8, l11);
  cout << "l2:" << l2 << endl;
  cout << "l11:" << l11 << endl;

  sortable_bytes_encode(d1, c8);
  for (int i = 0; i < 8; ++i) {
    // cout<<"index "<<i<<":"<<"byte:"<<hex<<bytes[i]<<endl;
    printf("i:%d,v:%x\n", i, c8[i] & 0xff);
  }
  double d11;
  sortable_bytes_decode(c8, d11);
  cout << "d1:" << d1 << endl;
  cout << "d11:" << d11 << endl;

  sortable_bytes_encode(d2, c8);
  for (int i = 0; i < 8; ++i) {
    // cout<<"index "<<i<<":"<<"byte:"<<hex<<bytes[i]<<endl;
    printf("i:%d,v:%x\n", i, c8[i] & 0xff);
  }
  sortable_bytes_decode(c8, d11);
  cout << "d12:" << d11 << endl;
  TestSorter ts({1, 2, 3, 1});
  ts.print();
  ts.sort(nullptr);
  ts.print();

  TestIntroSorter tis({1,  34,  5,    6,   6,     44,  333, 15, 15,
                       15, 51,  5333, 137, 56456, 8,   2,   4,  7,
                       0,  100, 100,  100, 200,   399, 1399});
  tis.print();
  tis.sort(nullptr);
  // tis.heap_sort(0,25,0);
  // tis.quick_sort(0,25,0);
  tis.print();

  Point<int, 2> p({1, 2}, 1);
  Point<int, 3> p3({1, 2, 3}, 2);
  cout << "sizeof point 2 int:" << sizeof(p) << ",0:" << p.get(0)
       << ",1:" << p.get(1) << endl;
  u_char* bytes = p.bytes();
  for (int i = 0; i < p.bytes_size(); ++i) {
    // cout<<"index "<<i<<":"<<"byte:"<<hex<<bytes[i]<<endl;
    printf("i:%d,v:%x\n", i, bytes[i]);
  }

  Point<int, 2> p2 = p;
  cout << "sizeof point2 2 int:" << sizeof(p) << ",0:" << p2.get(0)
       << ",1:" << p2.get(1) << endl;
  Point<int, 2> p4({3, 4}, 2);
  cout << "p4:" << p4 << endl;
  Point<int, 2> p5({0, 257}, 4);
  Point<int, 2> p7({9, 3}, 6);
  Point<int, 2> p8({-40, -58}, 200);
  Point<int, 2> p9({-92, -32}, 201);
  Point<int, 2> p10({3, 4}, 202);
  Point<int, 2> p11({9, -8}, 200);
  Point<int, 2> p12({10, 6}, 201);
  Point<int, 2> p13({4, -10}, 202);
  Point<int, 2> p14({-2, -2}, 203);
  Point<int, 2> p15({-6, -1}, 204);
  Point<int, 2> p16({-4, -1}, 205);
  //[9,-8]#200||[10,6]#201||[4,-10]#202||[-2,-2]#203||[-6,-1]#204||[-4,-1]#205||
  MemoryPoints<int, 2> mps;
  mps.write(p);
  mps.write(p7);
  mps.write(p4);
  mps.write(p5);
  mps.write(p8);
  mps.write(p9);
  mps.write(p10);
  mps.write(p11);
  mps.write(p12);
  mps.write(p13);
  mps.write(p14);
  mps.write(p15);
  mps.write(p16);
#if 1
  std::default_random_engine random(time(nullptr));
  std::uniform_int_distribution<int> dis1(-10, 10);
  for (int i = 0; i < 1111; ++i) {
    int x = dis1(random);
    int y = dis1(random);
    Point<int, 2> p({x, y}, i + 200);
    mps.write(p);
  }
#endif
  int sorted_dim = 1;
  int mid = mps.size() / 2;
  cout << "before select" << endl;
  for (int i = 0; i < mps.size(); ++i) {
    cout << mps.get(i) << "||";
  }
  cout << endl;
  mps.select(0, mps.size(), mps.size() / 2, &sorted_dim);
  cout << "mid after select :" << mps.get(mid);
  cout << "after select" << endl;
  for (int i = 0; i < mps.size(); ++i) {
    cout << mps.get(i) << "||";
  }
  cout << endl;
  cout << "mps size:" << mps.size() << endl;
  Point<int, 2> min, max;
  mps.minmax(min, max);

  cout << "min:" << min << endl;
  cout << "max:" << max << endl;

  Point<int, 2> p6 = p4 - p5;
  cout << "diff:" << p6 << endl;

  vector<int> prefix_lens(2, 4);
  mps.common_prefix_lengths(prefix_lens);
  cout << "prefix lens:" << endl;
  copy(prefix_lens.begin(), prefix_lens.end(),
       ostream_iterator<int>(cout, ","));
  cout << endl;

  BkdTree<int, 2> bkd_out;
  File* kdm = new MMapFile();
  kdm->open("data/yas.kdm", true);
  File* kdi = new MMapFile();
  kdi->open("data/yas.kdi", true);
  File* kdd = new MMapFile();
  kdd->open("data/yas.kdd", true);
  int field_id = 1;
  bkd_out.pack(field_id, &mps, kdm, kdi, kdd);
  // kdm end
  kdm->write_vint(0);
  kdm->close();

  Point<int, 2> low({1, 1}, -1);
  Point<int, 2> high({5, 5}, -1);
  File* kdm_in = new MMapFile();
  kdm_in->open("data/yas.kdm", false);
  BkdTree<int, 2> bkd_in(kdm_in, kdi, kdd);
  vector<int> result_docids;
  bkd_in.intersect(field_id, low, high, kdi, kdd, result_docids);
  cout << "interfect result:";
  for (int i = 0; i < result_docids.size(); ++i) {
    cout << result_docids[i] << ",";
  }
  cout << endl;
  cout << "expect result:";
  for (int i = 0; i < mps.size(); ++i) {
    Point<int, 2> cur = mps.get(i);
    if (cur <= high && cur >= low) {
      cout << cur.get_docid() << ",";
    }
  }

  cout << endl;

#if 0
  for (auto iter = mps.begin();iter != mps.end();++iter) {
    cout << "iter dump:" << *iter << endl;
  }
#endif

  cout << "gcd()" << gcd(24, 36) << endl;
  cout << "lcm()" << lcm(36, 24) << endl;
  cout << "number of left leaves: " << get_number_left_leaves(31) << endl;
  exit(0);
  cout << "hello yas" << endl;
  File* mf = new MMapFile();
  int ret = mf->open("yas.hdb", true);
  cout << "mmfile open ret:" << ret << endl;
  (mf)->write(static_cast<int64_t>(-1), string("test for mmapfile"));
  string str;
  mf->read(0, str, 5);
  cout << "read out:" << str << endl;
  mf->append(string("test for append"), nullptr);
  cout << "file size:" << mf->size() << endl;
  mf->truncate(1033);
  cout << "mf size:" << mf->size() << endl;

  mf->close();
  HashDB hdb;
  hdb.open("data/yas1.hdb");
  cout << "=====================" << endl;
  int status = hdb.set("test2", "test for first key1");
  cout << "set staus:" << status << ",size:" << hdb.size() << endl;
  cout << "=====================" << endl;
  string value;
  hdb.get("test1", value);
  cout << "get value test1:" << value << endl;
  cout << "=====================" << endl;
  hdb.get("test2", value);
  cout << "get value test2:" << value << endl;
  cout << "=====================" << endl;
  status = hdb.get("key not exists", value);
  cout << "get status:" << status << endl;
  cout << "=====================" << endl;
  status = hdb.del("test2");
  cout << "del status:" << status << ",size:" << hdb.size() << endl;
  cout << "=====================" << endl;
  hdb.get("test1", value);
  cout << "get value test1:" << value << endl;
  cout << "=====================" << endl;
  value.clear();
  status = hdb.get("test2", value);
  cout << "status:" << status << ",get value test2:" << value << endl;
  cout << "=====================" << endl;
  status = hdb.set("xxx", "sogou.com");
  cout << "set status:" << status << ",size:" << hdb.size() << endl;
  cout << "=====================" << endl;
  status = hdb.set("yyy", "sogou.com1");
  cout << "set status:" << status << ",size:" << hdb.size() << endl;

  status = hdb.test("3418257432248451726");
  cout << "test 3418257432248451726 status:" << status << endl;

  status = hdb.test("test2");
  cout << "test test2 status:" << status << endl;

#if 0
  ifstream ifs;
  ifs.open(argv[1]);
  string line;

  while (getline(ifs, line)) {
    cout << "line:" << line << endl;
    size_t start = line.find_first_of('\t');
    string key = line.substr(0, start);
    // string value = line.substr(start + 1);
    //cout << "value size:" << value.size() << endl;
    string value;
    hdb.get(key, value);
    cout << "bulk get key:" << key << ",value:" << value << endl;
    // hdb.set(key, value);
  }
  ifs.close();
#endif

  hdb.rebuild();
#if 0
  //HashDB hdb1;
  //hdb1.open("data/yas.hdb.1");
  auto iter = hdb.make_iterator();
  iter->first();
  do {
    string key, value;
    iter->get(key, value);
    cout << "iter:" << key << ",value:" << value << endl;
    // hdb1.set(key,value);
  } while (iter->next() != -1);

#endif
  hdb.close();

  MurmurHash2 mh2;
  int64_t h = mh2.hash64("test for murmur hash", 20181220);
  cout << "mm2 hash:" << h << endl;
  h = mh2.hash64("test for murmur hash1", 20181220);
  cout << "mm2 hash:" << h << endl;
  FNVHash fnv;
  int64_t h1 = fnv.hash64("The quick brown fox jumps over the lazy dog.");
  cout << "fnv hash:" << hex << h1 << endl;

  SharedMutex sm;
  sm.lock();
  sm.unlock();
  sm.lock_shared();
  sm.unlock_shared();

  SegmentSharedMutex<SharedMutex> sm1;
  sm1.lock(0);
  sm1.unlock(0);
  SegmentSharedMutex<SharedMutex> sm2 = move(sm1);
  cout << "sm1 coutn:" << sm1.get_num_slots()
       << ",sm2 count:" << sm2.get_num_slots() << endl;
  cout << "slots:" << sm1.get_num_slots() << endl;

  auto hash = [=](string key, uint64_t buckets) -> uint64_t {
    return (fnv.hash64(key)) % buckets;
  };

  SegmentHashSharedMutex<SharedMutex, string> shm(10000000, 128, hash);
  shm.lock("test");
  shm.unlock("test");
  shm.lock_shared("shared");
  shm.unlock_shared("shared");
  cout << "test ScopedSegmentHashSharedMutex" << endl;
  ScopedSegmentHashSharedMutex<SegmentHashSharedMutex<SharedMutex, string>> ssh(
      shm, "test", true);
  ScopedSegmentHashSharedMutex<SegmentHashSharedMutex<SharedMutex, string>>
      ssh1(shm, "test", false);
  return 0;
}