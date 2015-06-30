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
bool undirected = false;     // handle the graph as undirected graph
bool firstStarting = false;  // set first node as starting point
bool check = false;          // check for and remove redundant edges
bool decode = false;         // decode SAT solution
bool verify = false;         // verify hamiltonian cycle solution
int verbosity = 0;           // verbosity of the tool
int boardEncoding = 1111;    // board constraints to be encoded
int distanceToEncode = 1;    // binary clauses to exclude neighborhood
bool cycle = true;           // find hamiltonian cycle
bool orderEncoding = false;   // use order encoding as representation

void usage(char* binary) {
  cerr << "USAGE: " << binary << " [OPTIONS] input" << endl;
  cerr << endl;
  cerr << "OPTIONS:" << endl
       << "-c      ... check graph for redundant edges and remove them" << endl
       << "-b ABCD ... how to encode number constraints for nodes: A=1: AMO(Node,Number), B=1: ALO(Node,Number), C=1: AMO(Number,Node), D=1: ALO(Number,Node)" << endl
       << "            at least A and D, or B and C, have to be set to 1 for a correct encoding" << endl
       << "-d      ... decode solution from SAT solution (SAT solution given on stdin)" << endl
       << "-f      ... set first node as starting point" << endl
       << "-l n    ... encode distance constarints up to distance n" << endl
       << "-o      ... use order encoding as representation" << endl
       << "-u      ... undirected graph" << endl
       << "-w      ... verify solution on input graph (solution given on stdin)" << endl
       << "-v      ... increase verbosity" << endl;
}

/// read solution from stdin, and return solution exit code (0,10,20)
int readValueVector( vector<int>& valueVector )
{
  istream* fileptr= &cin;
  istream& file = *fileptr;

  int solution = 0;
  string line;
  while( getline( file, line ) ) {
  // check satisfiability
    if( line[0] == 's' ) { 
      if( line.find( "UNSATISFIABLE" ) != string::npos ) {
			  solution = 20;
			  break;
      } else if( line.find( "UNKNOWN" ) != string::npos ) { 
			  solution = 0;
			  break;
      } else {
			  solution = 10;
      }
    }
    // check model
    if( line[0] == 'v' ) {
	int ind = 2;
	while( ind < line.size() && line[ind] == ' ' ) ++ind;
	while(line.size() > ind)
	{
		int number = 0;
		bool negative = false;
		while ( ind < line.size() && line[ind] == '-')
		{
			negative = true;
			ind++;
		}
		while( ind < line.size() && line[ind] >= '0' && line[ind] <= '9' )
		{
			number *=10;
			number += line[ind++] - '0';
		}
		if( number == 0 ) break;
		number = (negative) ? 0 - number : number;
		valueVector.push_back( number );
		while(line[ind] == ' ') ind++;
	}
    }
  }
  
  return solution;
}

/// print elements of a vector
template <typename T>
inline std::ostream& operator<<(std::ostream& other, const std::vector<T>& data ) 
{
  for( int i = 0 ; i < data.size(); ++ i )
    other << " " << data[i];
  return other;
}



