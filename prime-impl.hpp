#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <limits>
#include "clauses.hpp"

namespace prime_impl
{
  using std::set;
  using std::vector;
  using std::cout;
  using std::endl;

  typedef set<int> clauses;

  int add_cls( const clause &c );
  clause& getcls( int idx );
  bool unit_res();
  // In: clauses cs: set of set of int
  //     order: vars in some order
  //
  // Unit clauses allowed
  //
  // Out: prime implicates, no tautologies
  void get(vector<int>& order, int max_var, clauses& pi);
}
