/* @file           Socket.h
 * @copyright      HangZhou Hikvision System Technology Co., Ltd. All Right Reserved.
 * @brief          Machine Vision Component Socket class
 *
 * @author         zhenglinjun
 * @date           2015-08-01
 *
 * @note
 *
 * @warning
 */


#ifndef __SOCKET_H__
#define __SOCKET_H__

#pragma once

#if defined __WIN32 || defined __WIN64 || defined WIN32 || defined WIN64
#define WINDOWS
#endif

#ifdef WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#endif

#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <vector>
#include <fstream>

#define SOCKET_TIMEOUT            (0)
#ifndef WINDOWS
#define SOCKET_ERROR              (-1)
#endif
#define SOCKET_CLOSE              (-2)

#ifndef SOCKET_MAX_BUFFER_BYTES
#define SOCKET_MAX_BUFFER_BYTES   (64 << 10) // 64KB
#endif

namespace MVComponent
{
typedef int SocketId;
typedef std::string Ip;
typedef unsigned int Port;
#ifdef WINDOWS
typedef int socklen_t;
#endif

class SocketException : public std::exception
{
  private:
    std::string         _error;

  public:
    SocketException(const std::string&);
    ~SocketException() throw();

    virtual const char* what() const throw();
    unsigned long GetError(std::string& err_msg);
    friend std::ostream& operator<< (std::ostream &out, SocketException &e);
};

struct Address : protected sockaddr_in
{
  private:
    void _address(Ip, Port);

  public:
    Address();
    Address(Port);
    Address(Ip, Port);
    Address(struct sockaddr_in);
    Address(const Address&);

    inline Ip GetAddressIp(void) const;
    inline Ip SetAddressIp(Ip);

    inline Port GetAddressPort(void) const;
    inline Port SetAddressPort(Port);

    friend std::ostream& operator<< (std::ostream&, Address&);
};

template <typename DataType>
struct Datagram
{
  public:
    Address             address;
    DataType            data;
    unsigned int        received_bytes;
    unsigned int        received_elements;

    Datagram();

    template <typename T> void operator= (const Datagram<T>&);
#if __cplusplus >= 201103L
    template <typename T> void operator= (Datagram<T>&&);
#endif
};

class CommonSocket
{
  private:
#ifdef WINDOWS
    static unsigned int _num_sockets;
#endif
    static void _socket(void);

  protected:
    SocketId            _socket_id;
    int                 _socket_type;
    bool                _opened;
    bool                _binded;

  public:
    CommonSocket(void);
    CommonSocket(int);

    ~CommonSocket(void);

    SocketId GetSocketId(void);

    void Open(void);
    void Close(void);

    virtual void BindOnPort(Port);
    int SetOption(int, int, const void*, socklen_t);
    int GetOption(int, int, void*, socklen_t*);

    // Set option common
    int SetBroadcast(bool isboardcast);
    int SetNonblock(bool isnonblock);
    int SetTtl(int ttl);
    int GetTtl(int& ttl);
    int SetMulticastTtl(int ttl);
    int SetTos(int tos);
    int GetTos(int& tos);
    int SetTimeout(int sendtimeout, int recvtimeout);
    int GetTimeout(int& sendtimeout, int& recvtimeout);
    int SetBuffsize(int sendbuffsize, int recvbuffsize);
    int GetBuffsize(int& sendbuffsize, int& recvbuffsize);
    int SetDontfragment(bool isdf);
};

class UDP : public CommonSocket
{
  public:
    UDP(void);
    UDP(const UDP&);

  public:
    Ip GetAddressIp(void);
    Port GetAddressPort(void);
    Address GetAddress(void);

    int Send(Ip ip, Port port, const char* data, size_t len);
    int Send(const Address& address, const char* data, size_t len);

    int Receive(Address& address, char* data, unsigned int& len);
    int ReceiveTimeout(unsigned int ms, Address& address, char* data, unsigned int& len);

  public:
    template <typename T> inline int Receive(Address&, T*, size_t, unsigned int&);
    template <typename T> inline int ReceiveTimeout(unsigned int, Address&, T*, size_t, unsigned int&);

  public:
    template <typename T> int Send(Ip, Port, const T*, size_t);
    template <typename T> int Send(const Address&, const T*, size_t);
    template <typename T> int Send(Ip, Port, T);
    template <typename T> int Send(const Address&, T);

    template <typename T> Datagram<T*> Receive(T*, size_t len = SOCKET_MAX_BUFFER_BYTES / sizeof(T));
    template <typename T, size_t N> Datagram<T[N]> Receive(size_t len = N);
    template <typename T> Datagram<T> Receive(void);

    template <typename T> Datagram<T*> ReceiveTimeout(unsigned int, T*, size_t len = SOCKET_MAX_BUFFER_BYTES / sizeof(T));
    template <typename T, size_t N> Datagram<T[N]> ReceiveTimeout(unsigned int, size_t len = N);
    template <typename T> Datagram<T> ReceiveTimeout(unsigned int);
};

class TCP : public CommonSocket
{
  private:
    Address                               _address;
    bool                                  _ispeer;
    std::vector<std::pair<int, Address> > _clients;
#ifdef WINDOWS
    HANDLE                                _clients_mutex;
#else
    pthread_mutex_t                       _clients_mutex;
#endif
    class LockGuard;        // Nested Classes

  public:
    TCP(void);
    TCP(const TCP&);
    ~TCP(void);

    // Set option
    int SetReuseaddr(bool isreuseaddr);
    int SetLingeroff();
    int SetLingeron(short timeoutsec);
    int SetNodelay(bool isnodelay);

    void Close(void);

    Ip GetAddressIp(void);
    Port GetAddressPort(void);
    Address GetAddress(void);

    void ListenOnPort(Port, unsigned int listeners = 1);
    void ConnectTo(Address);

    TCP AcceptClient(void);

  public:
    int Send(const char* buffer, size_t len);
    int Receive(char* buffer, size_t len);
    int SendTimeout(unsigned int ms, const char* buffer, size_t len);
    int ReceiveTimeout(unsigned int ms, char* buffer, size_t len);

    template <typename T> int Send(const T*, size_t);
    template <typename T> int Receive(T*, size_t);

    template <typename T> int SendTimeout(unsigned int, const T*, size_t);
    template <typename T> int ReceiveTimeout(unsigned int, T*, size_t);

    int AcceptAll(TCP&) throw();
    template <typename T> int ReceiveAll(TCP&, unsigned int, T*, size_t) throw();
};
}


#endif  // __SOCKET_H__
