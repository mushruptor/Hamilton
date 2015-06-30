#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <math.h>
using namespace std;


/*
 * Internally I work with zero-based identifiers for nodes, positions and cnf variables.
 * But since DIMACS works with one-based indentifiers, every input/output has to be decremented/incremented.
 */


struct edge {
	int n1;
	int n2;
};


bool equalEdge(edge e1, edge e2) {
	return e1.n1 == e2.n1 && e1.n2 == e2.n2;
}


int nodenumber = 0;
int cnfcount = 0;
edge currentedge;


void writevariables1() {

	for (int x = 0; x < nodenumber; x++) {

		// every node x has to be in the path on some position y
		for (int y = 0; y < nodenumber; y++) {
			int var = x * nodenumber + y + 1;
			cout << var << " ";
		}
		cout << "0" << endl;

		// every position x has to be occupied by at least one node y
		for (int y = 0; y < nodenumber; y++) {
			int var = y * nodenumber + x + 1;
			cout << var << " ";
		}
		cout << "0" << endl;

		for (int y1 = 0; y1 < nodenumber; y1++) {
			for (int y2 = y1 + 1; y2 < nodenumber; y2++) {
				
				// no node x can appear twice in the path (on two positions y1 and y2)
				int var1 = x * nodenumber + y1 + 1;
				int var2 = x * nodenumber + y2 + 1;
				cout << -var1 << " " << -var2 << " 0" << endl;

				// no position x can be occupied by two nodes y1 and y2
				var1 = y1 * nodenumber + x + 1;
				var2 = y2 * nodenumber + x + 1;
				cout << -var1 << " " << -var2 << " 0" << endl;
			}
		}
	}

	return;
}


void writevariables2(edge e) {
	int node1 = e.n1;
	int node2 = e.n2;

	for (int position = 0; position < nodenumber; position++) {
		int var1 = node1 * nodenumber + position + 1;
		int var2 = node2 * nodenumber + ((position + 1) % nodenumber) + 1;
		cout << -var1 << " " << -var2 << " 0" << endl;
	}
}


bool incCurrentedge() {
	currentedge.n2 = (currentedge.n2 + 1) % nodenumber;
	if ( currentedge.n2 == 0 ) {
		currentedge.n1 = (currentedge.n1 + 1) % nodenumber;
		if ( currentedge.n1 == 0 ) {
			return false;
		}
	}

	if ( currentedge.n1 == currentedge.n2 ) {
		return incCurrentedge();
	}
	return true;
}


main (int argc, char* argv[]) {
	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " FILENAME" << endl;
		return 1;
	}

	// Start with the edge from node 0 to node 0. This will not be used as it is incremented before its first use.
	currentedge.n1 = 0;
	currentedge.n2 = 0;

	string line; 
	string filename = argv[1];
	ifstream myfile (filename.c_str());
	if (myfile.is_open()) {
    	
    	bool processingEdges = false;
    	while ( !processingEdges && getline (myfile,line) ) {
      		
   			char c = line[0];

    		switch (c) {
    			case 'c': 
    				continue;
    			case 'p':
    				string word;
					istringstream iss(line);
    				iss >> word; // should be "p"
					iss >> word; // should be "edge"
					iss >> word; // should be the number of nodes
					nodenumber = stoi(word);
					iss >> word; // should be the number of the edges; might be wrong, but minisat can handle a wrong number of conditions as well
					int edgenumber = stoi(word);
					int conditionsnumber = 2 * pow(nodenumber, 3) - 2 * pow(nodenumber, 2) + nodenumber * ( 2 - edgenumber );

					cout << "p cnf " << pow(nodenumber, 2) << " " << conditionsnumber << endl;
					writevariables1();
					
					processingEdges = true;
					break;
    		}

    	}


    	edge newedge;

    	while ( getline (myfile, line) ) {
    		char c = line[0];

    		if ( c == 'c' ) {
    			continue;
    		} else if ( c == 'e' ) {
    			string word;
				istringstream iss(line);
    			iss >> word; // should be "e"
				iss >> word; // should be the first node of the processed edge
				int node1 = stoi(word) - 1;
				iss >> word; // should be the second node of the processed edge
				int node2 = stoi(word) - 1;

				if ( newedge.n1 == node1 && newedge.n2 == node2 ) {
					continue;
				}

				newedge.n1 = node1;
				newedge.n2 = node2;

				while ( incCurrentedge() && !equalEdge(newedge, currentedge) ) {
					writevariables2(currentedge);
				}
    		}

    	}


    	myfile.close();
    	
    	while (incCurrentedge()) {
				writevariables2(currentedge);
		}

    } else cout << "Unable to open file" << endl; 

	return 0;
}