#pragma once
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <list>

using namespace std;

#define PROTOCOL_TCP            1
#define PROTOCOL_UDP            2

#define SOCKET_NO_ERROR         0
#define SOCKET_CREATE_SUCCESS   1
#define SOCKET_CREATE_ERROR     -1
#define SOCKET_CONNECT_SUCCESS  2
#define SOCKET_CONNECT_ERROR    -2
#define SOCKET_BIND_SUCCESS     3
#define SOCKET_BIND_ERROR       -3
#define SOCKET_LISTEN_SUCCESS   4
#define SOCKET_LISTEN_ERROR     -4
#define SOCKET_ACCEPT_SUCCESS   5
#define SOCKET_ACCEPT_ERROR     -5
#define SOCKET_SEND_SUCCESS     6
#define SOCKET_SEND_ERROR       -6
#define SOCKET_RECV_SUCCESS     7
#define SOCKET_RECV_ERROR       -7
#define SOCKET_RECV_CLOSED      8


class SocketComm
{
private:
    int func_return;
    int sockfd = -1;
    int clientfd = -1;
    struct sockaddr_in socketAddr;
    struct sockaddr_in clientAddr;
    int err_code = SOCKET_NO_ERROR;
public:
    SocketComm();
    SocketComm(uint16_t PROTOCOL);
    SocketComm(uint16_t PROTOCOL, string address);

    void SocketCreate(uint16_t PROTOCOL);
    sockaddr_in SocketAddr(string address);
    void SocketClose();

    void SocketSend(int connfd, string send_str);
    string SocketRecv(int connfd);

    // TCP Client
    void SocketClientConnect();
    void SocketClientSend(string send_str);
    string SocketClientRecv();

    // TCP Server
    void SocketServerBind();
    void SocketServerListen(uint16_t conn_max = 5);
    string SocketServerAccept();
    void SocketServerSend(string send_str);
    string SocketServerRecv();

    // UDP
    void SocketUDPBind(string localAddress);
    void SocketUDPSendTo(string send_str);
    string SocketUDPRecvFrom();

    // Address
    string GetClientAddress();

    // Error
    int GetErrorCode();
    string GetErrorCodeInfo();
    void PrintErrorCodeInfo();
};