#ifndef LOCK_COMPARE_PETERSON_LOCK_H_
#define LOCK_COMPARE_PETERSON_LOCK_H_

#include <atomic>

namespace utils {

class PetersonLock {
 public:
  PetersonLock(void) {
    std::atomic_store_explicit(&victim_, static_cast<std::size_t>(0), std::memory_order_release);
    std::atomic_store_explicit(&flag_[0], false, std::memory_order_release);
    std::atomic_store_explicit(&flag_[1], false, std::memory_order_release);
  }

  void Lock(std::size_t id) {
    std::atomic_store_explicit(&flag_[id], true, std::memory_order_relaxed);
    std::atomic_exchange_explicit(&victim_, id, std::memory_order_acq_rel);
    while (std::atomic_load_explicit(&flag_[1 - id], std::memory_order_acquire) &&
           std::atomic_load_explicit(&victim_, std::memory_order_relaxed) == id) {}
  }

  void Unlock(const std::size_t& id) {
    std::atomic_store_explicit(&flag_[id], false, std::memory_order_release);
  }

 private:
  std::atomic<bool> flag_[2];
  std::atomic<std::size_t> victim_;
};

} // namespace utils

#endif // LOCK_COMPARE_PETERSON_LOCK_H_
