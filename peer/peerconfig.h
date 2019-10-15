// 2019201037 Amit Jindal

#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/sha.h>
#include <sstream>
#include <fstream>
#include <pthread.h>
using namespace std;

extern string logs;
extern long int chunksize; // Predefined chunk size (512*1024)

//  To calculate hash of a given file.
string calculateHash(string, long int, long int);

// Create mtorrent file.
void createFile(string, string, string, string, long int, string);

//  interactive function which interact with caller function and return hash by calling
//  calculate hash function.
string torrentCreater(string, string, string, string);

//  To calculate  hash of hash.
string generateHashOfString(string);

//  Share command functionality.
string upload_file(vector<string>, string, string, string);

//  When client starts, all mtorrent files will be shared.
string startshare(string, string);

//  Remove command functionality.
string stop_share(vector<string>, string);

//  Download command functionality/
string download_file(vector<string>);

//  Peer which act as server to serve other peers.
void *serverservice(void *);

//  To handle multiple peers simultaneously.
void *serverclientservice(void *);

//  To print logs into log file.
void logwriter(string);

//  Download file form server(Act as server).
void *getclient(void *);

//  String Splitter
vector<string> stringSplitter(string, char);
