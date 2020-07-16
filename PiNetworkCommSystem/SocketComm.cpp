#include "SocketComm.h"

using namespace std;

SocketComm::SocketComm()
{
}

SocketComm::SocketComm(uint16_t PROTOCOL)
{
    this->SocketCreate(PROTOCOL);
}

SocketComm::SocketComm(uint16_t PROTOCOL, string address)
{
    this->SocketCreate(PROTOCOL);
    socketAddr = this->SocketAddr(address);
}

void SocketComm::SocketCreate(uint16_t PROTOCOL)
{
    if (PROTOCOL == PROTOCOL_TCP)
        func_return = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    else if (PROTOCOL == PROTOCOL_UDP)
        func_return = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    sockfd = func_return;
    if (func_return < 0)
        err_code = SOCKET_CREATE_ERROR;
    else
        err_code = SOCKET_CREATE_SUCCESS;
}

sockaddr_in SocketComm::SocketAddr(string address)
{
    struct sockaddr_in sockAddr;

    int port_pos = address.find(':');

    string IP = address.substr(0, port_pos);
    uint16_t PORT = (uint16_t)atoi(address.substr(port_pos + 1, address.length()).c_str());

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(PORT);
    sockAddr.sin_addr.s_addr = inet_addr(IP.c_str());

    return sockAddr;
}

void SocketComm::SocketClientConnect()
{
    func_return = connect(sockfd, (struct sockaddr*)&socketAddr, sizeof(socketAddr));
    if (func_return == -1)
        err_code = SOCKET_CONNECT_ERROR;
    else
        err_code = SOCKET_CONNECT_SUCCESS;
}

void SocketComm::SocketServerBind()
{
    func_return = bind(sockfd, (struct sockaddr*)&socketAddr, sizeof(socketAddr));
    if (func_return == -1)
        err_code = SOCKET_BIND_ERROR;
    else
        err_code = SOCKET_BIND_SUCCESS;
}

void SocketComm::SocketServerListen(uint16_t conn_max)
{
    func_return = listen(sockfd, conn_max);
    if (func_return == -1)
        err_code = SOCKET_LISTEN_ERROR;
    else
        err_code = SOCKET_LISTEN_SUCCESS;
}

string SocketComm::SocketServerAccept()
{
    memset(&clientAddr, 0, sizeof(clientAddr));
    socklen_t len = sizeof(clientAddr);

    func_return = accept(sockfd, (struct sockaddr*)&clientAddr, &len);
    clientfd = func_return;
    if (func_return == -1)
        err_code = SOCKET_ACCEPT_ERROR;
    else
        err_code = SOCKET_ACCEPT_SUCCESS;


    char port_c[10];
    sprintf(port_c, "%d", ntohs(clientAddr.sin_port));
    string port_s(port_c);
    return string(inet_ntoa(clientAddr.sin_addr)) + ":" + port_s;
}

void SocketComm::SocketSend(int connfd, string send_str)
{
    func_return = send(connfd, send_str.c_str(), strlen(send_str.c_str()), 0);
    if (func_return == -1)
        err_code = SOCKET_SEND_ERROR;
    else
        err_code = SOCKET_SEND_SUCCESS;
}

void SocketComm::SocketClientSend(string send_str)
{
    this->SocketSend(sockfd, send_str);
}

void SocketComm::SocketServerSend(string send_str)
{
    this->SocketSend(clientfd, send_str);
}

void SocketComm::SocketUDPBind(string localAddress)
{
    sockaddr_in localAddr = this->SocketAddr(localAddress);

    func_return = bind(sockfd, (struct sockaddr*)&localAddr, sizeof(localAddr));
    if (func_return == -1)
        err_code = SOCKET_BIND_ERROR;
    else
        err_code = SOCKET_BIND_SUCCESS;
}

