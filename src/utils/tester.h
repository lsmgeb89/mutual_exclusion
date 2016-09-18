#ifndef LOCK_COMPARE_TESTER_H_
#define LOCK_COMPARE_TESTER_H_

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

namespace utils {

template <std::size_t THREAD_NUM> class BaseTester {
 public:
  BaseTester(const std::size_t loop_count)
    : count_(0),
      loop_count_(loop_count) {}

  void Test(void) {
    CreateThreads();

    for (auto& thread : thread_pool) {
      thread.join();
    }

    std::cout << "count = " << count_ << std::endl;
  }

 private:
  virtual void CreateThreads(void) = 0;

 protected:
  uint64_t count_;
  std::size_t loop_count_;
  std::vector<std::thread> thread_pool;
};

template <typename LockType, std::size_t THREAD_NUM> class TASTester : public BaseTester<THREAD_NUM> {
 public:
  TASTester(const std::size_t loop_count)
    : BaseTester<THREAD_NUM>(loop_count) {}

 private:
  void CreateThreads(void) override {
    for (std::size_t id = 0; id < THREAD_NUM; ++id) {
      this->thread_pool.emplace_back([this] { this->Add(); });
    }
  }

  void Add(void) {
    for (uint64_t i = 0; i < this->loop_count_; ++i) {
      lock_.Lock();
      ++this->count_;
      lock_.Unlock();
    }
  }

 private:
  LockType lock_;
};

template <std::size_t LEVEL_NUM, std::size_t THREAD_NUM> class Tester : public BaseTester<THREAD_NUM>{
 public:
  Tester(const std::size_t loop_count)
    : BaseTester<THREAD_NUM>(loop_count) {}

 private:
  void CreateThreads(void) override {
    for (std::size_t id = 0; id < THREAD_NUM; ++id) {
      this->thread_pool.emplace_back([this, id] { this->Add(id); });
    }
  }

  void Add(const std::size_t& id) {
    std::size_t node_id;
    std::array<std::size_t, LEVEL_NUM> level_id;

    for (uint64_t i = 0; i < this->loop_count_; ++i) {
      lock_.Lock(id, node_id, level_id);
      ++this->count_;
      lock_.Unlock(node_id, level_id);
    }
  }

 private:
  utils::TournamentLock<LEVEL_NUM, THREAD_NUM> lock_;
};

} // namespace utils

#endif // LOCK_COMPARE_TESTER_H_
