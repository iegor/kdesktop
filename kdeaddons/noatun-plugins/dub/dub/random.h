//
//
// C++ Interface for module: Random
//
// Description: 
//
//
// Author: exa
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef Random_Interface
#define Random_Interface

#include <cstdlib>
#include <time.h>

// wrapper for random functions
class Random {
public:
  int operator() (int N) {
    return random_int (N);
  }

  static void init() {
    seed += time(0);
    srandom(seed);
  }
  static double random_double (double upper_bound) {
    return double(random()) * upper_bound / RAND_MAX;
  }
  
  static int random_int (int upper_bound) {
    return random() % upper_bound;
  }
private:
  static int seed;
};

#endif

