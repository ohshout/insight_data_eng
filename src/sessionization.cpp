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
//TODO update or clean first, think about corner case!

using namespace date;
using namespace std;

class Solution {

public:
	struct userRecord {
		date::sys_seconds start;
		date::sys_seconds end;
		int count;
		list<pair<string, date::sys_seconds>>::iterator last_acc_it;
		list<string>::iterator start_list_it;
		userRecord (date::sys_seconds t,
							  list<pair<string, date::sys_seconds>>::iterator i1,
								list<string>::iterator i2) : start(t), end(t), count(1), last_acc_it (i1), start_list_it (i2) {}
	};

	Solution (ofstream& of, unsigned int period) : outfile (of), period_ms (period) {}
	void process_line (string line);
	void finish_and_dump ();

private:
	unordered_map<string, userRecord> active_users_dict;
	list<pair<string, sys_seconds>> last_access; //<ip, time>
	list<string> active_users_ordered_by_start;
	unsigned int period_ms;
	ofstream& outfile;

	void clean_users (date::sys_seconds cur);
	void update_user (const string& ip, date::sys_seconds tp);
	void dump_user (const string& ip, const userRecord& record);
};

/** 
	* Line format: 
	* 101.81.133.jja,2017-06-30,00:00:00,0.0,1608552.0,0001047469-17-004337,-index.htm,200.0,80251.0,1.0,0.0,0.0,9.0,0.0,
	* Split up the line using ',' as delimiter. 
	* Extract ip and datetime. That's all we care about.
	*/
void Solution::process_line (string line)
{
	istringstream iss (line);
	string token;

	// extract ip
	std::getline (iss, token, ',');
	string ip = token;
	cout << "ip: " << ip << endl;

	// extract datetime
	getline (iss, token, ',');
	string datetime = token;
	getline (iss, token, ',');
	datetime = datetime + "," + token;
	cout << "datetime: " << datetime << endl;
	// parse datetime
	istringstream dt (datetime);
	date::sys_seconds tp;
	dt >> date::parse("%F,%T", tp);

	// use tp as the current time
	// and clean up inactive users
	clean_users(tp);

	// insert current user as an active one
	update_user (ip, tp);
}

/**
	* Iterate over the list of users' last access time from the back.
	* For a user, if (last_access_time - cur) > period,
	* the user has gone inactive, dump the user's activity to file.
	*/
void Solution::clean_users (date::sys_seconds cur)
{
	while (last_access.size()) {
		// new access was pushed to the head of the list
		// so we scan from the back to get the oldest one
		auto l = last_access.back();
		string ip = l.first;
		auto it = active_users_dict.find(ip);
		// user must be currently active
		assert(it != active_users_dict.end());

		//retrieve the user's record to check its last access time
		auto record = it->second;
		auto diff = std::chrono::duration_cast<std::chrono::milliseconds> (cur - record.end);
		unsigned int diff_ms = diff.count();
		if (diff_ms > period_ms) {
			// the user has gone inactive
			// dump the user activity to file
			dump_user (ip, record);

			// housekeeping:
			// 1. remove the user from the active user list ordered by start time
			// 2. remove the user from the active user hash
			// 3. remove the user from the last access time list
			active_users_ordered_by_start.erase(record.start_list_it);
			active_users_dict.erase(ip);
			last_access.pop_back();
		} else {
			// this user is active
			// meaning that all other users with later access time must be active
			break;
		}
	}
}

/**
	* dump user record into file
	*/
void Solution::dump_user (const string& ip, const userRecord& record)
{
	auto dur = std::chrono::duration_cast<std::chrono::milliseconds> (record.end - record.start);
	//std::cout << "printing to file: " << ip << "," << record.start << "," << record.end << "," \
		<< dur.count()/1000 << "," << record.count << std::endl;
	outfile << ip << "," << record.start << "," << record.end << "," \
		<< dur.count()/1000 + 1<< "," << record.count << endl;
}

/**
	* create or update active user record
	*/
void Solution::update_user (const string& ip, date::sys_seconds tp)
{
	auto it = active_users_dict.find(ip);
	if (it == active_users_dict.end()) {
		// For new user:
		// 1. push into last_access
		// 2. push into active_users_ordered_by_start
		// 3. insert into active_users_dict
		last_access.push_front({ip, tp});
		active_users_ordered_by_start.push_front(ip);
		active_users_dict.insert(
				make_pair(ip, userRecord{tp, last_access.begin(), active_users_ordered_by_start.begin()}));
	} else {
		// existing user
		auto& record = it->second;
		if (tp == record.end) {
			// multiple requests at the same time
			// just update the count
			record.count += 1;
		} else {
			// new request at a later time
			// 1. update last_access
			// 2. update iterator to last_accessin active_users_dict
			// 3. update end time in active_users_dict
			// 4. update count in active_users_dict
			last_access.erase(record.last_acc_it);
			last_access.push_front({ip, tp});
			record.last_acc_it = last_access.begin();
			record.end = tp;
			record.count += 1;
		}
	}
}

/**
	* Dump all remaining active users in the ascending order of start time.
	*/
void Solution::finish_and_dump ()
{
	for (auto it = active_users_ordered_by_start.rbegin();
			 it != active_users_ordered_by_start.rend();
			 it++) 
	{
		string ip = *it;
		auto dict_it = active_users_dict.find(ip);
		// user must be currently active
		assert(dict_it != active_users_dict.end());
		auto& record = dict_it->second;
		dump_user(ip, record);
	}
}

int main(int argc, char *argv[])
{
	// check arguments
	if (argc != 4) {
		cout << "Insufficient arguments!" << endl;
		exit(EXIT_FAILURE);
	}

	// read inactivity period value
	ifstream period_file (argv[2]);
	//ifstream period_file ("./input/inactivity_period.txt");
	string period;
	getline(period_file, period);
	unsigned int period_ms = 1000*stoi(period);
	cout << "period: " << period_ms << endl;

	// open output file
	ofstream outfile (argv[3]);

	// init simulation
	Solution sol (outfile, period_ms);

	// open input log file
	ifstream infile (argv[1]);
	//ifstream infile ("./input/log.csv");

	string line;
	// skip the header line
	getline (infile, line);
	// iterate over every following line
	while (getline (infile, line)) {
		sol.process_line(line);
	}

	// before finish simulation, dump all remaining active users
	sol.finish_and_dump();
}


