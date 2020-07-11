#include <cstdio>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <list>

using namespace std;

//#define DEBUG_PARA

#define UNKNOWN_MODE        0
#define CLIENT_MODE         1
#define SERVER_MODE         2

#define UNKNOWN_MODE        0
#define SEND_MODE           1
#define RECV_MODE           2

#define UNKNOWN_PROTOCOL    0
#define TCP_PROTOCOL        1
#define UDP_PROTOCOL        2

uint8_t CS_MODE = UNKNOWN_MODE;
uint8_t SR_MODE = UNKNOWN_MODE;
uint8_t PROTOCOL = UNKNOWN_PROTOCOL;
string IP = "127.0.0.1";
uint16_t PORT = 8888;

list<string> lst_argv;
list<string>::iterator iter;

// Help argv: PiNetworkCommSystem.out -h
// Debug argv: -tcp -client -send -ip 192.168.3.8 -port 9999
// Debug argv: -tcp -server -recv -ip 192.168.110.130 -port 9999

// TCP SERVER: SOCKET > BIND > LISTEN > ACCEPT  > RECV > SEND > CLOSE
// TCP CLIENT: SOCKET >>>>>(BIND)>>>>>> CONNECT > SEND > RECV > CLOSE
int main(int argc, char* argv[])
{
    cout << "hello from PiNetworkCommSystem!" << endl;

    for (int i = 1; i < argc; i++)
        lst_argv.push_back(argv[i]);

    for (iter = lst_argv.begin(); iter != lst_argv.end(); *iter++)
    {
        // HELP
        if (string(*iter).compare("-h") == 0)
        {
            cout << endl
                << "--- HELP ---" << endl
                << "PROTOCOL:" << endl
                << "-tcp            TCP PROTOCOL" << endl
                << "-udp            UDP PROTOCOL" << endl
                << "CLIENT/SERVER:" << endl
                << "-client         CLIENT MODE(ONLY FOR TCP)" << endl
                << "-server         SERVER MODE(ONLY FOR TCP)" << endl
                << "SEND/RECV:" << endl
                << "-send           SEND MODE" << endl
                << "-recv           RECV MODE" << endl
                << "ADDRESS:" << endl
                << "-ip 127.0.0.1   IP" << endl
                << "-port 8888      PORT" << endl
                << "EXAMPLE:" << endl
                << "HELP :              PiNetworkCommSystem.out -h" << endl
                << "TCP CLIENT SEND:    PiNetworkCommSystem.out -tcp -client -send -ip 192.168.3.8 -port 9999" << endl
                << "TCP SERVER RECV:    PiNetworkCommSystem.out -tcp -server -recv -ip 192.168.110.130 -port 9999" << endl;
            return 0;
        }

#ifdef DEBUG_PARA
        cout << *iter;
#endif

        if (string(*iter).compare("-tcp") == 0)
            PROTOCOL = TCP_PROTOCOL;
        if (string(*iter).compare("-udp") == 0)
            PROTOCOL = UDP_PROTOCOL;

        if (string(*iter).compare("-client") == 0)
            CS_MODE = CLIENT_MODE;
        if (string(*iter).compare("-server") == 0)
            CS_MODE = SERVER_MODE;

        if (string(*iter).compare("-send") == 0)
            SR_MODE = SEND_MODE;
        if (string(*iter).compare("-recv") == 0)
            SR_MODE = RECV_MODE;

        if (string(*iter).compare("-ip") == 0)
        {
            *iter++;
#ifdef DEBUG_PARA
            cout << " " << *iter;
#endif
            IP = *iter;
        }
        if (string(*iter).compare("-port") == 0)
        {
            *iter++;
#ifdef DEBUG_PARA
            cout << " " << *iter;
#endif
            PORT = atoi(string(*iter).c_str());
        }
#ifdef DEBUG_PARA
        cout << endl;
#endif
    }

    if (PROTOCOL == TCP_PROTOCOL)
    {
        cout << "--- TCP " << ((CS_MODE == CLIENT_MODE) ? "CLIENT" : "SERVER") << " MODE ---" << endl;
        cout << "--- " << IP << ":" << PORT << " ---" << endl;

        // create socket
        int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sockfd < 0)
        {
            cout << "socket create error, return " << sockfd << "." << endl;
            return -1;
        }
        cout << "socket create success, return " << sockfd << "." << endl;

        // socketAddr
        struct sockaddr_in socketAddr;
        socketAddr.sin_family = AF_INET;
        socketAddr.sin_port = htons(PORT);
        socketAddr.sin_addr.s_addr = inet_addr(IP.c_str());

        int connfd;
        // TCP CLIENT
        if (CS_MODE == CLIENT_MODE)
        {
            // connect
            if (connect(sockfd, (struct sockaddr*)&socketAddr, sizeof(socketAddr)) == -1)
            {
                cout << "socket connect error, return " << -1 << "." << endl;
                return -2;
            }
            cout << "socket connect success, return " << 0 << "." << endl;
            connfd = sockfd;
        }
        // TCP SERVER
        else if (CS_MODE == SERVER_MODE)
        {
            //bind
            if (bind(sockfd, (struct sockaddr*)&socketAddr, sizeof(socketAddr)) == -1)
            {
                cout << "socket bind error, return " << -1 << "." << endl;
                return -2;
            }
            cout << "socket bind success, return " << 0 << "." << endl;

            // listen
            if (listen(sockfd, 5) == -1)
            {
                cout << "socket listen error, return " << -1 << "." << endl;
                return -2;
            }
            cout << "socket listen success, return " << 0 << "." << endl;

            struct sockaddr_in clientaddr;
            memset(&clientaddr, 0, sizeof(clientaddr));
            int clientfd;
            socklen_t len = sizeof(clientaddr);
            if ((clientfd = accept(sockfd, (struct sockaddr*)&clientaddr, &len)) == -1)
            {
                cout << "socket accept error, return " << -1 << "." << endl;
                return -2;
            }
            cout << "socket accept success, client address:"
                << inet_ntoa(clientaddr.sin_addr) << ":" << ntohs(clientaddr.sin_port) << "." << endl;

            connfd = clientfd;
        }

        // SEND/RECV
        if (SR_MODE == SEND_MODE)
        {
            // Send
            string send_buf;
            cout << ">";
            while (cin >> send_buf)
            {
                if (send_buf.compare("exit()") == 0)
                {
                    cout << "exit() detected, confirm to exit!" << endl
                        << "Y/N >";
                    cin >> send_buf;
                    if (send_buf.compare("Y") == 0 || send_buf.compare("y") == 0)
                    {
                        break;
                    }
                    else
                    {
                        cout << ">";
                        continue;
                    }
                }
                if (send(connfd, send_buf.c_str(), strlen(send_buf.c_str()), 0) == -1)
                {
                    cout << "socket send error, return " << -1 << "." << endl;
                    return -3;
                }
                cout << "socket send to server : \"" << send_buf << "\"." << endl;
                cout << ">";
            }
        }
        else if (SR_MODE == RECV_MODE)
        {
            // Recv
            char recv_buf[1024];
            cout << "waiting for recv..." << endl;
            while (1)
            {
                memset(recv_buf, 0, sizeof(recv_buf));

                int ret = recv(connfd, recv_buf, 1024, 0);
                if (ret == -1)
                {
                    cout << "socket recv error, return " << -1 << "." << endl;
                    return -3;
                }
                else if (ret > 0)
                {
                    cout << "socket recv from server : \"" << recv_buf << "\"." << endl;
                }
            }
        }
        // Close
        close(sockfd);
        cout << "socket closed." << endl;
        cout << "goodbye." << endl;
    }


    return 0;
}