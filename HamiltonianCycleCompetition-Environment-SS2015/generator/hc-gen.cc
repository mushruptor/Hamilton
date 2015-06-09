/* verifier for hamiltonian path in DIMACS graph format to CNF, December 2014
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>

#include <assert.h>

using namespace std;

// OPTIONS
int nodes = 0;
int edgesPerNode = 3;
bool forceSat = false;

void usage(char* binary) {
  cerr << "USAGE: " << binary << " [OPTIONS] input" << endl;
  cerr << endl;
  cerr << "OPTIONS:" << endl
       << "-n X    ... number of nodes for the graph" << endl
       << "-e X    ... number of edges for each node" << endl
       << "-s X    ... random seed to start with" << endl
       << "-f      ... ensure that there exists at least on HC" << endl;
}

/// print elements of a vector
template <typename T>
inline std::ostream& operator<<(std::ostream& other, const std::vector<T>& data ) 
{
  for( int i = 0 ; i < data.size(); ++ i )
    other << " " << data[i];
  return other;
}
/**
 *  main method
 */
int main(int argc, char *argv[])
{  
  if( argc < 2 ) { usage(argv[0]); return 1; }
  
  int seed = 0;
  
  // parse parameters
  for( int i = 1; i + 1 <= argc; ++ i ) {
 	  if( string(argv[i]) == "-n" && i + 1 < argc ) { // extract board encoding
      ++i;
      stringstream tmp( string( argv[i] ));
      tmp >> nodes;
    }
    else if( string(argv[i]) == "-e" && i + 1 < argc ) { 
      ++i;
      stringstream tmp( string( argv[i] ));
      tmp >> edgesPerNode;
    }
    else if( string(argv[i]) == "-s" && i + 1 < argc ) { 
      ++i;
      stringstream tmp( string( argv[i] ));
      tmp >> seed;
    } 
    else if( string(argv[i]) == "-f" ) { 
      forceSat = true;
    } else {
      cerr << "option " << argv[i] << " not implemented yet ... " << endl;
    }
  }
  
  srandom( seed );
  cerr << "c generate graph with " << nodes << " nodes and " << edgesPerNode << " edges per node. forced sat: " << forceSat << endl;
  

  vector< vector<int> > graph ( nodes ); // adjacence lists

	if( forceSat ) {
		vector< int > hc ( nodes -1);
		for( int i  = 0; i + 1< nodes; ++ i ) hc[i] = i+1; // fill hc
		int lastNode = 0, lastNodeI = 0;
		cerr << "c path start node: " << lastNode  << " (remain: " << hc << ")" <<  endl;
		for( int i  = 0; i < nodes; ++ i ) {
			int nextNodeI = hc.size() > 0  ?  rand() % hc.size()  :  0;
			int nextNode =  hc.size() > 0  ?  hc[ nextNodeI ]     :  0;
			
			graph[ lastNode ] . push_back ( nextNode );
			
			cerr << "c next path node: " << nextNode << " (remain: " << hc << ")" << endl;
			
			if( hc.size() > 0) {
					hc[ nextNodeI ] = hc[ hc.size() - 1 ]; hc.pop_back(); // delete element
			}
			lastNode = nextNode; lastNodeI = nextNodeI;
		}
		
		int sum = nodes; // we start counting from 0
		for( int i = 0 ; i < graph.size(); ++ i ) { assert( graph[i].size() == 1 ); sum+= graph[i][0]; } // each element has one edge
		assert( sum == ( nodes * (nodes + 1 ) ) / 2 ); // each node occurs exactly once
		
		edgesPerNode --; // have one node less per edge
	}
	
	for( int i = 0 ; i < nodes; ++ i ) {
		// generate nodes
		for( int j = 0 ; j < edgesPerNode; ++ j ) {
		  int nextNode = rand() % nodes;
		  if( nextNode == i ) { j --; continue; } // avoid self loops
			graph[i].push_back( nextNode ) ;
		}
		// sort adjacency list
		sort( graph[i].begin(), graph[i].end() );
	
	}
	
	if( forceSat ) edgesPerNode ++; // correct value again
	
	for( int i = 0 ; i < graph.size(); ++ i ) { assert( graph[i].size() == edgesPerNode ); } // each element has one edge
	
	cout << "p edge " << nodes << " " << edgesPerNode * nodes << endl;
	for( int i = 0 ; i < nodes; ++ i ) {
		// generate nodes
		for( int j = 0 ; j < graph[i].size(); ++ j ) {
			cout << "e " << i+1 << " " << graph[i][j]+1 << endl; // do not start with 0!
		}
	}
  return 0;
}