void SocketComm::SocketUDPSendTo(string send_str)
{
    func_return = sendto(sockfd, send_str.c_str(), strlen(send_str.c_str()), 0, (struct sockaddr*)&socketAddr, sizeof(socketAddr));
    if (func_return == -1)
        err_code = SOCKET_SEND_ERROR;
    else
        err_code = SOCKET_SEND_SUCCESS;
}

string SocketComm::SocketRecv(int connfd)
{
    char recv_buf[1000000];
    memset(recv_buf, 0, sizeof(recv_buf));

    func_return = recv(connfd, recv_buf, 1000000, 0);
    if (func_return == -1)
        err_code = SOCKET_RECV_ERROR;
    else if (func_return > 0)
    {
        err_code = SOCKET_RECV_SUCCESS;
        return string(recv_buf);
    }
    else if (func_return == 0)
    {
        err_code = SOCKET_RECV_CLOSED;
        return string("");
    }
}

string SocketComm::SocketClientRecv()
{
    return this->SocketRecv(sockfd);
}

string SocketComm::SocketServerRecv()
{
    return this->SocketRecv(clientfd);
}

string SocketComm::SocketUDPRecvFrom()
{
    char recv_buf[1024];
    memset(recv_buf, 0, sizeof(recv_buf));

    struct sockaddr_in clientaddr;
    memset(&clientaddr, 0, sizeof(clientaddr));
    socklen_t len = sizeof(clientaddr);

    func_return = recvfrom(sockfd, recv_buf, 1024, 0, (struct sockaddr*)&clientaddr, &len);

    if (func_return == -1)
        err_code = SOCKET_RECV_ERROR;
    else if (func_return > 0)
    {
        err_code = SOCKET_RECV_SUCCESS;
        return string(recv_buf);
    }
}

string SocketComm::GetClientAddress()
{
    // TODO clientAddr
    char port_c[10];
    sprintf(port_c, "%d", ntohs(clientAddr.sin_port));
    string port_s(port_c);

    return string(inet_ntoa(clientAddr.sin_addr)) + ":" + port_s;
}

void SocketComm::SocketClose()
{
    close(sockfd);
    if (clientfd != -1)
        close(clientfd);
}

int SocketComm::GetErrorCode()
{
    return err_code;
}

string SocketComm::GetErrorCodeInfo()
{
    string err_info;
    switch (err_code)
    {
    case SOCKET_NO_ERROR:
        return "Socket No Error";
    case SOCKET_CREATE_SUCCESS:
        return "Socket Create Success";
    case SOCKET_CREATE_ERROR:
        return "Socket Create Error";
    case SOCKET_CONNECT_SUCCESS:
        return "Socket Connect Success";
    case SOCKET_CONNECT_ERROR:
        return "Socket Connect Error";
    case SOCKET_BIND_SUCCESS:
        return "Socket Bind Success";
    case SOCKET_BIND_ERROR:
        return "Socket Bind Error";
    case SOCKET_LISTEN_SUCCESS:
        return "Socket Listen Success";
    case SOCKET_LISTEN_ERROR:
        return "Socket Listen Error";
    case SOCKET_ACCEPT_SUCCESS:
        return "Socket Accept Success";
    case SOCKET_ACCEPT_ERROR:
        return "Socket Accept Error";
    case SOCKET_SEND_SUCCESS:
        return "Socket Send Success";
    case SOCKET_SEND_ERROR:
        return "Socket Send Error";
    case SOCKET_RECV_SUCCESS:
        return "Socket Recv Success";
    case SOCKET_RECV_ERROR:
        return "Socket Recv Error";
    case SOCKET_RECV_CLOSED:
        return "Socket Recv Closed";
    default:
        return "Socket Unknown Error";
    }
}

void SocketComm::PrintErrorCodeInfo()
{
    cout << this->GetErrorCodeInfo() << ", return " << func_return << "." << endl;
    if (GetErrorCode() <= 0)
    {
        cout << "Application Exit with Error!" << endl;
        exit(EXIT_FAILURE);
    }
}
