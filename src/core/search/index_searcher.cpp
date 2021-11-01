#include "index_searcher.h"

#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <thread>

#include "common/common.h"
#include "common/shared_lock.h"
#include "log.h"
#include "matched_doc.h"
#include "matcher.h"

namespace yas {
IndexSearcher::IndexSearcher(std::shared_ptr<IndexReader> reader)
    : reader_(reader) {}

IndexSearcher::IndexSearcher() : reader_(nullptr) {}

IndexSearcher::~IndexSearcher() {}

void IndexSearcher::search(Query* q, MatchSet& set, int max_size) {
  SharedLock<SharedMutex> lock(shared_mutex_);
  if (!reader_) return;
  std::vector<std::shared_ptr<SubIndexReader>> sub_index_readers =
      reader_->get_sub_index_readers();
  for (auto&& sub_index_reader : sub_index_readers) {
    auto matcher = q->matcher(sub_index_reader.get());
    auto pl = matcher->posting_list();
    auto scorer = matcher->scorer();
    auto field_values_reader = sub_index_reader->field_values_reader();
    auto id_reader = field_values_reader->get_reader("id");
    while (pl->next() != NDOCID) {
      MatchedDoc md;
      md.docid_ = pl->docid();
      md.score_ = scorer->score();
      md.field_value_reader = field_values_reader;
      auto hits = pl->hit();

      std::string id;
      if (id_reader) {
        std::vector<uint8_t> value;
        id_reader->get(md.docid_, value);
        id.assign(value.begin(), value.end());
      }
      // std::cout << "docid:" << md.docid_ << ",score:" << md.score_
      //        << ",id:" << id << std::endl;
      for (auto&& kv : hits) {
        md.match_term_counts[kv.first] = kv.second;
        std::string doc_len_field = "__" + kv.first + "_dl";
        auto doc_len_reader = field_values_reader->get_reader(doc_len_field);
        if (doc_len_reader) {
          uint64_t norm = 0;
          doc_len_reader->get(md.docid_, norm);
          norm = uchar2uint(norm);
          md.doc_lens[kv.first] = norm;
        }
      }
      set.add(md);
      if (set.size() >= max_size) return;
    }
  }
}

std::shared_ptr<Query> IndexSearcher::rewrite(std::shared_ptr<Query> query) {
  std::shared_ptr<Query> original = query;
  do {
    original = query;
    query = query->rewrite();
  } while (query.get() != original.get());
  return query;
}

void IndexSearcher::search(Query* q, MatchSet& set) {
  search(q, set, std::numeric_limits<int>::max());
}

void IndexSearcher::set_reader(std::shared_ptr<IndexReader> reader) {
  shared_mutex_.lock();
  if (reader_) reader_->close();
  reader_ = reader;
  shared_mutex_.unlock();
}

// std::shared_ptr<IndexReader> IndexSearcher::get_reader() { return reader_; }

IndexStat IndexSearcher::get_index_stat() {
  SharedLock<SharedMutex> lock(shared_mutex_);
  IndexStat stat;
  if (reader_)
    return reader_->get_index_stat();
  else
    return stat;
}

void IndexSearcher::auto_reload() {
  LOG_INFO("create a auto reload thread");
  if (!reader_) LOG_ERROR("reader is null auto reload starts failed");
  auto option = reader_->get_option();
  auto commit_file = option.get_index_commit_file();

  if (access(commit_file.c_str(), F_OK) == -1) {
    int ret = mkfifo(commit_file.c_str(), 0777);
    if (ret != 0) {
      LOG_ERROR("can not creat fifo file:%s,mode 777\n", commit_file.c_str());
      return;
    }
  }

  int commit_fd = open(commit_file.c_str(), O_RDONLY | O_NONBLOCK);
  int epoll_fd = epoll_create(200);
  if (epoll_fd < 0) {
    LOG_ERROR("create epoll error:%s\n", strerror(errno));
    return;
  }
  LOG_INFO("epoll_fd=%d,cmmmit fd=%d,commit_file=%s\n", epoll_fd, commit_fd,
           commit_file.c_str());
  int result;
  struct epoll_event event;
  memset(&event, 0, sizeof(event));
  event.events = EPOLLIN | EPOLLET;
  event.data.fd = commit_fd;
  result = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, commit_fd, &event);
  struct epoll_event events[16];
  while (true) {
    int nevent = epoll_wait(epoll_fd, events, 16, -1);
    LOG_INFO("get %d epoll events", nevent);
    char sig = 0;
    for (int i = 0; i < nevent; i++) {
      LOG_INFO("%dth,event: 0x%x,fd=%d,commit_fd=%d", i, events[i].events,
               events[i].data.fd, commit_fd);
      int len = read_with_check(events[i].data.fd, &sig, sizeof(sig));
      if (len < 0) {
        LOG_ERROR("read fifo error:%s", strerror(errno));
      }
      LOG_INFO("get fifo data: len=%d,sig=%d", len, sig);
      if (sig == -1) {
        LOG_INFO("got a finish signal");
        break;
      }
      if (sig == 1) {
        LOG_INFO("got a reload signal");
        auto reader = std::make_shared<IndexReader>(option);
        int ret = reader->open();

        if (ret == 0) {
          LOG_INFO("reader reload successfully!");
          set_reader(reader);
        } else {
          LOG_INFO("reader reload failed!");
        }
      }
    }
  }
  close(commit_fd);
  close(epoll_fd);
  LOG_INFO("auto_reload exists");
}

int IndexSearcher::enable_auto_reload() {
  std::thread reload(&IndexSearcher::auto_reload, this);
  reload.detach();
  return 0;
}

void IndexSearcher::lock_shared() { shared_mutex_.lock_shared(); }

void IndexSearcher::unlock_shared() { shared_mutex_.unlock_shared(); }

}  // namespace yas