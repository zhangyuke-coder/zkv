
#include "iterator.h"

namespace zkv {

Iterator::Iterator() {
  cleanup_head_.function = nullptr;
  cleanup_head_.next = nullptr;
}

Iterator::~Iterator() {
  if (!cleanup_head_.IsEmpty()) {
    cleanup_head_.Run();
    for (CleanupNode* node = cleanup_head_.next; node != nullptr;) {
      node->Run();
      CleanupNode* next_node = node->next;
      delete node;
      node = next_node;
    }
  }
}

void Iterator::RegisterCleanup(CleanupFunction func, void* arg1, void* arg2) {
  assert(func != nullptr);
  CleanupNode* node;
  if (cleanup_head_.IsEmpty()) {
    node = &cleanup_head_;
  } else {
    node = new CleanupNode();
    node->next = cleanup_head_.next;
    cleanup_head_.next = node;
  }
  node->function = func;
  node->arg1 = arg1;
  node->arg2 = arg2;
}

namespace {

class EmptyIterator : public Iterator {
 public:
  EmptyIterator(const DBStatus& s) : status_(s) {}
  ~EmptyIterator() override = default;

  bool Valid() const override { return false; }
  void Seek(const std::string_view& target) override {}
  void SeekToFirst() override {}
  void SeekToLast() override {}
  void Next() override { assert(false); }
  void Prev() override { assert(false); }
  std::string_view key() const override {
    assert(false);
    return std::string_view();
  }
  std::string value() override {
    static const std::string kEmpty;
    return kEmpty;
  }
  DBStatus status() const override { return status_; }

 private:
  DBStatus status_;
};

}  // anonymous namespace

Iterator* NewEmptyIterator() { return new EmptyIterator(Status::kSuccess); }

Iterator* NewErrorIterator(const DBStatus& status) {
  return new EmptyIterator(status);
}
}