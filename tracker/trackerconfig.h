//  2019201037 Amit Jindal

#include <bits/stdc++.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

//  All client services are included in this function like upload,download,stop_share,logout.
void *clientService(void *);

// For printing logs 
void logwriter(string);

// For splitting the string.
vector<string> stringSplitter(string, char);

// For updation of seederList file.
void updateSeederFile();

// Print DataStructure.
void printDS();
