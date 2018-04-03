#include <list>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include "date.h"

using namespace date;
using namespace std;

struct recordEntry {
	string ip;
	string start_date;
	string end_date;
};

int main(int argc, char *argv[])
{
	//check arguments
	if (argc != 4) {
		cout << "Insufficient arguments!" << endl;
		exit(EXIT_FAILURE);
	}

	//open input log file
	ifstream infile ("./input/log.csv");
	string line;

	//skip the first line
	getline (infile, line);
	//iterate over every following line
	while (getline (infile, line)) {
		// read a line from the file
		istringstream iss (line);
		string token;
		// split up the line using ',' as delimiter
		getline (iss, token, ',');
		cout << "ip: " << token << endl;
		string datetime = getline (iss, token, ',') + "," + getline (iss, token, ',');
		cout << "datetime: " << datetime << endl;
	}
}


