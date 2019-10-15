// 2019201037 Amit Jindal

#include "peerconfig.h"

string download_file(vector<string> cmdVec) // Download command functionality.
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
    // Read hash string from mtorrent file.
    ifstream iff(cmdVec[1]);
    getline(iff, line);
    getline(iff, line);
    getline(iff, line);
    getline(iff, line);
    getline(iff, line);
    hash = generateHashOfString(line); // Generate hash of hash.
    logwriter(hash);
    msg = cmdVec[0] + delimiter + hash;
    logwriter(msg);
    return msg;
}