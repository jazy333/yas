//#include "murmur_hash2.h"
#include <iostream>
#include <string>

#include "hash_db.h"
#include "mmap_file.h"

using namespace yas;
using namespace std;
int main() {
  // MurmurHash2 mh2;
  // mh2.hash64("test",20181220);
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
  hdb.close();

  return 0;
}