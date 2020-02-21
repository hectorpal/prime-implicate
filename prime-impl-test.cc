#include "clauses.hpp"
#include "prime-impl.hpp"

using namespace std;
//using namespace prime_impl;


int
main( int argc, char **argv )
{
  cout << "Introduce theory: each line is a clause" << endl;
  cout << "End with 0. Finish by a line only with 0" << endl;

  int max_var = -1;
  while( true )
    {
      clause c;
      int v = 0;
      cin >> v;
      while( v != 0)
	{
	  int absv = abs(v);
	  max_var = max(max_var, absv);
	  c.insert(v);
	  cin >> v;
	}
      if( c.empty() )
	break;
      else
	{
	  prime_impl::add_cls(c);
	}
    }

  cout << "Loaded theory " << endl;
  
/*

Order: map from var to int
to follow, iterate over map.
To compare, compare ints.

Param for var for starting to keep.
Before: forget those vars,
after: keep.

*/

  if( !prime_impl::unit_res() )
    {
      cout << "Inconsistent theory" << endl;  
      exit(1);
    }

  // any order. Every number should be different.
  vector<int> order;
  for( int i = 1; i<= max_var; i++ )
    order.push_back(i);
  
  // Calc prime implicates
  prime_impl::clauses pi;
  prime_impl::get(order, max_var, pi);

  if( !prime_impl::unit_res() )
    {
      cout << "Inconsistent theory" << endl;  
      exit(1);
    }
  
  cout << "\n\nPrime implicates:" << endl;
  for(prime_impl::clauses::iterator ci = pi.begin(); ci != pi.end(); ++ci )
    {
      clause& cls = prime_impl::getcls(*ci);
      pr_clause(cls);
    }
  cout << "end of prime implicates:" << endl;
  
}

