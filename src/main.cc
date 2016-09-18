#include "tas_lock.h"
#include "ttas_lock.h"
#include "tournament_lock.h"
#include "tester.h"

int main(int argc, char* argv[]) {
  {
    utils::TASTester<utils::TASLock, 10> a(1000000);
    a.Test();
  }

  {
    utils::TASTester<utils::TTASLock, 10> a(1000000);
    a.Test();
  }

  {
    utils::Tester<3, 8> a(1000000);
    a.Test();
  }

  return 0;
}
