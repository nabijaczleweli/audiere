#include <time.h>
#include "timer.hpp"


adr_u64 GetNow() {
  return adr_u64(1000000) * clock() / CLOCKS_PER_SEC;
}