int verifySolution( vector< vector<int> >& graph, const int nodes, const int edges ) {
  vector<int> solutionPath;
  int ret = readValueVector( solutionPath );
  
  if( ret == 0 ) {
    cout << "s UNKNOWN" << endl;
    return 0;
  } else if ( ret == 20 ) {
    cout << "s UNSATISFIABLE" << endl;
    return 20;
  } else {
    if( verbosity > 1 ) cerr << "c extracted path: " << solutionPath << endl;  
    
    int startNode = 0, prevNode = 0;
    vector<char> visitedAlready ( nodes + 1, 0 ); // check whether node has been visited
    
    for( int i = 0 ; i < solutionPath.size(); ++ i ) {
      if( solutionPath[i] > nodes || solutionPath[i] < 1 ) {
	cerr << "c found invalid node " << solutionPath[i] << endl;
	return 15;
      }
      
      if( visitedAlready[ solutionPath[i] ] != 0 ) {
	cerr << "c visited node " << solutionPath[i] << " already" << endl;
	return 15;
      } else {
	visitedAlready[ solutionPath[i] ] = 1;
      }
      
      if( i == 0 ) startNode = solutionPath[i];
      else {
	// check whether the current node is in the list of prevNode
	bool found = false;
	for( int j = 0 ; j < graph[ prevNode ].size(); ++ j ) {
	  if( graph[ prevNode ][j] == solutionPath[i] ) {
	    found = true; break;
	  }
	}
	if(!found) {
	  cerr << "c did not find edge from " << prevNode << " to " << solutionPath[i] << endl;
	  return 15;
	}
      }
      prevNode = solutionPath[i];
    }
    
    // check for cycle-condition:
    // check whether the current node is in the list of prevNode
#warning perform this check only, if hamiltonian cycle should be created
    if( solutionPath.size() > 0 && cycle) {
      bool found = false;
      for( int j = 0 ; j < graph[ prevNode ].size(); ++ j ) {
	if( graph[ prevNode ][j] == solutionPath[0] ) {
	  found = true; break;
	}
      }
      if(!found) {
	cerr << "c did not find edge from " << prevNode << " to " << solutionPath[0] << endl;
	return 15;
      }
    }
    
    // check whether all nodes have been visited
    for( int node = 1; node <= nodes; ++ node ) {
      if( visitedAlready[node] != 1 ) {
	cerr << "c did not visit node " << node << endl;
	return 15;
      }
    }
    
    return 10;
  }
}

/**
 *  parse graph in DIMACS graph format into given structures
 */
bool parseGraph( char* filename, vector< vector<int> >& graph, int& nodes, int& edges )
{
  fstream infile ( filename );
  if( !infile ) return false;
  
  if( verbosity > 0 && !undirected ) cerr  << "c assuming directed graph" << endl;
  
  vector<char> compact; // check whether graph is compact
  
  string line, edgeString;
  int realEdges = 0, realNodes = 0, usedNodes = 0,linecount = 0;
  while( getline( infile, line  ) ) // parse file line by line
  {
    linecount ++;
    if( line.find("c") == 0  ) continue; // skip comments and empty lines

    if( line.find("p") == 0 ) {
      stringstream tmp( line );
      string h1,h2;
      tmp >> h1 >> h2 >> nodes >> edges ;
      if( verbosity > 0 ) cerr << "c found nodes: " << nodes << " and edges: " << edges << endl;
      graph.resize( nodes + 1);
      continue; 
    }
    
    bool empty = true;
    for( int i = 0 ; i < line.size(); ++ i ) {
      if( line[i] != ' ' ){ empty= false; break;}
    }
    if( empty ) continue;

    int from = -1, to = -1;
    stringstream linestream( line );
    
    linestream >> edgeString >> from >> to;
    
    if( edgeString != "e" ) {
      cerr << "c expected 'e' at beginning of line " << linecount << ", found " << edgeString << " instead - ABORT" << endl;
      return 1;
    }
    if( from == -1 || to == -1 ) {
      cerr << "c WARNING invalid edge in line[" << linecount << "]: " << line << endl;
      continue; // invalid edge
    }
    
    realEdges ++;
    realNodes = realNodes >= from ? realNodes : from;
    realNodes = realNodes >= to ? realNodes : to;

    if( verbosity > 2 ) cerr << "found edge " << from << " -> " << to << " edge: " << edgeString << endl;
    
    if( compact.size() <= from ) compact.resize( from + 1);
    if( compact.size() <= to ) compact.resize( to + 1);
    if( compact[from] != 1 )
      { usedNodes ++; compact[from] = 1; }
    if( compact[to] != 1 )  
      { usedNodes ++; compact[to] = 1; }
    
    // add parsed edge to graph
    if ( graph.size() <= from ) graph.resize(from+1);
    graph[ from ] .push_back( to );
    if( undirected ) { 
      if ( graph.size() <= to ) graph.resize(to+1);
      graph[ to ] .push_back(from);
    }
  }
  infile.close();
  
  if( realNodes != nodes ) cerr << "c WARNING nodes information mismatch: header: " << nodes << " used: " << realNodes << endl;
  if( realEdges != edges ) cerr << "c WARNING edges information mismatch: header: " << edges << " used: " << realEdges << endl;
  nodes = nodes >= realNodes ? nodes : realNodes;
  edges = edges >= realEdges ? edges : realEdges;

  if( nodes != usedNodes ) cerr << "c WARNING graph is not compact, there are unused nodes (" << usedNodes << " used out of " << nodes << ")" << endl;
  
  return true;
}

