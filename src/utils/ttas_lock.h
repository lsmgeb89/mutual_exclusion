#ifndef LOCK_COMPARE_TTAS_LOCK_H_
#define LOCK_COMPARE_TTAS_LOCK_H_

#include <atomic>

namespace utils {

class TTASLock {
 public:
  TTASLock(void)
    : lock_(false) {}

  void Lock(void) {
    while(true) {
      while(atomic_load(&lock_)) {};
      if (!std::atomic_exchange(&lock_, true)) {
        break;
      }
    }
  }

  void Unlock(void) {
    std::atomic_store(&lock_, false);
  }

 public:
  static constexpr auto name_ = "TTASLock";

 private:
  std::atomic<bool> lock_;
};

} // namespace utils

#endif // LOCK_COMPARE_TTAS_LOCK_H_
