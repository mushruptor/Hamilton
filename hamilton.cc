#include <iostream>
#include <string>

main (int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " FILENAME" << std::endl;
		return 1;
	}
	string line, filename;
	filename = argv[1];
	ifstream myfile (filename);
	if (myfile.is_open()) {
    	while ( getline (myfile,line) ) {
      		cout << line << '\n';
    	}
    	myfile.close();
    }

	else cout << "Unable to open file"; 

	return 0;
}