/**
 *  main method
 */
int main(int argc, char *argv[])
{  
  if( argc < 2 ) { usage(argv[0]); return 1; }
  
  // parse parameters
  for( int i = 1; i + 1 < argc; ++ i ) {
    if( verbosity > 1 ) cerr << "c handle[" << i << "]: " << argv[i] << endl;
    if( string(argv[i]) == "-u" ) undirected = true;
    else if( string(argv[i]) == "-v" ) verbosity++;
    else if( string(argv[i]) == "-f" ) firstStarting=true;
    else if( string(argv[i]) == "-d" ) decode=true;
    else if( string(argv[i]) == "-w" ) verify=true;
    else if( string(argv[i]) == "-o" ) orderEncoding=true;
    else if( string(argv[i]) == "-b" && i + 1 < argc ) { // extract board encoding
      ++i;
      stringstream tmp( string( argv[i] ));
      tmp >> boardEncoding;
    }
    else if( string(argv[i]) == "-l" && i + 1 < argc ) { // encode distance relation
      ++i;
      stringstream tmp( string( argv[i] ));
      tmp >> distanceToEncode;
      if( distanceToEncode < 1 ) {
	cerr << "c set -l option from " << distanceToEncode << " to 1" << endl;
	distanceToEncode = 1;
      }
    }
    else {
      cerr << "option " << argv[i] << " not implemented yet ... " << endl;
    }
  }
  
  if( verbosity > 0 ) {
    cerr << "c file: " << argv[ argc - 1 ] << endl;
    cerr << "c undirected:    "  << (int) undirected << endl
         << "c decode:        "  << (int) decode  << endl
         << "c verify:        "  << (int) verify  << endl
	 << "c verbosity:     "  << verbosity << endl
	 << "c firstStarting: "  << (int) firstStarting << endl
	 << "c order encoding: " << (int) orderEncoding << endl
	 << "c cycle: "          << (int) cycle << endl
	 << "c boardEncoding: "  << boardEncoding << endl;
	 

  }
  
  int nodes = 0, edges = 0;
  vector< vector<int> > graph; // adjacence lists

  if( !parseGraph( argv[ argc-1], graph, nodes, edges ) ) {
    cerr << "c could not parse graph from file " << argv[ argc-1] << " - ABORT" << endl;
    return 1;
  }
  
  if( verify ) {
    cerr << "c verify given path with respect to parsed graph ... " << endl;
    int ret = verifySolution( graph, nodes, edges );
    
    if( ret == 10 ) {
      cout << "s VERIFIED" << endl;
    } else {
      if( ret == 15 ) {
	cout << "s NOT VERIFIED" << endl;
      } else {
	cout << "s UNKNOWN" << endl;
      }
    }
    return ret;
  } else if ( decode ) {
    cerr << "c this binary does not include solution decoding ... " << endl;
    return 1;
  } else {
    // encode hamiltonian cycle to stdout
    cerr << "c this binary does not include problem encoding ... " << endl;
    return 1;
  }
  
  return 0;
}
