#ifndef LOCK_COMPARE_TESTER_H_
#define LOCK_COMPARE_TESTER_H_

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

namespace utils {

class BaseTester {
 public:
  BaseTester(const std::size_t& thread_num,
             const std::size_t& loop_num)
    : counter_(0),
      thread_num_(thread_num),
      loop_num_(loop_num) {}

  void Test(void) {
    auto begin = std::chrono::steady_clock::now();

    CreateThreads();

    for (auto& thread : thread_pool) {
      thread.join();
    }

    auto end = std::chrono::steady_clock::now();

    std::cout << "Thread = " << thread_num_ << ", Loop = " << loop_num_
              << ", Count = " << counter_ << ", Total Time = "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()
              << " (ns)" << std::endl;
  }

 private:
  virtual void CreateThreads(void) = 0;

 protected:
  uint64_t counter_;
  std::size_t thread_num_;
  std::size_t loop_num_;
  std::vector<std::thread> thread_pool;
};

template <typename LockType> class TASTester : public BaseTester {
 public:
  TASTester(const std::size_t& thread_num,
            const std::size_t& loop_num)
    : BaseTester(thread_num, loop_num) {}

 private:
  void CreateThreads(void) override {
    for (std::size_t id = 0; id < this->thread_num_; ++id) {
      this->thread_pool.emplace_back([this] { this->Add(); });
    }
  }

  void Add(void) {
    for (uint64_t i = 0; i < this->loop_num_; ++i) {
      lock_.Lock();
      ++this->counter_;
      lock_.Unlock();
    }
  }

 private:
  LockType lock_;
};

class PetersonTester : public BaseTester {
 public:
  PetersonTester(const std::size_t& thread_num,
                 const std::size_t& loop_num)
    : BaseTester(thread_num, loop_num) {}

 private:
  void CreateThreads(void) override {
    for (std::size_t id = 0; id < this->thread_num_; ++id) {
      this->thread_pool.emplace_back([this, id] { this->Add(id); });
    }
  }

  void Add(const std::size_t& id) {
    for (uint64_t i = 0; i < this->loop_num_; ++i) {
      lock_.Lock(id);
      ++this->counter_;
      lock_.Unlock(id);
    }
  }

 private:
  utils::PetersonLock lock_;
};

template <std::size_t LEVEL_NUM> class TourTester : public BaseTester {
 public:
  TourTester(const std::size_t& thread_num,
             const std::size_t& loop_num)
    : BaseTester(thread_num, loop_num) {}

 private:
  void CreateThreads(void) override {
    for (std::size_t id = 0; id < this->thread_num_; ++id) {
      this->thread_pool.emplace_back([this, id] { this->Add(id); });
    }
  }

  void Add(const std::size_t& id) {
    std::size_t node_id;
    std::array<std::size_t, LEVEL_NUM> level_id;

    for (uint64_t i = 0; i < this->loop_num_; ++i) {
      lock_.Lock(id, node_id, level_id);
      ++this->counter_;
      lock_.Unlock(node_id, level_id);
    }
  }

 private:
  utils::TournamentLock<LEVEL_NUM, 1 << LEVEL_NUM> lock_;
};

} // namespace utils

#endif // LOCK_COMPARE_TESTER_H_
