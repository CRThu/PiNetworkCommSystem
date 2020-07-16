#include <string>
#include <iostream>
#include <list>
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>

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
#define PROTOCOL_TCP        1
#define PROTOCOL_UDP        2
#define HTTP_GET            3
#define HTTP_POST           4

uint8_t CS_MODE = UNKNOWN_MODE;
uint8_t SR_MODE = UNKNOWN_MODE;
uint8_t PROTOCOL = UNKNOWN_PROTOCOL;
string LocalAddress = "";
string RemoteAddress = "";

list<string> lst_argv;
list<string>::iterator iter;

SocketComm socketComm;

// For Ubuntu Test
// Help argv:   -h
// Debug argv:  -tcp -client -send -remote 192.168.3.8:9999
// Debug argv:  -tcp -client -recv -remote 192.168.3.8:9999
// Debug argv:  -tcp -server -send -local 192.168.3.39:9999
// Debug argv:  -tcp -server -recv -local 192.168.3.39:9999
// Debug argv:  -udp -send -remote 192.168.3.8:9999
// Debug argv:  -udp -recv -local 192.168.3.39:9999 -remote 192.168.3.8:9999

// For RaspberryPi Test
// Help argv:   -h
// Debug argv:  -tcp -client -send -remote 192.168.3.8:9999
// Debug argv:  -tcp -client -recv -remote 192.168.3.8:9999
// Debug argv:  -tcp -server -send -local 192.168.3.34:9999
// Debug argv:  -tcp -server -recv -local 192.168.3.34:9999
// Debug argv:  -udp -send -remote 192.168.3.8:9999
// Debug argv:  -udp -recv -local 192.168.3.34:9999 -remote 192.168.3.8:9999

// TCP CLIENT: SOCKET >>>>>(BIND)>>>>>> CONNECT > SEND > RECV > CLOSE
// TCP SERVER: SOCKET > BIND > LISTEN > ACCEPT  > RECV > SEND > CLOSE
// UDP       : SOCKET >(BIND)>  SENDTO  > RECVFROM > CLOSE
// UDP       : SOCKET > BIND > RECVFROM >  SENDTO  > CLOSE

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
                << "-local          192.168.110.130:9999" << endl
                << "-remote         192.168.3.8:9999" << endl
                << "EXAMPLE:" << endl
                << "HELP :              PiNetworkCommSystem.out -h" << endl
                << "TCP CLIENT SEND:    PiNetworkCommSystem.out -tcp -client -send -remote 192.168.3.8:9999" << endl
                << "TCP SERVER RECV:    PiNetworkCommSystem.out -tcp -server -recv -local 192.168.3.10:9999" << endl
                << "UDP SEND:           PiNetworkCommSystem.out -udp -send -remote 192.168.3.8:9999" << endl
                << "UDP RECV:           PiNetworkCommSystem.out -udp -recv -local 192.168.3.10:9999 -remote 192.168.3.8:9999" << endl;
            return 0;
        }

#ifdef DEBUG_PARA
        cout << *iter;
#endif
        // ARGV
        if (string(*iter).compare("-tcp") == 0)
            PROTOCOL = PROTOCOL_TCP;
        if (string(*iter).compare("-udp") == 0)
            PROTOCOL = PROTOCOL_UDP;
        if (string(*iter).compare("-get") == 0)
            PROTOCOL = HTTP_GET;
        if (string(*iter).compare("-post") == 0)
            PROTOCOL = HTTP_POST;

        if (string(*iter).compare("-client") == 0)
            CS_MODE = CLIENT_MODE;
        if (string(*iter).compare("-server") == 0)
            CS_MODE = SERVER_MODE;

        if (string(*iter).compare("-send") == 0)
            SR_MODE = SEND_MODE;
        if (string(*iter).compare("-recv") == 0)
            SR_MODE = RECV_MODE;

        if (string(*iter).compare("-local") == 0)
        {
            *iter++;
            LocalAddress = *iter;
#ifdef DEBUG_PARA
            cout << " " << LocalAddress;
#endif
        }

        if (string(*iter).compare("-remote") == 0)
        {
            *iter++;
            RemoteAddress = *iter;
#ifdef DEBUG_PARA
            cout << " " << RemoteAddress;
#endif
        }

#ifdef DEBUG_PARA
        cout << endl;
