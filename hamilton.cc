#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;


int vertexnumber = 0;
int cnfvarcount = 0;
int cnfcount = 0;
int currentedge = 1;


void writevariables1() {
	cnfvarcount = vertexnumber * vertexnumber;

	for (int vertex = 0; vertex < vertexnumber; vertex++) {
		// every vertex has to be in the path
		for (int position = 0; position < vertexnumber; position++) {
			int var = vertex * vertexnumber + position + 1;
			cout << var << " ";
		}
		cout << "0" << endl;

		// no vertex can appear twice in the path
		for (int position1 = 0; position1 < vertexnumber; position1++) {
			for (int position2 = position1 + 1; position2 < vertexnumber; position2++) {
				int var1 = vertex * vertexnumber + position1 + 1;
				int var2 = vertex * vertexnumber + position2 + 1;
				cout << -var1 << " " << -var2 << " 0" << endl;
			}
		}
	}

	for (int position = 0; position < vertexnumber; position++) {
		// every position has to be occupied by at least one vertex
		for (int vertex = 0; vertex < vertexnumber; vertex++) {
			int var = vertex * vertexnumber + position + 1;
			cout << var << " ";
		}
		cout << "0" << endl;

		// no position can be occupied by two vertices
		for (int vertex1 = 0; vertex1 < vertexnumber; vertex1++) {
			for (int vertex2 = vertex1 + 1; vertex2 < vertexnumber; vertex2++) {
				int var1 = vertex1 * vertexnumber + position + 1;
				int var2 = vertex2 * vertexnumber + position + 1;
				cout << -var1 << " " << -var2 << " 0" << endl;
			}
		}
	}

	return;
}


void writevariables2(int edge) {
	edge = edge - 1;
	int vertex1 = edge / vertexnumber;
	int vertex2 = edge % vertexnumber;
	if (vertex1 == vertex2) return;
	// edge between vertex1 and vertex2 does not exist
	for (int position = 0; position < vertexnumber; position++) {
		int var1 = vertex1 * vertexnumber + position + 1;
		int var2 = vertex2 * vertexnumber + ((position + 1) % vertexnumber) + 1;
		cout << -var1 << " " << -var2 << " 0" << endl;
	}
}


void handleline(string line) {
	char c = line[0];
	string word;
	istringstream iss(line);
	switch (c) {
		case 'c':
			break;
		case 'p':
			iss >> word; // should be "p"
			iss >> word; // should be "edge"
			iss >> word; // should be the number of vertices
			vertexnumber = stoi(word);
			writevariables1();
			break;
		case 'e':
			iss >> word; // should be "e"
			iss >> word; // should be the first vertex of the processed edge
			int vertex1 = stoi(word) - 1;
			iss >> word; // should be the second vertex of the processed edge
			int vertex2 = stoi(word) - 1;

			int edge = vertex1 * vertexnumber + vertex2 + 1;

			while (currentedge < cnfvarcount && edge != currentedge) {
				writevariables2(currentedge);
				currentedge++;
			}

			break;
	}
	return;
}


main (int argc, char* argv[]) {
	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " FILENAME" << endl;
		return 1;
	}

	string line; 
	string filename = argv[1];
	ifstream myfile (filename.c_str());
	if (myfile.is_open()) {
    	while ( getline (myfile,line) ) {
      		
    		handleline(line);

    	}
    	myfile.close();
    	
    	while (currentedge < cnfvarcount) {
				writevariables2(currentedge);
				currentedge++;
		}

    } else cout << "Unable to open file" << endl; 

	return 0;
}