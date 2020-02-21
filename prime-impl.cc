#include <assert.h>
#include "prime-impl.hpp"

// This follows Tyson algorithm 
// improved by using Kernel resolution (Del Val)
// that in this case reduces to bucket elimination
// See Pierre Marquis article at the Handbook of Defeasible Reasoning and Uncertainty Management Systems

// If the input is subsumption free, the input will be so

namespace prime_impl
{
  int saved = 0;
  const bool debug_unit = false;
  const bool debug_it = false;

  vector<clause> clause_rep;
  vector<bool> clause_rep_used;
  int num_clauses = 0;
  
  clause& getcls( int idx )
  {
    return clause_rep[idx];
  }
  int add_cls( const clause &c )
  {
    clause_rep.push_back(c);
    clause_rep_used.push_back(true);
    if( debug_it )
      {
	cout << "Agregando clausula (num "<< (num_clauses) << ") nueva: ";
	pr_clause(c);
      }
    assert( clause_rep.size() == clause_rep_used.size() );
    return num_clauses++;
  }

  void disable_cls( int idx )
  {
    clause_rep_used[idx] = false;
  }
  bool is_enabled_cls( int idx )
  {
    return clause_rep_used[idx];
  }

  // In: clauses
  // In & Out: unit clauses
  // Out: res: clauses after unit resolution
  bool
  unit_res()
  {
    set<int> unit;

    for( int ci = 0; ci < num_clauses; ci++ )
      if( is_enabled_cls(ci) )
	{
	  clause& c = getcls(ci);
	  if(c.size() == 1)
	    unit.insert(*c.begin());
	}

    bool new_unit; 
    do
      {
	new_unit = false;
	for( int ci = 0; ci < num_clauses; ci++ )
	  if( is_enabled_cls(ci) )
	    {
	      clause& c = getcls(ci);
	      if( c.size() == 1 and unit.count( *c.begin() ) > 0 )
		continue;
	      for(set<int>::iterator u = unit.begin();
		  u != unit.end(); ++u )
		{
		  if( c.count( *u ) > 0 && c.size() > 1 )
		    {
		      disable_cls( ci );
		      break;
		    }
		  if( c.count( -*u ) > 0 )
		    {
		      c.erase( -*u );
		      if( c.empty() ) // empty clause
			return false;
		    }
		}
	  
	      if( c.size() == 1 )
		{
		  unit.insert(*c.begin());
		  new_unit = true;
		}
	    }
      }
    while(new_unit);
  
    if(debug_unit)
      {
	std::cout << "Unit: ";
	pr_clause(unit);
	std::cout << "Clauses (with Unit)" << std::endl;
	for( int ci = 0; ci < num_clauses; ci++ )
	  if( is_enabled_cls(ci) )
	    {
	      clause& c = getcls(ci);
	      pr_clause(c);
	    }
	std::cout << "end of Clauses" << std::endl << std::flush;
      }
    for(set<int>::iterator u = unit.begin();
	u != unit.end(); ++u )
      if( unit.count( -*u ) > 0 )
	return false;
    return true;
  }

  void print_all_clauses()
  {
    cout << "========= ALL CLAUSES" << endl;
    for( int ci = 0; ci < num_clauses; ci++ )
      {
	cout << "Clause #" << ci;
	if( is_enabled_cls(ci) )
	  cout << " (enabled)";
	else
	  cout << " (disabled)";
	cout << " : ";
	    clause& c= getcls(ci);
	    pr_clause(c);
      }
    cout << "========= END OF ALL CLAUSES" << endl << endl;
  }
  