#endif
    }

    if (PROTOCOL == PROTOCOL_TCP || PROTOCOL == PROTOCOL_UDP)
    {
        // SOCKET SETUP
        cout << "--- " << (PROTOCOL == PROTOCOL_TCP ? "TCP" : "UDP")
            << " " << ((CS_MODE == CLIENT_MODE) ? "CLIENT" : "SERVER") << " MODE ---" << endl;
        if (LocalAddress.compare("") != 0)
            cout << "--- Local Address: " << LocalAddress << " ---" << endl;
        if (RemoteAddress.compare("") != 0)
            cout << "--- Remote Address: " << RemoteAddress << " ---" << endl;


        if (PROTOCOL == PROTOCOL_TCP)
        {
            if (CS_MODE == CLIENT_MODE)
            {
                // CREATE SOCKET
                socketComm = SocketComm(PROTOCOL, RemoteAddress);
                socketComm.PrintErrorCodeInfo();

                // TCP CLIENT CONNECT
                socketComm.SocketClientConnect();
                socketComm.PrintErrorCodeInfo();
            }
            else if (CS_MODE == SERVER_MODE)
            {
                // CREATE SOCKET
                socketComm = SocketComm(PROTOCOL, LocalAddress);
                socketComm.PrintErrorCodeInfo();

                // TCP SERVER BIND LISTEN
                socketComm.SocketServerBind();
                socketComm.PrintErrorCodeInfo();

                socketComm.SocketServerListen();
                socketComm.PrintErrorCodeInfo();

                cout << "client address: " << socketComm.SocketServerAccept() << endl;
            }
        }
        else if (PROTOCOL == PROTOCOL_UDP)
        {
            // CREATE SOCKET
            socketComm = SocketComm(PROTOCOL, RemoteAddress);
            socketComm.PrintErrorCodeInfo();

            // UDP BIND
            socketComm.SocketUDPBind(LocalAddress);
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

                if (PROTOCOL == PROTOCOL_TCP)
                {
                    if (CS_MODE == CLIENT_MODE)
                        socketComm.SocketClientSend(send_buf);
                    else if (CS_MODE == SERVER_MODE)
                        socketComm.SocketServerSend(send_buf);
                }
                else if (PROTOCOL == PROTOCOL_UDP)
                {
                    socketComm.SocketUDPSendTo(send_buf);
                }

                socketComm.PrintErrorCodeInfo();

                //cout << "Send Data: " << "\"" << send_buf << "\"." << endl;

                cout << ">";
            }
        }
        else if (SR_MODE == RECV_MODE)
        {
            // For Find Address if not Assign LocalAddress
            if (PROTOCOL == PROTOCOL_UDP && LocalAddress.compare("") == 0)
            {
                socketComm.SocketUDPSendTo("UDP Client Waiting For Recv!");
            }

            // Recv
            cout << "waiting for recv..." << endl;
            while (1)
            {
                string recv_str;

                if (PROTOCOL == PROTOCOL_TCP)
                {
                    if (CS_MODE == CLIENT_MODE)
                        recv_str = socketComm.SocketClientRecv();
                    else if (CS_MODE == SERVER_MODE)
                        recv_str = socketComm.SocketServerRecv();
                }
                else if (PROTOCOL == PROTOCOL_UDP)
                {
                    recv_str = socketComm.SocketUDPRecvFrom();
                }
                socketComm.PrintErrorCodeInfo();
                if (socketComm.GetErrorCode() == SOCKET_RECV_CLOSED)
                    break;

                cout << "Socket Recv from Server : \"" << recv_str << "\"." << endl;
            }
        }

        // Close
        socketComm.SocketClose();
        cout << "socket closed." << endl;
    }
    else if (PROTOCOL == HTTP_GET)
    {
        // TODO
        string get_str = "GET / HTTP/1.1\r\nHost: 220.181.112.244\r\nConnection: Close\r\n\r\n";

        // CREATE SOCKET
        socketComm = SocketComm(PROTOCOL_TCP, "220.181.112.244:80");
        socketComm.PrintErrorCodeInfo();

        // TCP CLIENT CONNECT
        socketComm.SocketClientConnect();
        socketComm.PrintErrorCodeInfo();

        socketComm.SocketClientSend(get_str);
        socketComm.PrintErrorCodeInfo();

        string recv_str;
        recv_str = socketComm.SocketClientRecv();
        socketComm.PrintErrorCodeInfo();

        if (recv_str.compare("") != 0)
        {
            recv_str += socketComm.SocketClientRecv();
            socketComm.PrintErrorCodeInfo();
        }

        cout << "Socket Recv from Server : \"" << recv_str << "\"." << endl;
    }
    else if (PROTOCOL = HTTP_POST)
    {
            int fd;
            double temp = 0;
            char buf[64];
            string temp_str;
        while (1)
        {

            fd = open("/sys/class/thermal/thermal_zone0/temp", O_RDONLY);

            if (fd < 0)
            {
                cout << "failed to open /thermal_zone0/temp" << endl;
                strcpy(buf, "20000");
                //return -1;
            }
            if (read(fd, buf, 64) < 0)
            {
                cout << "failed to read temp" << endl;
                strcpy(buf, "20000");
                //return -1;
            }

            // 转换为浮点数打印  
            temp = atoi(buf) / 1000.0;
            sprintf(buf, "%00.1f", temp);
            temp_str = buf;

            cout << "temp: " << temp_str << endl;

            string post_str = string("POST /devices/610838948/datapoints?type=3 HTTP/1.1\r\n")
                + string("api-key:uUPKZrkPYa3640q3Sy=z5d=GhrA=\r\n")
                + string("Host:api.heclouds.com\r\n")
                + string("Connection:close\r\n")
                + string("Content-Length:13\r\n")
                + string("\r\n")
                + string("{\"TEMP\":" + temp_str + "}");


            // CREATE SOCKET
            socketComm = SocketComm(PROTOCOL_TCP, "183.230.40.33:80");
            socketComm.PrintErrorCodeInfo();

            // TCP CLIENT CONNECT
            socketComm.SocketClientConnect();
            socketComm.PrintErrorCodeInfo();

            socketComm.SocketClientSend(post_str);
            socketComm.PrintErrorCodeInfo();

            string recv_str;
            recv_str = socketComm.SocketClientRecv();
            socketComm.PrintErrorCodeInfo();

            if (recv_str.compare("") != 0)
            {
                recv_str += socketComm.SocketClientRecv();
                socketComm.PrintErrorCodeInfo();
            }

            cout << "Socket Recv from Server : \"" << recv_str << "\"." << endl;

            sleep(3);
        }
    }
    cout << "goodbye." << endl;
    return 0;
}