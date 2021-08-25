#pragma once
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <vector>

namespace yas {
template <class Key, class Value>
class SkipListNode {
 public:
  SkipListNode(int max_level, Key key, Value value)
      : key(key), value(value), level(max_level, nullptr) {}
  SkipListNode(int max_level) : key(), value(), level(max_level, nullptr) {}
  SkipListNode* next() { return level[0]; }

 public:
  Key key;
  Value value;
  std::vector<SkipListNode*> level;
};

template <class Key, class Value, class Compare = std::less<Key>>
class SkipList {
 public:
  using node_type = SkipListNode<Key, Value>;

  SkipList() : level_(1), size_(0) { head_ = new node_type(kMaxLevel); }

  ~SkipList() {
    auto cur = head_;
    while (cur) {
      auto next = cur->level[0];
      delete cur;
      cur = next;
    }
    head_ = nullptr;
    level_ = 1;
    size_ = 0;
  }

  node_type* insert(Key key, Value value) {
    node_type* cur = head_;
    std::vector<node_type*> update(kMaxLevel, nullptr);
    do_lower_bound(head_, key, &update);
    int new_level = random_level();
    if (new_level > level_) {
      for (int i = level_; i < new_level; ++i) {
        update[i] = head_;
      }
      level_ = new_level;
    }

    node_type* node = new node_type(new_level, key, value);

    for (int i = 0; i < new_level; ++i) {
      node->level[i] = update[i]->level[i];
      update[i]->level[i] = node;
    }
    size_++;
    return node;
  }

  node_type* erase(Key key) {
    node_type* low = head_;
    std::vector<node_type*> update_low(kMaxLevel, nullptr);
    low = do_lower_bound(head_, key, &update_low);

    node_type* high = low;
    std::vector<node_type*> update_high(kMaxLevel, nullptr);
    high = do_upper_bound(low, key, &update_high);
    while (low != high) {
      auto cur = low->level[0];
      delete low;
      low = cur;
      size_--;
    }

    for (int i = 0; i < level_; ++i) {
      if (update_low[i]) {
        update_low[i]->level[i] = update_high[i];
      }
    }
    return low;
  }

  size_t size() { return size_; }

  int level() { return level_; }

  node_type* lower_bound(Key low) {
    return do_lower_bound(head_, low, nullptr);
  }

  node_type* upper_bound(Key up) { return do_upper_bound(head_, up, nullptr); }

  int random_level() {
    int level = 1;
    while ((rand() & 0xFFFF) < (0.5 * 0xFFFF)) level += 1;
    return (level < kMaxLevel) ? level : kMaxLevel;
  }

 private:
  node_type* do_lower_bound(node_type* start, Key low,
                            std::vector<node_type*>* update) {
    if (!start) return nullptr;
    auto cur = start, pre = start;
    int node_level =
        start->level.size() > level_ ? level_ : start->level.size();
    for (int i = node_level - 1; i >= 0; i--) {
      cur = pre;
      do {
        pre = cur;
        cur = cur->level[i];
      } while (cur && Compare()(cur->key, low));

      if (update) (*update)[i] = pre;
    }

    return cur;
  }

  node_type* do_upper_bound(node_type* start, Key up,
                            std::vector<node_type*>* update) {
    if (!start) return nullptr;
    int node_level =
        start->level.size() > level_ ? level_ : start->level.size();
    auto cur = start, pre = start;
    for (int i = node_level - 1; i >= 0; i--) {
      cur = pre;
      do {
        pre = cur;
        cur = cur->level[i];
      } while (cur && !Compare()(up, cur->key));
      if (update) (*update)[i] = cur;
    }

    return cur;
  }

 private:
  node_type* head_;
  int level_;
  size_t size_;
  static const int kMaxLevel;
};

template <class Key, class Value, class Compare>
const int SkipList<Key, Value, Compare>::kMaxLevel = 64;

}  // namespace yas