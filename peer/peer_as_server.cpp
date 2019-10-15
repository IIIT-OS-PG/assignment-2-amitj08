//  2019201037 Amit Jindal

#include "peerconfig.h"

void *serverservice(void *clientIP) // Peer listening as a server to serve other clients.
{
    int server_fd;
    struct sockaddr_in address;
    int opt = 1, new_socket;
    int addrlen = sizeof(address);
    string cip = *(string *)clientIP;
    string client_ip, SPORT;
    char *s_ip;
    vector<string> cmd = stringSplitter(cip, ':');
    client_ip = cmd[0];
    SPORT = cmd[1];
    s_ip = new char[client_ip.length()];
    strcpy(s_ip, client_ip.c_str());
    //  Connection establishment code.
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        logwriter("socket failed -- server");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        logwriter("setsocketopt -- server");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_port = htons(stoi(SPORT));

    if (inet_pton(AF_INET, s_ip, &address.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        logwriter("Invalid address/ Address not supported -- server");
        return clientIP;
    }

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        logwriter("bind failed -- server");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("listen");
        logwriter("listen error -- server");
        exit(EXIT_FAILURE);
    }
    pthread_t peerName;

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            logwriter("accept error -- server");
            exit(EXIT_FAILURE);
        }
        logwriter("connection established -- server");

        // Create new thread for each client and pass socket number as an argument.
        int *arg = (int *)malloc(sizeof(*arg));
        *arg = new_socket;
        pthread_create(&peerName, 0, serverclientservice, arg);
        //close(new_socket);
    }
    return clientIP;
}

void *serverclientservice(void *peerName)
{
    int new_socket = *((int *)peerName);
    char buffer[1024] = {0};
    read(new_socket, buffer, 1024);
    logwriter("Peer gets request from client");
    logwriter(string(buffer));
    string fname = string(buffer);
    logwriter(fname);
    ifstream readInputFile(fname, ifstream::binary);
    long fsize, chksz, flag = 0 ;

    struct stat statObj;
    stat(fname.c_str(), &statObj);
    fsize = statObj.st_size; // Size of the file.
    chksz = chunksize;
    //readInputFile.seekg(0, ios::beg);
    long no_Of_Chunks = fsize / chksz; // Calculate number of chunks.
    if (no_Of_Chunks == 0)
    {
        no_Of_Chunks = 1;
        chksz = fsize;
    }
    logwriter("Peer sending file to client");
    char *buff;

    //  Send file from server to client.
    while (no_Of_Chunks > 0)
    {
        buff = new char[chksz];
        readInputFile.read(buff, chksz);
        send(new_socket, buff, chksz, 0);
        //logwriter(to_string(no_Of_Chunks));
        no_Of_Chunks--;
        if (no_Of_Chunks == 0 && flag == 0)
        {
            flag = 1;
            long int lastchksz = fsize % chksz;
            if (lastchksz != 0)
            {
                no_Of_Chunks = 1;
                chksz = lastchksz;
            }
        }
    }
    close(new_socket);
    readInputFile.close();
    logwriter("File sent successfully -- server");
    return peerName;
}

/*
#include "headerfile.h"
#define PORT 5004
int main()
{

//creating socket()    
int server_sockid =socket(AF_INET, SOCK_STREAM, 0);
if(server_sockid<0){
    cout<<"Error in connection"<<endl;
   	exit(1);
    }
    cout<<"1. Client Socket is created.\n";
//bind()
struct sockaddr_in addr;
addr.sin_family =AF_INET;
addr.sin_port = htons(PORT);
addr.sin_addr.s_addr=INADDR_ANY;
int addrlen = sizeof(sockaddr);
int status=bind (server_sockid,(struct sockaddr *)&addr,sizeof(addr));
//listen
listen (server_sockid,3);
//accept
int sockfd =accept(server_sockid ,(struct sockaddr *)&addr,(socklen_t*)&addrlen);
//recv
FILE *fp =fopen( "abc.txt" , "wb" );
char Buffer[BUFF_SIZE]; 
int file_size;

recv(server_sockid, &file_size, sizeof(file_size),0);
int n;
while ((n=recv(sockfd,Buffer,BUFF_SIZE,0))>0 && file_size>0){
fwrite (Buffer,sizeof(char),n,fp);
memset ( Buffer , '\0', BUFF_SIZE);
file_size = file_size - n;
} 

close(sockfd);
close(server_sockid);
fclose(fp);
}
*/