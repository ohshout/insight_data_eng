#include <list>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include "date.h"

//TODO instead of update dict and L for every line
//we might be able to aggregate multiple lines if they're of the same user
//TODO use array and enum to handle getline

using namespace date;
using namespace std;

struct userRecord {
	sys_seconds start;
	sys_seconds end;
	int count;
	list<pair<string, sys_seconds>>::iterator it;
	userRecord (sys_seconds t, list<pair<string, sys_seconds>>::iterator i) :
		start(t), end(t), count(1), it (i) {}
};

int main(int argc, char *argv[])
{
	// check arguments
	if (argc != 4) {
		cout << "Insufficient arguments!" << endl;
		exit(EXIT_FAILURE);
	}

	unordered_map<string, userRecord> dict;
	list<pair<string, sys_seconds>> L; //<ip, time>

	//read inactivity period value
	ifstream period_file (argv[2]);
	string period_s;
	getline(period_file, period_s);
	int period = atoi(period_s);
	cout << "period: " << period << endl;

	// open input log file
	ifstream infile (argv[1]);
	string line;

	// skip the first line
	getline (infile, line);
	// iterate over every following line
	while (getline (infile, line)) {
		// read a line from the file
		istringstream iss (line);
		string token;
		// split up the line using ',' as delimiter
		getline (iss, token, ',');
		string ip = token;
		cout << "ip: " << ip << endl;
		getline (iss, token, ',');
		string datetime = token;
		getline (iss, token, ',');
		datetime = datetime + "," + token;
		cout << "datetime: " << datetime << endl;
		// nothing after is useful

		//parse datetime
		istringstream dt (datetime);
		sys_seconds tp;
		dt >> parse("%F,%T", tp);
		cout << tp << endl;

		//clean inactive users





		//update active users
		if (M.find(ip) == M.end()) {
			L.push_front({ip, tp});
			M[ip] = {tp, L.begin()};
		} else {
			//already exists
			if (tp == M[ip].end) {
				//multiple requests at the same time
				//just update the count
				M[ip].count += 1;
			} else {
				//end time needs to be updated
				L.erase(M[ip].it);
				L.push_front({ip, tp});
				M[ip].end = tp;
				M[ip].count += 1;
				M[ip].it = L.begin();
			}
		}
	}
}


