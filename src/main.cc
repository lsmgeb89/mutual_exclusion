#include <cstdlib>
#include "tas_lock.h"
#include "ttas_lock.h"
#include "tournament_lock.h"
#include "tester.h"

int main(int argc, char* argv[]) {
  const std::size_t level(4);
  std::size_t thread_num(0);
  std::size_t loop_num(0);

  if (argc != 3) {
    std::cerr << "Wrong Parameter!" << std::endl;
    return EXIT_FAILURE;
  } else {
    thread_num = static_cast<std::size_t>(std::stoul(argv[1]));
    loop_num = static_cast<std::size_t>(std::stoul(argv[2]));
    if (thread_num > (1 << level)) {
      std::cerr << "Too many threads!" << std::endl;
      return EXIT_FAILURE;
    }
  }

  {
    utils::TASTester<utils::TASLock> a(thread_num, loop_num);
    a.Test();
  }

  {
    utils::TASTester<utils::TTASLock> a(thread_num, loop_num);
    a.Test();
  }

  {
    utils::TourTester<level> a(thread_num, loop_num);
    a.Test();
  }

  return EXIT_SUCCESS;
}
