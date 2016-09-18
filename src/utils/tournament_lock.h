#ifndef LOCK_COMPARE_TOURNAMENT_LOCK_H_
#define LOCK_COMPARE_TOURNAMENT_LOCK_H_

#include <array>
#include "peterson_lock.h"

namespace utils {

template <std::size_t LEVEL_NUM, std::size_t THREAD_CAPACITY> class TournamentLock {
 public:
  void Lock(const std::size_t& thread_id,
            std::size_t& node_id,
            std::array<std::size_t, LEVEL_NUM>& level_id) {
    node_id = thread_id + THREAD_CAPACITY - 1;
    for (std::size_t level = 0; level < LEVEL_NUM; level++) {
      level_id.at(level) = (node_id + 1) % 2;
      node_id = (node_id - 1) / 2;
      nodes_.at(node_id).Lock(level_id[level]);
    }
  }

  void Unlock(std::size_t& node_id, std::array<std::size_t, LEVEL_NUM>& level_id) {
    node_id = 0;
    for (std::ptrdiff_t level = LEVEL_NUM - 1; level >= 0; level--) {
      nodes_.at(node_id).Unlock(level_id[level]);
      node_id = 2 * node_id + 1 + level_id[level];
    }
  }

 private:
  std::array<PetersonLock, THREAD_CAPACITY - 1> nodes_;
};

} // namespace utils

#endif // LOCK_COMPARE_TOURNAMENT_LOCK_H_
