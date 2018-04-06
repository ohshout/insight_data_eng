# Dependencies:
Please use g++ > 5.0. Tested wth g++ 5.4.0

# Data Structures:
My main data structure is a hash table (named *active_users_dict*) that maps an active user's ip address
to his/her activity record, which is composed of the first access time, the latest access time and number of accesses.

There are two associative data structures. Both are linked list. One is named *last_access* and stores
each active user's ip address and his/her latest access time. The list is ordered by access time.
The tail of the list stores the earliest one.

The other is named *active_users_ordered_by_start* and stores each active user's ip address ordered
by his/her first access time. The head of the list stores the earliest one.

# Algorithm Design
The program reads the input file line by line. For each line, it extracts the ip address and the current time.

## Clean inactive users
The program iterates over the *last_access* list starting from the tail.
For each list node, if ((this node's timestamp - cur) > *inactivity_period*), indicating the user has gone inactive,
the program then retrieves the stored ip address and find the user's record in *active_users_dict*. It then
dumps the user's activity to the output file.

## Insert the new record
The program then creates or updates a user record, reflecting the information in the current line. It also updates
the associative data structures accordingly.

## Finish and dump
When the program encounters the end of the input file, it iterates over the *active_users_ordered_by_start* list
starting from the head. It dumps records of each user on the list, to the output file.

# Unit Tests:
I'm not uploading all of my test files because they're so large that Github doesn't allow me to upload...
