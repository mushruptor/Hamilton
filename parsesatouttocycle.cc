#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>

using namespace std;


vector<int> variables;
int nodenumber = 0;


main (int argc, char* argv[]) 
{
	string line; 
	string filename = argv[1];
	ifstream myfile (filename.c_str());
	
	if (myfile.is_open()) 
    {	
		getline (myfile, line); // is "SAT" or "UNSAT"

        if (line == "UNSAT")
        {
            cout << "s UNSATISFIABLE" << endl;
            return 20;
        }
        if (line != "SAT")
        {
            // unknown output
            return 33;
        }

		getline (myfile, line); // should be the solution of the SAT problem

		string word;
		istringstream iss(line);
    	iss >> word;
    	long variable = stoi(word);

    	while ( variable != 0 ) 
        {
            if (variable > 0) 
            {
                variables.push_back(variable - 1);
            }
    		
    		iss >> word;
    		variable = stoi(word);
    	}

    	nodenumber = variables.size();
        long * nodes;
    	nodes = new long[nodenumber];

    	for (unsigned long i = 0; i < variables.size(); ++i) 
        {
            long position = variables[i] % nodenumber;
            long node = variables[i] / nodenumber;
  			nodes[position] = node;
    	}

        cout << "s SATISFIABLE" << endl;
        cout << "v ";
    	for (unsigned long j = 0; j < nodenumber - 1; j++) 
        {
    		cout << nodes[j] + 1 << " ";
    	}
        cout << nodes[nodenumber - 1] + 1 << endl;
	}

	return 10;
}