// 2019201037 Amit Jindal

#include "trackerconfig.h"

struct Data
{
    string IP, path;
};
map<string, vector<struct Data>> storeData; // Datastructure to store hash,ip,and filepath.
vector<string> cmd;
string seedersList="seedersList";
string logs="logs";
string sip2, p2;

int main(int argc, char **argv)
{
    int server_fd, PORT;
    struct sockaddr_in address;
    int opt = 1, new_socket;
    int addrlen = sizeof(address);
    string IP_1, IP_2, sip1 = "", p1 = "", line;
    if (argc != 3)
        exit(EXIT_FAILURE);
    else
    {
        IP_1 = string(argv[1]);     // Tracker's primary ip.
        IP_2 = string(argv[2]);     // Trackers's secondary ip.

        // Load all data which is present in seederList file to DataStructure.
        ofstream of;
        of.open(logs, fstream::out);
        of.close();
        logwriter("Log file created in Tracker Machine");

        ifstream infile(seedersList);
        while (getline(infile, line))
        {
            vector<struct Data> v;
            string h = "", path = "", ip = "";
            cmd = stringSplitter(line, ' ');
            h = cmd[0];
            ip = cmd[1];
            for (unsigned int i = 2; i < cmd.size(); i++)
                path = path + cmd[2] + " ";
            path.pop_back();
            struct Data d;
            d.IP = ip;
            d.path = path;
            v.push_back(d);
            if (storeData.find(h) == storeData.end())
                storeData.insert(pair<string, vector<struct Data>>(h, v));
            else
            {
                vector<struct Data> tmp = storeData[h];
                tmp.push_back(d);
                storeData[h] = tmp;
            }
        }

            // Split Ip and port.
            cmd = stringSplitter(IP_1, ':');
            sip1 = cmd[0]; 
            p1 = cmd[1];
            PORT = stoi(p1);
            cmd = stringSplitter(IP_2, ':');
            sip2 = cmd[0];
            p2 = cmd[1];
        
    }
    char *s_ip;
    s_ip = new char[sip1.length() + 1];
    strcpy(s_ip, sip1.c_str());

    // Connection establishment code.
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        logwriter("socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        logwriter("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, s_ip, &address.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        logwriter("Invalid address/ Address not supported");
        return -1;
    }

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        logwriter("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 32) < 0)
    {
        perror("listen failed");
        logwriter("listen failed");
        exit(EXIT_FAILURE);
    }
    cout<<"Tracker is up and running"<<endl;
    pthread_t clientName;
    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept failed");
            logwriter("accpet failed");
            exit(EXIT_FAILURE);
        }
        logwriter("connection established");
        // Every time new thread to serve each client and pass socket number as argument.
        int *arg = (int *)malloc(sizeof(*arg));
        *arg = new_socket;
        pthread_create(&clientName, 0, clientService, arg);
    }
    cout << "Tracker Closed!!!" << endl;
    logwriter("Tracker closed");
}
void *clientService(void *scno)
{
    int new_socket = *((int *)scno);
    char *hello;
    string msg;
    do
    {
        int flag = 0;
        string command = "";
        char buffer[1024] = {0};
        int x = read(new_socket, buffer, 1024); //  Get data from client.
        if (x == 0)
        {
            return scno;
        }
        logwriter("Request from client");
        logwriter(string(buffer));
        struct Data d;
        cmd = stringSplitter(string(buffer), ','); // Seperate data by comma.
        command = cmd[0];
        if (command == "upload_file") // upload_file file functionality.
        {
            buffer[1024] = {0};
            string hash = "";
            cmd = stringSplitter(string(buffer), ',');
            command = cmd[0];
            hash = cmd[1];
            d.IP = cmd[2];
            d.path = cmd[3];
            vector<struct Data> v = storeData[hash];
            flag = 0;
            if (v.size() == 0)
            {
                storeData[hash].push_back(d); // New shared data push into Datastructure.
                // New shared data push into seederList File.
                ofstream outfile;
                outfile.open(seedersList, ios_base::app | ios_base::out);
                outfile << hash << " " << d.IP << " " << d.path << endl;
                outfile.close();
                msg = "Successfully uploaded";
                logwriter("File successfully uploaded");
            }
            else
            {
                for (auto j = v.begin(); j != v.end(); j++)
                {
                    struct Data st = *j;
                    if (st.IP == d.IP)
                    {
                        flag = 1;
                        break;
                    }
                }
                if (flag == 0)
                {
                    storeData[hash].push_back(d);         // New shared data push into Datastructure.
                                                           // New shared data push into seederList File.
                    ofstream of;
                    of.open(seedersList, ios_base::app | ios_base::out);
                    of<< hash << " " << d.IP << " " << d.path << endl;
                    of.close();
                    msg = "Successfully upload_filed";
                    logwriter("File successfully upload_filed");
                }
                else
                {
                    msg = "File is already upload_filed";
                    logwriter("File is already upload_filed");
                }
            }
            //printDS();    // print all data present in Datastructure.
            hello = new char[msg.length() + 1];
            strcpy(hello, msg.c_str());
            send(new_socket, hello, strlen(hello), 0); // Send data to client.
        }
        else if (command == "stop_share") // stop_share share file functionality.
        {
            buffer[1024] = {0};
            string cip, hash = "";
            cmd = stringSplitter(string(buffer), ',');
            command = cmd[0];
            cip = cmd[1];
            hash = cmd[2];
            // Remove shared file entry from Datastructure.
            vector<struct Data> v = storeData[hash];
            flag = 0;
            int chk = 0;
            for (auto it = v.begin(); it != v.end(); it++)
            {
                if ((*it).IP == cip)
                {
                    if (v.size() == 1)
                    {
                        chk = 1;
                        flag = 11;
                        break;
                    }
                    else
                    {
                        chk = 1;
                        v.erase(it);
                        flag = 1;
                        break;
                    }
                }
            }
            if (flag == 1)
                storeData[hash] = v;
            else if (flag == 11)
                storeData.erase(hash);
            if (chk == 1)
            {
                logwriter("File successfully removed");
                msg = "File successfully removed";
            }
            else
            {
                logwriter("File does not exist on the server");
                msg = "File does not exist on the server";
            }
            hello = new char[msg.length() + 1];
            strcpy(hello, msg.c_str());
            send(new_socket, hello, strlen(hello), 0); // Send data to client.
            updateSeederFile();                              // Update seederList File.
        }
        else if (command == "download_file") // Download File functionality.
        {
            buffer[1024] = {0};
            string hash;
            msg = "";
            cmd = stringSplitter(string(buffer), ',');
            command = cmd[0];
            hash = cmd[1];
            vector<struct Data> v = storeData[hash];
            if (v.size() == 0)
            {
                msg = "File not found";
                logwriter("File not found");
            }
            else
            {
                // Collect list of IPs and File paths and send it to client.
                for (unsigned int j = 0; j < v.size(); j++)
                {
                    if (j == 0)
                        msg = v[j].IP + "@" + v[j].path;
                    else
                        msg = msg + "," + v[j].IP + "@" + v[j].path;
                }
                logwriter("File found");
                logwriter(msg);
            }
            hello = new char[msg.length() + 1];
            strcpy(hello, msg.c_str());
            send(new_socket, hello, strlen(hello), 0); // Send data to client.
        }
        else if (command == "logout") // Client logout functionality.
        {
            buffer[1024] = {0};
            string ip = "", hash, tip;
            cmd = stringSplitter(string(buffer), ',');
            command = cmd[0];
            ip = cmd[1];
            // Remove all entries from Datastructure that has requested ip.
            for (auto it = storeData.begin(); it != storeData.end(); it++)
            {
                hash = (*it).first;
                flag = 0;
                vector<struct Data> v = (*it).second;
                for (auto it1 = v.begin(); it1 != v.end(); it1++)
                {
                    tip = (*it1).IP;
                    if (tip == ip)
                    {
                        if (v.size() == 1)
                        {
                            flag = 11;
                            break;
                        }
                        else
                        {
                            v.erase(it1);
                            flag = 1;
                            break;
                        }
                    }
                }
                if (flag == 1)
                    storeData[hash] = v;
                else if (flag == 11)
                    storeData.erase(hash);
            }
            msg = "Logged Out";
            hello = new char[msg.length() + 1];
            strcpy(hello, msg.c_str());
            send(new_socket, hello, strlen(hello), 0); // Send data to client.
            updateSeederFile();                              // Update seederList file.
            close(new_socket);
            logwriter("Logged Out");
            break;
        }
    } while (1);
    return scno;
}
// To print log into logfile.
void logwriter(string s)
{
    ofstream of;
    of.open(logs, ios_base::app | ios_base::out);
    of << s << endl;
    of.close();
}

