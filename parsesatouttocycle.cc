#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>

using namespace std;


vector<int> variables;
int nodenumber = 0;


main (int argc, char* argv[]) {
	string line; 
	string filename = argv[1];
	ifstream myfile (filename.c_str());
	
	if (myfile.is_open()) {
		
		getline (myfile, line); // should be "SAT"
		getline (myfile, line); // should be the solution of the SAT problem

		string word;
		istringstream iss(line);
    	iss >> word;
    	int variable = stoi(word);

    	while ( variable != 0 ) {
    		variables.push_back(variable - 1);
    		iss >> word;
    		variable = stoi(word);
    	}

    	nodenumber = round(sqrt(variables.size()));
    	int * nodes;
    	nodes = new int[nodenumber];

    	for (unsigned i = 0; i < variables.size(); i++) {
    		if ( variables[i] > 0 ) {
    			int position = variables[i] % nodenumber;
    			int node = variables[i] / nodenumber;
    			nodes[position] = node;
    		}
    	}

        cout << "v ";
    	for (unsigned j = 0; j < nodenumber - 1; j++) {
    		cout << nodes[j] + 1 << " ";
    	}
        cout << nodes[nodenumber - 1] + 1 << endl;

	}

	return 0;
}