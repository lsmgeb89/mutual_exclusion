#ifndef LOCK_COMPARE_TESTER_H_
#define LOCK_COMPARE_TESTER_H_

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <sstream>
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

  void SetThreadNum(const std::size_t& thread_num) { thread_num_ = thread_num; }

  std::chrono::duration<double, std::nano> Test(void) {
    auto begin = std::chrono::steady_clock::now();

    CreateThreads();

    for (auto& thread : thread_pool) {
      thread.join();
    }

    auto end = std::chrono::steady_clock::now();

    if (counter_ != thread_num_ * loop_num_) {
      throw std::runtime_error("Test failed");
    }

    std::cout << "Thread = " << thread_num_ << ", Loop = " << loop_num_
              << ", Count = " << counter_ << ", Total Time = "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()
              << " (ns)" << std::endl;

    Reset();

    return (end - begin);
  }

  void Reset(void) {
    counter_ = 0;
    thread_pool.clear();
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

class Tester {
 public:
  Tester(const std::size_t& thread_num,
         const std::size_t& loop_num,
         const std::size_t& test_times)
    : thread_num_(thread_num),
      loop_num_(loop_num),
      test_times_(test_times),
      tas_tester_(thread_num, loop_num),
      ttas_tester_(thread_num, loop_num),
      tour_tester_(thread_num, loop_num) {
    tas_result_.resize(thread_num);
    ttas_result_.resize(thread_num);
    tour_result_.resize(thread_num);
  }

  void Test(void) {
    for (std::size_t t_num = 1; t_num <= thread_num_; t_num++) {
      tas_tester_.SetThreadNum(t_num);
      ttas_tester_.SetThreadNum(t_num);
      tour_tester_.SetThreadNum(t_num);

      // For each thread number, we test multiple times
      for (std::size_t i = 0; i < test_times_; i++) {
        tas_result_.at(t_num - 1) += tas_tester_.Test();
        ttas_result_.at(t_num - 1) += ttas_tester_.Test();
        tour_result_.at(t_num - 1) += tour_tester_.Test();
      }

      // average
      tas_result_.at(t_num - 1) /= test_times_;
      ttas_result_.at(t_num - 1) /= test_times_;
      tour_result_.at(t_num - 1) /= test_times_;
    }
  }

  std::string ResultToString(void) {
    std::stringstream out_stream;
    for (auto item : tas_result_) {
      out_stream << item.count() << ", ";
    }
    out_stream << std::endl;
    for (auto item : ttas_result_) {
      out_stream << item.count() << ", ";
    }
    out_stream << std::endl;
    for (auto item : tour_result_) {
      out_stream << item.count() << ", ";
    }
    out_stream << std::endl;

    return out_stream.str();
  }

 private:
  static constexpr std::size_t level = 4;
  std::size_t thread_num_;
  std::size_t loop_num_;
  std::size_t test_times_;

  utils::TASTester<utils::TASLock> tas_tester_;
  utils::TASTester<utils::TTASLock> ttas_tester_;
  utils::TourTester<level> tour_tester_;

  std::vector<std::chrono::duration<double, std::nano>> tas_result_;
  std::vector<std::chrono::duration<double, std::nano>> ttas_result_;
  std::vector<std::chrono::duration<double, std::nano>> tour_result_;
};

} // namespace utils

#endif // LOCK_COMPARE_TESTER_H_
