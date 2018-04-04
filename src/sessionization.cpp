#include <list>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cassert>
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
	//if (argc != 4) {
	//	cout << "Insufficient arguments!" << endl;
	//	exit(EXIT_FAILURE);
	//}

	unordered_map<string, userRecord> dict;
	list<pair<string, sys_seconds>> L; //<ip, time>
	sys_seconds prev;

	//read inactivity period value
	//ifstream period_file (argv[2]);
	ifstream period_file ("./input/inactivity_period.txt");
	string period_s;
	getline(period_file, period_s);
	unsigned int period = stoi(period_s);
	unsigned int period_ms = 1000*period;
	cout << "period: " << period << endl;

	// open input log file
	//ifstream infile (argv[1]);
	ifstream infile ("./input/log.csv");
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
		//cout << tp << endl;

	//	auto diff = std::chrono::duration_cast<std::chrono::milliseconds> (tp-prev);
	//	unsigned int diff_ms = diff.count();

	//	cout << diff_ms << "ms" << endl;
	//	prev = tp;

		//clean inactive users
		while (L.size()) {
			auto l = L.back();
			auto it = dict.find(l.first);
			assert(it != dict.end());
			auto record = it->second;
			auto diff = std::chrono::duration_cast<std::chrono::milliseconds> (tp - record.end);
			unsigned int diff_ms = diff.count();
			if (diff_ms > period_ms) {
				cout << "printing to file: " << l.first << " " << record.count << endl;
				dict.erase(l.first);
				L.pop_back();
			} else
				break;
		}

		//update active users
		auto it = dict.find(ip);
		if (it == dict.end()) {
			L.push_front({ip, tp});
			dict.insert(make_pair(ip, userRecord{tp, L.begin()}));
		} else {
			auto& record = it->second;
			//already exists
			if (tp == record.end) {
				//multiple requests at the same time
				//just update the count
				record.count += 1;
			} else {
				//end time needs to be updated
				L.erase(record.it);
				L.push_front({ip, tp});
				record.end = tp;
				record.count += 1;
				record.it = L.begin();
			}
		}
	}

	//before finish simulation, print all remaining active users
	//in the ascending order of start time
}


