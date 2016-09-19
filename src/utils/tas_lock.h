#ifndef LOCK_COMPARE_TAS_LOCK_H_
#define LOCK_COMPARE_TAS_LOCK_H_

#include <atomic>
#include <string>

namespace utils {

class TASLock {
 public:
  TASLock(void)
    : lock_(ATOMIC_FLAG_INIT) {}

  void Lock(void) {
    while(lock_.test_and_set(std::memory_order_acquire));
  }

  void Unlock(void) {
    lock_.clear(std::memory_order_release);
  }

 public:
  static constexpr auto name_ = "TASLock";

 private:
  std::atomic_flag lock_;
};

} // namespace utils

#endif // LOCK_COMPARE_TAS_LOCK_H_
