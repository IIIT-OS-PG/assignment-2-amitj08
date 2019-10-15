//  2019201037 Amit Jindal

//Peer working as client

#include "peerconfig.h"

map<string, string> downloads;
vector<string> cmd;
string client_ip, tracker_ip1, tracker_ip2;
long int chunksize = 524288; // 512KB
string logs = "logs";

struct arg_struct // Structure to be passed to download file.
{
    char *torrent, *destination, *fname, *ip, *port;
    int sock;
};

int main(int argc, char **argv)
{
    string c_ip, c_port, tip_1, tip_2, tport_1, tport_2;
    int i = 0;
    if (argc < 1)
    {
        exit(EXIT_FAILURE);
    }
    else
    {
        if (argc != 4)
        {
            cout << "Invalid Argument format" << endl;
            return 0;
        }
        else
        {
            client_ip = string(argv[1]);   // Client IP.
            tracker_ip1 = string(argv[2]); // Primary tracker IP.
            tracker_ip2 = string(argv[3]); // Secondary tracker IP.

            // Creation of log file.
            ofstream of;
            of.open(logs, fstream::out);
            of.close();
            logwriter("Log file created in Peer Machine");
            cmd = stringSplitter(client_ip, ':'); // Seperate client ip and port.
            c_ip = cmd[0];
            c_port = cmd[1];
            cmd = stringSplitter(tracker_ip1, ':'); // Seperate tracker-1 ip and port.
            tip_1 = cmd[0];
            tport_1 = cmd[1];
            cmd = stringSplitter(tracker_ip2, ':'); // Seperate tracker-2 ip and port.
            tip_2 = cmd[0];
            tport_2 = cmd[1];
        }
    }

    pthread_t serverThread;
    // Start serverservice function in which Peer act as server to give files to other clients.
    // It will keep running until client shuts down.
    pthread_create(&serverThread, 0, serverservice, (void *)&client_ip);
    int sock = 0;
    struct sockaddr_in serv_addr;
    string IP = tip_1 + ":" + tport_1;
    string command;
    char *sip_1;
    sip_1 = new char[tip_1.length() + 1];
    strcpy(sip_1, tip_1.c_str());

    // Connection establishment code.
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        logwriter("Socket creation error");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(stoi(tport_1));

    if (inet_pton(AF_INET, sip_1, &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        logwriter("Invalid address/ Address not supported");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        logwriter("Connection failed");
        return -1;
    }
    logwriter("Connection established");

    // All torrent files will be shared when client turns on.
    struct dirent **namelist;
    int n = scandir(".", &namelist, NULL, alphasort);
    n--;
    while (n >= 0)
    {
        string s1 = string(namelist[n]->d_name);
        string s2 = ".torrent";
        if (s1.find(s2) != string::npos)
        {
            string sharemsg = startshare(s1, client_ip);
            char *hello;
            char buffer[1024] = {0};
            hello = new char[sharemsg.length() + 1];
            strcpy(hello, sharemsg.c_str());
            send(sock, hello, strlen(hello), 0); // Send data to tracker.
            read(sock, buffer, 1024);            // Receive data from tracker.
            logwriter(string(buffer));
        }
        n--;
    }
    do
    {
        string s = "";
        vector<string> cmdVec;
        cmdVec.clear();
        getline(cin, command); // Command entered by user.
        logwriter(command);
        cmdVec = stringSplitter(command, ' '); // Seperate command.

        if (cmdVec[0] == "upload_file") // upload_file command functionality.
        {
            cmdVec[2] = cmdVec[2] + ".torrent";
            logwriter("Client wants to upload_file something");
            if (cmdVec.size() != 3)
            {
                cout << "Invalid upload_file command" << endl;
                logwriter("Invalid upload_file command");
                continue;
            }
            char buffer[1024] = {0};
            char *hello;
            string msg = upload_file(cmdVec, client_ip, tracker_ip1, tracker_ip2);
            if (msg == "$")
                continue;
            hello = new char[msg.length() + 1];
            strcpy(hello, msg.c_str());
            send(sock, hello, strlen(hello), 0); // Send data to tracker.
            read(sock, buffer, 1024);            // Receive data from tracker.
            cout << string(buffer) << endl;
            logwriter(string(buffer));
        }
        else if (cmdVec[0] == "download_file") // Download functionality.
        {
            cmdVec[1] = cmdVec[1] + ".torrent";
            logwriter("Client wants file from server");
            if (cmdVec.size() != 3)
            {
                cout << "Invalid download_file command" << endl;
                logwriter("Invalid download_file command");
                continue;
            }
            string msg = download_file(cmdVec);
            char *hello;
            char buffer[1024] = {0};
            if (msg == "$")
                continue;
            hello = new char[msg.length() + 1];
            strcpy(hello, msg.c_str());
            send(sock, hello, strlen(hello), 0); // Send command to tracker.
            read(sock, buffer, 1024);            // Receive list of ip and file paths from tracker.
            logwriter("Client download_file reply");
            logwriter(string(buffer));
            vector<pair<string, string>> noOfIps;
            string getip, getpath;
            i = 0;
            s = "";
            // Seperate ip and paths that is received from tracker.
            while (buffer[i] != '\0')
            {
                if (buffer[i] == '@')
                {
                    getip = s;
                    s = "";
                }
                else if (buffer[i] == ',')
                {
                    getpath = s;
                    noOfIps.push_back(pair<string, string>(getip, getpath));
                    s = "";
                }
                else
                    s += buffer[i];
                i++;
            }
            getpath = s;
            noOfIps.push_back(pair<string, string>(getip, getpath));
            string csip, csport, cs = getip;
            cmd = stringSplitter(cs, ':');
            csip = cmd[0];
            csport = cmd[1];
            struct arg_struct args;
            args.fname = new char[noOfIps[0].second.length() + 1];
            strcpy(args.fname, noOfIps[0].second.c_str());
            args.port = new char[csport.length() + 1];
            strcpy(args.port, csport.c_str());
            args.ip = new char[csip.length() + 1];
            strcpy(args.ip, csip.c_str());
            args.torrent = new char[cmdVec[1].length() + 1];
            strcpy(args.torrent, cmdVec[1].c_str());
            args.destination = new char[cmdVec[2].length() + 1];
            strcpy(args.destination, cmdVec[2].c_str());
            args.sock = sock;
            pthread_t threadId;
            // getclient fn will contact peer to download file.
            pthread_create(&threadId, 0, getclient, (void *)&args);
        }
        else if (cmdVec[0] == "stop_share") // Stop sharing functionality.
        {
            cmdVec[1] = cmdVec[1] + ".torrent";
            logwriter("Peer wants to stop sharing shared file");
            if (cmdVec.size() != 2)
            {
                cout << "Invalid stop_share command" << endl;
                logwriter("Invalid stop_share command");
                continue;
            }
            string msg = stop_share(cmdVec, client_ip);
            char *hello;
            char buffer[1024] = {0};
            if (msg == "$")
                continue;
            hello = new char[msg.length() + 1];
            strcpy(hello, msg.c_str());
            send(sock, hello, strlen(hello), 0); // Send command to tracker.
            read(sock, buffer, 1024);            // Receive data from tracker.
            string chkStatus = string(buffer);
            if (chkStatus == "File successfully removed")
            {
                logwriter("File removed successfully");
                cout << "File removed successfully" << endl;
                char *rmfile;
                rmfile = new char[cmdVec[1].length() + 1];
                strcpy(rmfile, cmdVec[1].c_str());
                remove(rmfile);
            }
            else
            {
                cout << "File was not shared" << endl;
                logwriter("File was not shared");
            }
        }
        else if (cmdVec[0] == "logout") // Client shut down functionality.
        {
            if (cmdVec.size() != 1)
            {
                cout << "Invalid close command" << endl;
                logwriter("Invalid close command");
                continue;
            }
            logwriter("client wants to close connection");
            char *hello;
            char buffer[1024] = {0};
            string msg = cmdVec[0] + "," + client_ip;
            hello = new char[msg.length() + 1];
            strcpy(hello, msg.c_str());
            send(sock, hello, strlen(hello), 0); // Send command to tracker.
            read(sock, buffer, 1024);            // Receive data from tracker.
            close(sock);
            logwriter("connection closed");
            cout << "Connection closed" << endl;
            break;
        }
        else if (cmdVec[0] == "show_downloads") // Show downloads functionality.
        {
            if (cmdVec.size() != 1)
            {
                cout << "Invalid show_downloads command" << endl;
                logwriter("Invalid show_downloads command");
                continue;
            }
            cout << endl;
                 cout<< "*****Downloads*****" << endl;
            if (downloads.size() == 0)
                cout << "No downloads" << endl;
            else
            {
                for (auto it = downloads.begin(); it != downloads.end(); it++)
                    cout << (*it).second << " - " << (*it).first << endl;
            }
        }
        else // Handle validation
        {
            cout << "Invalid command. Please type again." << endl;
            logwriter("Invalid command. Please type again.");
        }
    } while (1);
    cout << "Exiting.." << endl;
    return 0;
}

// To print log into log file.
void logwriter(string s)
{
    ofstream of;
    of.open(logs, ios_base::app | ios_base::out);
    of << s << endl;
    of.close();
}

// Download file from peer offering download facility.
void *getclient(void *arg)
{
    struct arg_struct args = *((struct arg_struct *)arg);
    int sock = 0;
    string msg;
    char *hello;
    // Connection establishment code with server.
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        logwriter("Socket creation error while making Connection with server");
        return arg;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(stoi(string(args.port)));

    if (inet_pton(AF_INET, args.ip, &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        logwriter("Invalid address/ Address not supported while making Connection with server");
        return arg;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        logwriter("Connection failed while making Connection with server");
        return arg;
    }
    logwriter("Connection established while making connection with server");
    char *buffer;
    long int n;
    buffer = new char[chunksize];
    send(sock, args.fname, strlen(args.fname), 0);
    ofstream createFile(args.destination, ofstream::binary);
    logwriter("client send request to server for file");
    string destpath = string(args.destination);
    downloads[destpath] = "D"; // Push file into download Data Structure.
    // Download file.
    do
    {
        buffer = new char[chunksize];
        n = read(sock, buffer, chunksize);
        createFile.write(buffer, n);
    } while (n > 0);
    createFile.close();
    downloads[destpath] = "C";
    logwriter("client get file from server");
    cout << "File downloaded" << endl;
    string mt = string(args.torrent);
    string fs = string(args.destination);
    logwriter(fs);
    vector<string> temp;

    // Automatic share command functionality.
    temp.push_back("upload_file");
    temp.push_back(fs);
    temp.push_back(mt);
    msg = upload_file(temp, client_ip, tracker_ip1, tracker_ip2);
    if (msg == "$")
        return arg;
    logwriter(msg);
    hello = new char[msg.length() + 1];
    strcpy(hello, msg.c_str());
    send(args.sock, hello, strlen(hello), 0);
    read(args.sock, buffer, 1024);
    logwriter(string(buffer));
    return arg;
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

/*
#include "headerfile.h"
#define PORT 5004

int main()
{
//creating socket()
int client_sockid=socket(AF_INET,SOCK_STREAM,0);
    if(client_sockid<0){
    cout<<"Error in connection"<<endl;
   	exit(1);
    }
    cout<<"1. Client Socket is created.\n";
//connect
struct sockaddr_in server_addr;
memset(&server_addr, '\0', sizeof(server_addr));
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons( PORT );
//server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
int status=connect(client_sockid,(struct sockaddr *)&server_addr,sizeof(server_addr));
	if (status<0)
	{
	  printf("Error in connection.\n");
	  exit(1);
	}
	printf("2. Connected to Server %s %d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
//sending file
FILE *fp = fopen ("f2.txt","rb");
fseek ( fp , 0 , SEEK_END);
int size = ftell(fp);
rewind (fp);
cout<<size<<endl;
//int file_size;
send (client_sockid ,&size, sizeof(size), 0);
char Buffer[BUFF_SIZE]; 
int n;
while ((n=fread(Buffer,sizeof(char),BUFF_SIZE,fp))>0 && size>0){
	send(client_sockid ,Buffer,n,0);
   	memset(Buffer,'\0',BUFF_SIZE);
	size =size-n ;
    if(n<=0 || size<=0)
     break;
}
fclose(fp);
//close( client_sockid);
close(client_sockid); //0 if successful;
}
*/

/*
#include<iostream>
#include<pthread.h>

using namespace std;

void yourturn()
{
    for(int i=11;i<21;i++)
    cout<<"yourturn: "<<i<<endl;
}

void* myturn(void* args)
{
    for(int i=1;i<11;i++)
    cout<<"myturn: "<<i<<endl;
    return NULL;
}

int main()
{
    pthread_t newthread;
    pthread_create(&newthread,NULL,myturn,NULL);
    yourturn();

    //wait for the thread to finish its execution
    pthread_join(newthread,NULL);
    cout<<"Thread completed"<<endl;
    return 0;
}
*/
/*
#include <dirent.h>

int alphasort(const struct dirent **d1, const struct dirent **d2);
int scandir(const char *dir, struct dirent ***namelist,
       int (*sel)(const struct dirent *),
       int (*compar)(const struct dirent **, const struct dirent **));

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
...
struct dirent **namelist;
int i,n;


    n = scandir(".", &namelist, 0, alphasort);
    if (n < 0)
        perror("scandir");
    else {
        for (i = 0; i < n; i++) {
            printf("%s\n", namelist[i]->d_name);
            free(namelist[i]);
            }
        }
    free(namelist);
*/