#ifndef LOCK_COMPARE_PETERSON_LOCK_H_
#define LOCK_COMPARE_PETERSON_LOCK_H_

#include <atomic>

namespace utils {

class PetersonLock {
 public:
  PetersonLock(void) {
    std::atomic_store(&victim_, static_cast<std::size_t>(0));
    std::atomic_store(&flag_[0], false);
    std::atomic_store(&flag_[1], false);
  }

  void Lock(std::size_t id) {
    std::atomic_store(&flag_[id], true);
    std::atomic_exchange(&victim_, id);
    while (std::atomic_load(&flag_[1 - id]) &&
           std::atomic_load(&victim_) == id) {}
  }

  void Unlock(const std::size_t& id) {
    std::atomic_store(&flag_[id], false);
  }

 public:
  static constexpr auto name_ = "PetersonLock";

 private:
  std::atomic<bool> flag_[2];
  std::atomic<std::size_t> victim_;
};

} // namespace utils

#endif // LOCK_COMPARE_PETERSON_LOCK_H_
