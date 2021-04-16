//#include "murmur_hash2.h"
#include <fstream>
#include <iostream>
#include <string>


#include "common.h"
#include "fnv_hash.h"
#include "hash_db.h"
#include "mmap_file.h"
#include "murmur_hash2.h"
#include "segment_mutex.h"
#include "shared_mutex.h"
#include "point.h"
#include "memory_points.h"
#include "sorter.h"
#include "intro_sorter.h"

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

class TestSorter :public Sorter {
public:
  TestSorter(initializer_list<int> v) :_data(v) {
  }

  void sort() {
    sort(0, _data.size());
  }

  void print() {
    copy(_data.begin(), _data.end(), ostream_iterator<int>(cout, ","));
    cout<<endl;
  }

  virtual void sort(int i, int j) override {
    binary_sort(i, j);
    //heap_sort(i,j);
  }
  virtual void swap(int i, int j) override {
    int tmp = _data[i];
    _data[i] = _data[j];
    _data[j] = tmp;
  }
  virtual int compare(int i, int j) override {
    return _data[i] - _data[j];
  }
private:
  vector<int> _data;
};


class TestIntroSorter :public IntroSorter {
public:
  TestIntroSorter(initializer_list<int> v) :_data(v) {
  }

  void sort() {
    IntroSorter::sort(0, _data.size());
  }

  void print() {
    copy(_data.begin(), _data.end(), ostream_iterator<int>(cout, ","));
    cout<<endl;
  }

  virtual void swap(int i, int j) override {
    int tmp = _data[i];
    _data[i] = _data[j];
    _data[j] = tmp;
  }
  virtual int compare(int i, int j) override {
    return _data[i] - _data[j];
  }
private:
  vector<int> _data;
};



int main(int argc, char* argv[]) {
  // MurmurHash2 mh2;
  // mh2.hash64("test",20181220);

  TestSorter ts({ 1,2,3,1});
  ts.print();
  ts.sort();
  ts.print();

  TestIntroSorter tis({1,34,5,6,6,44,333,15,15,15,51,5333,137,56456,8,2,4,7,0,100,100,100,200,399,1399});
  tis.print();
  tis.sort();
  tis.print();

  Point<int, 2> p({ 1,2 }, 1);
  Point<int, 3> p3({ 1,2,3 }, 2);
  cout << "sizeof point 2 int:" << sizeof(p) << ",0:" << p[0] << ",1:" << p[1] << endl;
  u_char* bytes = p.bytes();
  for (int i = 0;i < p.bytes_size();++i) {
    //cout<<"index "<<i<<":"<<"byte:"<<hex<<bytes[i]<<endl;
    printf("i:%d,v:%x\n", i, bytes[i]);
  }

  Point<int, 2> p2 = p;
  cout << "sizeof point2 2 int:" << sizeof(p) << ",0:" << p2[0] << ",1:" << p2[1] << endl;

  Point<int, 2> p4({ 3,4 }, 2);
  cout << "p4:" << p4 << endl;
  Point<int, 2> p5({ 0,257 }, 4);
  MemoryPoints<int, 2> mps;
  mps.write(p);
  mps.write(p2);
  mps.write(p4);
  mps.write(p5);

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
  copy(prefix_lens.begin(), prefix_lens.end(), ostream_iterator<int>(cout, ","));
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