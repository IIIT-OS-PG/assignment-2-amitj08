//  2019201037 Amit Jindal

#include "peerconfig.h"

string stop_share(vector<string> cmdVec, string client_ip) //  Remove file functionality.
{
    string hash, msg, delimiter = ",";
    string line;
    struct stat statObj;
    if (stat(cmdVec[1].c_str(), &statObj) == -1)
    {
        cout << "File does not exist" << endl;
        logwriter("File does not exist");
        return "$";
    }
    // To get hash from mtorrent file
    ifstream iff(cmdVec[1]);
    getline(iff, line);
    getline(iff, line);
    getline(iff, line);
    getline(iff, line);
    getline(iff, line);
    hash = generateHashOfString(line); // Generate hash of hash.
    logwriter(hash);
    msg = cmdVec[0] + delimiter + client_ip + delimiter + hash;
    logwriter(msg);
    return msg;
}