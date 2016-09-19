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
  BaseTester(const std::string& lock_name,
             const std::size_t& thread_num,
             const std::size_t& loop_num)
    : lock_name_(lock_name),
      counter_(0),
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

#ifndef NDEBUG
    std::clog << "[" << lock_name_ << "] Thread = " << thread_num_ << ", Loop = " << loop_num_
              << ", Count = " << counter_ << ", Total Time = "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()
              << " (ns)" << std::endl;
#endif

    Reset();

    return (end - begin);
  }

  void Reset(void) {
    counter_ = 0;
    thread_pool.clear();
  }

 private:
  virtual void CreateThreads(void) = 0;

 public:
  std::string lock_name_;
 protected:
  uint64_t counter_;
  std::size_t thread_num_;
  std::size_t loop_num_;
  std::vector<std::thread> thread_pool;
};

template <typename LockType> class TASTester : public BaseTester {
 public:
  TASTester(const std::string& lock_name,
            const std::size_t& thread_num,
            const std::size_t& loop_num)
    : BaseTester(lock_name, thread_num, loop_num) {}

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

#ifndef NDEBUG
class PetersonTester : public BaseTester {
 public:
  PetersonTester(const std::size_t& thread_num,
                 const std::size_t& loop_num)
    : BaseTester(utils::PetersonLock::name_, thread_num, loop_num) {}

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
#endif

template <std::size_t LEVEL_NUM> class TourTester : public BaseTester {
 public:
  TourTester(const std::size_t& thread_num,
             const std::size_t& loop_num)
    : BaseTester(utils::TournamentLock<LEVEL_NUM, 1 << LEVEL_NUM>::name_, thread_num, loop_num) {}

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
      tas_tester_(utils::TASLock::name_, thread_num, loop_num),
      ttas_tester_(utils::TTASLock::name_, thread_num, loop_num),
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

    // parameter
    out_stream << "Thread Number: 1 ~ " << thread_num_
               << ", Loop Number: " << loop_num_
               << ", test times: " << test_times_
               << ", Tournament Tree Height: " << level_
               << ", Time Unit: Nanosecond"
               << std::endl;

    // header
    out_stream << "Thread Number, "
               << tas_tester_.lock_name_ << ", "
               << ttas_tester_.lock_name_ << ", "
               << tour_tester_.lock_name_ << std::endl;

    // line
    for (std::size_t i = 0; i < thread_num_; i++) {
      out_stream << i + 1 << ", "
                 << tas_result_.at(i).count() << ", "
                 << ttas_result_.at(i).count() << ", "
                 << tour_result_.at(i).count() << std::endl;
    }

    return out_stream.str();
  }

 private:
  static constexpr std::size_t level_ = 4;
  std::size_t thread_num_;
  std::size_t loop_num_;
  std::size_t test_times_;

  utils::TASTester<utils::TASLock> tas_tester_;
  utils::TASTester<utils::TTASLock> ttas_tester_;
  utils::TourTester<level_> tour_tester_;

  std::vector<std::chrono::duration<double, std::nano>> tas_result_;
  std::vector<std::chrono::duration<double, std::nano>> ttas_result_;
  std::vector<std::chrono::duration<double, std::nano>> tour_result_;
};

} // namespace utils

#endif // LOCK_COMPARE_TESTER_H_
