#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <list>

#include "SocketComm.h"

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

SocketComm socketComm;

// Help argv: PiNetworkCommSystem.out -h
// Debug argv: -tcp -client -send -ip 192.168.3.8 -port 9999
// Debug argv: -tcp -server -recv -ip 192.168.110.130 -port 9999
// Debug argv: -udp -client -recv -ip 192.168.3.8 -port 9999

// TCP SERVER: SOCKET > BIND > LISTEN > ACCEPT  > RECV > SEND > CLOSE
// TCP CLIENT: SOCKET >>>>>(BIND)>>>>>> CONNECT > SEND > RECV > CLOSE
// UDP SERVER: SOCKET > BIND > RECVFROM > SENDTO   > CLOSE
// UDP CLIENT: SOCKET >>>>>>>> SENDTO   > RECVFROM > CLOSE
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
                //<< "UDP CLIENT SEND:    PiNetworkCommSystem.out -udp -client -recv -ip 192.168.3.8 -port 9999" << endl
                //<< "UDP SERVER RECV:    PiNetworkCommSystem.out -udp -server -send -ip 192.168.110.130 -port 9999" << endl;
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
            PORT = (uint16_t)atoi(string(*iter).c_str());
        }
#ifdef DEBUG_PARA
        cout << endl;
#endif
    }

    if (PROTOCOL == TCP_PROTOCOL)
    {
        cout << "--- TCP " << ((CS_MODE == CLIENT_MODE) ? "CLIENT" : "SERVER") << " MODE ---" << endl;
        cout << "--- " << IP << ":" << PORT << " ---" << endl;

        // CREATE SOCKET
        socketComm = SocketComm(PROTOCOL, IP, PORT);
        socketComm.PrintErrorCodeInfo();

        // TCP CLIENT CONNECT
        if (CS_MODE == CLIENT_MODE)
        {
            socketComm.SocketClientConnect();
            socketComm.PrintErrorCodeInfo();
        }
        // TCP SERVER BIND LISTEN
        else if (CS_MODE == SERVER_MODE)
        {
            socketComm.SocketServerBind();
            socketComm.PrintErrorCodeInfo();

            socketComm.SocketServerListen();
            socketComm.PrintErrorCodeInfo();

            cout << "client address: " << socketComm.SocketServerAccept() << endl;
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

                if (CS_MODE == CLIENT_MODE)
                    socketComm.SocketClientSend(send_buf);
                else if (CS_MODE == SERVER_MODE)
                    socketComm.SocketServerSend(send_buf);
                socketComm.PrintErrorCodeInfo();

                //cout << "Send Data: " << "\"" << send_buf << "\"." << endl;

                cout << ">";
            }
        }
        else if (SR_MODE == RECV_MODE)
        {
            // Recv
            cout << "waiting for recv..." << endl;
            while (1)
            {
                string recv_str;
                if (CS_MODE == CLIENT_MODE)
                    recv_str = socketComm.SocketClientRecv();
                else if (CS_MODE == SERVER_MODE)
                    recv_str = socketComm.SocketServerRecv();

                socketComm.PrintErrorCodeInfo();
                if (socketComm.err_code == SOCKET_RECV_CLOSED)
                    break;

                cout << "Socket Recv from Server : \"" << recv_str << "\"." << endl;
            }
        }
        // Close
        socketComm.SocketClose();
        cout << "socket closed." << endl;
    }

    cout << "goodbye." << endl;
    return 0;
}