  // In: clauses cs: set of set of int
  //     order: function: var -> number, as vector of pair. Each number is unique
  //
  // Assume: no unit clauses
  //
  // Out: prime implicates, no tautologies
  void
  get(vector<int>& order, int max_var, clauses& pi)
  {
    int bucket_size = max_var + 1;
    
    // Put clauses in buckets
    vector<clauses> buckets;
    buckets.resize(bucket_size);
    for( int ci = 0; ci < num_clauses; ci++ )
      if( is_enabled_cls(ci) )
	{
	  clause& c = getcls(ci);
	  for( clause::iterator l = c.begin(); l !=  c.end(); l++ )
	    buckets[abs(*l)].insert(ci);
	}
    if(debug_it)
      {
	cout << "Bucket:" << endl;
	for( int i = 1; i < bucket_size; i++ )
	  cout << "position " << i << " has "
	       << buckets[i].size() << " elements" << endl << std::flush;
      }
    
    vector<clause> tmp_clause_rep;
    // process buckets
    for( int var = 1; var < bucket_size; var++ )
      {
	clauses& c = buckets[var];
	if(debug_it)
	  cout << "Processing bucket " << var 
	       << " with size = " << c.size() << endl << std::flush;
	if(c.size() <= 1) continue;

	// For every pair of clauses
	for( clauses::const_iterator cidx1 = c.begin();
	     cidx1 != c.end(); ++cidx1 )
	  {
	    clause& c1 = getcls(*cidx1);
	    clauses::const_iterator cidx2 = cidx1;
	    ++cidx2;
	    for( ;
		 cidx2 != c.end(); ++cidx2 )
	      {
		clause& c2 = getcls(*cidx2);

		if( debug_it )
		  {
		    cout << "With var "<< var << " to process clauses # " << *cidx1 << " and " << *cidx2 << endl; 
		    print_all_clauses();
		  }

		// Resolve c1 and c2 with var
		assert( c1.count(var) + c1.count(-var) > 0 );
		assert( c2.count(var) + c2.count(-var) > 0 );
	    
		if( c1.count(var) > 0 && c2.count(var) > 0 )
		  continue;
		if( c1.count(-var) > 0 &&  c2.count(-var) > 0 )
		  continue;
		if(debug_it)
		  {
		    cout << "c1 has " << c1.size() << " c2 has " << c2.size() << endl;
		    cout << "c1 "; pr_clause( c1 );
		    cout << "c2 "; pr_clause( c2 );
		  }
		clause c;
		set_union( c1.begin(), c1.end(),
			   c2.begin(), c2.end(),
			   inserter(c, c.begin() ) );
		c.erase( var );
		c.erase( -var );
		if(debug_it)
		  cout << " c has " << c.size() << endl << std::flush;

		bool tauto = false;
		// Check that c is not a tautology
		for(clause::const_iterator v = c.begin();
		    v != c.end(); ++v )
		  if( c.count( -*v ) > 0 )
		    {
		      // is tautology
		      tauto = true;
		      break;
		    }
	    
		if(!tauto)		
		  // saving clause for later processing
		  tmp_clause_rep.push_back(c);;
	      }

	    // Process new clauses
	    for( vector<clause>::iterator ci = tmp_clause_rep.begin();
		 ci != tmp_clause_rep.end(); ci++ )
	      {
		clause& c = *ci;

		clauses subssum;
		bool subssumed = false;
		// Check subsumption of new clauses
		for( int ci = 0; ci < num_clauses; ci++ )
		  if( is_enabled_cls(ci) )
		    {
		      clause& c3 = getcls(ci);
		      if( includes( c.begin(), c.end(), c3.begin(), c3.end() ) )
			{
			  subssumed = true;
			  break;
			}
		      else if( includes( c3.begin(), c3.end(), c.begin(), c.end() ) )
			subssum.insert(ci);
		    }
		if( !subssumed )
		  {
		    // Disable subssumed
		    for(clauses::iterator del = subssum.begin();
			del != subssum.end(); ++del )
		      disable_cls( *del );

		    if(debug_it)
		      cout << "init put bucket. size = " << c.size() << endl << std::flush;
		    // Add new clause
		    int cidx_new = add_cls( c );
		    
		    // Add to all future proper bucket
		    for( clause::iterator l = c.begin(); l != c.end(); ++l )
		      {
			int var2 = abs(*l);
			if( var2  > var ) // update if in one the following bucket
			  {
			    buckets[var2].insert( cidx_new );
			    if(debug_it)
			      {
				cout << "Adding to bucket " << var2 << " the clause: ";
				pr_clause(c);
				cout << std::flush;
			      }
			  }
			// otherwise, this bucket already been processed
		      }
		  }
	      }
	    tmp_clause_rep.clear();
	  }
      }
    // PI are active clauses
    pi.clear();
    for( int ci = 0; ci < num_clauses; ci++ )
      if( is_enabled_cls(ci) )
	pi.insert(ci);
  }

}