// String Splitter
vector<string> stringSplitter(string str, char delimiter)
{
    vector<string> tokens;
    int i = 0;
    string s = "";
    while (str[i] != '\0')
    {
         if (str[i] == delimiter)
        {
            tokens.push_back(s);
            s = "";
        }
        else
        {
            s = s + str[i];
        }
        i++;
    }
    tokens.push_back(s);
    return tokens;
}

// For updating seeder file.
void updateSeederFile()
{
    string hash, tip;
    char *sl;
    sl = new char[seedersList.length() + 1];
    strcpy(sl, seedersList.c_str());
    remove(sl);
    ofstream outstream;
    outstream.open(seedersList, fstream::out);
    outstream.close();
    outstream.open(seedersList, ios_base::app | ios_base::out);
    for (auto it = storeData.begin(); it != storeData.end(); it++)
    {
        string path;
        hash = (*it).first;
        vector<struct Data> v = (*it).second;
        for (unsigned int j = 0; j < v.size(); j++)
        {
            tip = v[j].IP;
            path = v[j].path;
            outstream << hash << " " << tip << " " << path << endl;
        }
    }
    outstream.close();
}

// Datastruture print.
void printDS()
{
    cout << "******DATA*******" << endl;
    for (auto i = storeData.begin(); i != storeData.end(); i++)
    {
        string h = (*i).first;
        cout << h << endl;
        vector<struct Data> v = (*i).second;
        for (unsigned int j = 0; j < v.size(); j++)
        {
            struct Data x = v[j];
            cout << v[j].IP << endl;
            cout << v[j].path << endl;
        }
    }
}
