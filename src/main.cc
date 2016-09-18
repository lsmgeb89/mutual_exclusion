#include <cstdlib>
#include "tas_lock.h"
#include "ttas_lock.h"
#include "tournament_lock.h"
#include "tester.h"

int main(int argc, char* argv[]) {
  std::size_t thread_num(0);
  std::size_t loop_num(0);
  std::size_t test_times(0);

  if (argc != 4) {
    std::cerr << "Wrong Parameter!" << std::endl;
    return EXIT_FAILURE;
  } else {
    thread_num = static_cast<std::size_t>(std::stoul(argv[1]));
    loop_num = static_cast<std::size_t>(std::stoul(argv[2]));
    test_times = static_cast<std::size_t>(std::stoul(argv[3]));
  }

  try {
    utils::Tester t(thread_num, loop_num, test_times);
    t.Test();
    std::cout << t.ResultToString();
  } catch (const std::runtime_error& err) {
    std::cerr << "Mutual Exclusion failed!" << std::endl;
  } catch (...) {
    std::cerr << "Internal Error!" << std::endl;
  }

  return EXIT_SUCCESS;
}
