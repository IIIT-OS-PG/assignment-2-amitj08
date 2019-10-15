//  2019201037 Amit Jindal

#include "peerconfig.h"

string upload_file(vector<string> cmdVec, string client_ip, string tracker_ip1, string tracker_ip2)
{
    string delimiter = ",", msg;
    string hash = torrentCreater(cmdVec[1], cmdVec[2], tracker_ip1, tracker_ip2);
    if (hash == "$")
        return "$";
    hash = generateHashOfString(hash);
    msg = cmdVec[0] + delimiter + hash + delimiter + client_ip + delimiter + cmdVec[1];
    logwriter(msg);
    return msg;
}
// When client starts, this function will share all the files (torrent files) with tracker.
string startshare(string fname, string client_ip)
{
    string delimiter = ",", msg, line, hash;
    struct stat statObj;
    if (stat(fname.c_str(), &statObj) == -1)
    {
        cout << "File does not exist" << endl;
        logwriter("File does not exist");
        return "$";
    }
    // Read hash string from torrent file.
    ifstream iff(fname);
    getline(iff, line);
    getline(iff, line);
    getline(iff, line);
    getline(iff, line);
    getline(iff, line);
    hash = generateHashOfString(line); // Generate hash of hash
    msg = "upload_file" + delimiter + hash + delimiter + client_ip + delimiter + fname;
    logwriter(msg);
    return msg;
}