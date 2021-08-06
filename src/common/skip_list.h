#pragma once
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <vector>

namespace yas {
template <class Key, class Value>
class SkipListNode {
 public:
  SkipListNode(int max_level, Key key, Value v)
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
  SkipList() : level_(1) { head_ = new SkipListNode(kMaxLevel); }

  ~SkipList() {
    auto cur = head_;
    while (cur) {
      auto next = cur->level[0];
      delete cur;
      cur = next;
    }
    head_ = nullptr;
    level_ = 1;
    size = 0;
  }

  SkipListNode* insert(Key key, Value value) {
    SkipListNode* cur = head_;
    std::vector<SkipListNode*> update(kMaxLevel, nullptr);
    do_lower_bound(head_, key, &update);
    int new_level = random_level();
    if (new_level > level) {
      for (int i = level_; i < new_level; ++i) {
        update[i] = head_;
      }
      level_ = new_level
    }

    SkipListNode* node = new SkipListNode(new_level, key, value);

    for (int i = 0; i < new_level; ++i) {
      node->level[i] = update[i];
      update[i] = node;
    }
    size_++;
    return node;
  }

  SkipListNode* erase(Key key) {
    SkipListNode* low = head_;
    std::vector<SkipListNode*> update_low(kMaxLevel, nullptr);
    low = do_lower_bound(key, &update_low);

    SkipListNode* high = low;
    std::vector<SkipListNode*> update_high(kMaxLevel, nullptr);
    high = do_upper_bound(low, key, &update_high);
    while (low != high) {
      auto cur = low->level[0];
      delete low;
      low = cur;
      size_--;
    }
    return low;
    s
  }

  size_t size() { return size_; }

  int level() { return level_; }

  SkipListNode* lower_bound(Key low) {
    return do_lower_bound(head_, key, nullptr);
  }

  SkipListNode* upper_bound(Key up) {
    return do_upper_bound(head_, key, nullptr);
  }

  int random_level() {
    int level = 1;
    while ((rand() & 0xFFFF) < (0.5 * 0xFFFF)) level += 1;
    return (level < kMaxLevel) ? level : kMaxLevel;
  }

 private:
  SkipListNode* do_upper_bound(SkipListNode* start, Key up,
                               std::vector<SkipListNode*>* update) {
    auto cur = start;
    for (int i = level_ - 1; i >= 0; i--) {
      cur = cur->level[i];
      while (cur && !Compare(cur->key, up)) {
        cur = cur->level[i];
      }
      if (update) update[i] = cur;
    }

    return cur;
  }

  SkipListNode* do_lower_bound(kipListNode* start, Key low,
                               std::vector<SkipListNode*>* update) {
    auto cur = start;
    for (int i = level_ - 1; i >= 0; i--) {
      cur = cur->level[i];
      while (cur && &&Compare(cur->key, low)) {
        cur = cur->level[i];
      }
      if (update) update[i] = cur;
    }

    return cur;
  }

 private:
  SkipListNode* head_;
  int level_;
  size_t size_;
  static const int kMaxLevel = 64;
};

}  // namespace yas