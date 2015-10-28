#include "Socket.h"

// Address.cpp
namespace MVComponent
{
void Address::_address(Ip ip, Port port)
{
    this->sin_family = AF_INET;
    this->SetAddressIp(ip);
    this->SetAddressPort(port);
}

Address::Address()
{
    _address("0.0.0.0", 0);
}

Address::Address(Port port)
{
    _address("0.0.0.0", port);
}

Address::Address(Ip ip, Port port)
{
    _address(ip, port);
}

Address::Address(struct sockaddr_in address)
{
    _address(inet_ntoa(address.sin_addr), address.sin_port);
}

Address::Address(const Address &address)
{
    this->sin_family = address.sin_family;
    this->sin_addr = address.sin_addr;
    this->sin_port = address.sin_port;
}

inline Ip Address::GetAddressIp(void) const
{
    return inet_ntoa(this->sin_addr);
}

inline Ip Address::SetAddressIp(Ip ip)
{
#ifdef WINDOWS
    unsigned long address = inet_addr(ip.c_str());

    if (address == INADDR_NONE)
    {
        std::stringstream error;
        error << "[ip] with [ip=" << ip << "] Invalid ip address provided";
        throw SocketException(error.str());
    }
    else
    {
        this->sin_addr.S_un.S_addr = address;
    }
#else
    if (inet_aton(ip.c_str(), &this->sin_addr) == 0)
    {
        std::stringstream error;
        error << "[ip] with [ip=" << ip << "] Invalid ip address provided";
        throw SocketException(error.str());
    }
#endif
    return this->GetAddressIp();
}

inline Port Address::GetAddressPort(void) const
{
    return ntohs(this->sin_port);
}

inline Port Address::SetAddressPort(Port port)
{
    this->sin_port = htons(port);
    return this->GetAddressPort();
}

std::ostream& operator<< (std::ostream &out, Address &address)
{
    out << address.GetAddressIp() << ":" << address.GetAddressPort();
    return out;
}
}

// CommonSocket
namespace MVComponent
{
#ifdef WINDOWS
unsigned int CommonSocket::_num_sockets = 0;
#endif

void CommonSocket::_socket(void)
{
#ifdef WINDOWS
    _num_sockets++;
    if (_num_sockets == 1)
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            throw SocketException("[constructor] Cannot initialize socket environment");
    }
#endif
}

CommonSocket::CommonSocket(void)
{
    CommonSocket::_socket();
}

CommonSocket::CommonSocket(int socket_type)
{
    CommonSocket::_socket();

    this->_socket_type = socket_type;
    this->_opened = false;
    this->_binded = false;
}

CommonSocket::~CommonSocket(void)
{
#ifdef WINDOWS
    this->_num_sockets--;
    if (this->_num_sockets == 0)
        WSACleanup();
#endif
}

SocketId CommonSocket::GetSocketId(void)
{
    return this->_socket_id;
}

void CommonSocket::Open(void)
{
    if (!this->_opened)
    {
        if ((this->_socket_id = socket(AF_INET, this->_socket_type, 0)) == -1)
            throw SocketException("[open] Cannot create socket");
        this->_opened = true;
        this->_binded = false;
    }
}

void CommonSocket::Close(void)
{
    if (this->_opened)
    {
#ifdef WINDOWS
        closesocket(this->_socket_id);
#else
        shutdown(this->_socket_id, SHUT_RDWR);
        ::close(this->_socket_id);
#endif
    }

    this->_opened = false;
    this->_binded = false;
}

void CommonSocket::BindOnPort(Port port)
{
    if (this->_binded)
        throw SocketException("[bind_on_port] Socket already binded to a port, close the socket before to re-bind");

    if (!this->_opened)
        this->Open();

    Address address(port);

    if (bind(this->_socket_id, (struct sockaddr*)&address, sizeof(struct sockaddr)) == -1)
    {
        std::stringstream error;
        error << "[bind_on_port] with [port=" << port << "] Cannot bind socket";
        throw SocketException(error.str());
    }

    this->_binded = true;
}

int CommonSocket::SetOption(int level, int optname, const void* optval, socklen_t optlen)
{
    int ret = 0;

    if ((ret = ::setsockopt(_socket_id, level, optname, (const char*)optval, optlen)) == SOCKET_ERROR)
    {
        std::stringstream error;
        error << "[set_option] error";
        throw SocketException(error.str());
    }

    return ret;
}

int CommonSocket::GetOption(int level, int optname, void* optval, socklen_t* optlen)
{
    int ret = 0;
    if ((ret = getsockopt(_socket_id, level, optname, (char*)optval, optlen)) == SOCKET_ERROR)
    {
        std::stringstream error;
        error << "[get_option] error";
        throw SocketException(error.str());
    }
    return ret;
}

int CommonSocket::SetBroadcast(bool isbroadcast)
{
    int ret = 0;
    if ((ret = ::setsockopt(_socket_id, SOL_SOCKET, SO_BROADCAST, (const char*)&isbroadcast, sizeof(bool))) == SOCKET_ERROR)
    {
        std::stringstream error;
        error << "[set_broadcast] error";
        throw SocketException(error.str());
    }
    return ret;
}

int CommonSocket::SetNonblock(bool isnonblock)
{
    int ret = 0;
#ifdef WINDOWS
    unsigned long arg;
    arg = (isnonblock) ? 1 : 0;
    if ((ret = ioctlsocket(_socket_id, FIONBIO, (unsigned long*)&arg)) == SOCKET_ERROR)
    {
        std::stringstream error;
        error << "[set_nonblock] error";
        throw SocketException(error.str());
    }
#else
    int flags = fcntl(_socket_id, F_GETFL, 0);
    long arg = (isnonblock) ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
    if ((ret = fcntl(_socket_id, F_SETFL, arg)) == SOCKET_ERROR)
    {
        std::stringstream error;
        error << "[set_nonblock] error";
        throw SocketException(error.str());
    }
#endif
    return ret;
}

int CommonSocket::SetTtl(int ttl)
{
    int ret = 0;
    if ((ret = setsockopt(_socket_id, IPPROTO_IP, IP_TTL, (char*)&ttl, sizeof(int))) == SOCKET_ERROR)
    {
        std::stringstream error;
        error << "[set_ttl] error";
        throw SocketException(error.str());
    }
    return ret;
}

int CommonSocket::GetTtl(int& ttl)
{
    int ret = 0;
    socklen_t len;
    if ((ret = getsockopt(_socket_id, IPPROTO_IP, IP_TTL, (char*)&ttl, &len)) == SOCKET_ERROR)
    {
        std::stringstream error;
        error << "[get_ttl] error";
        throw SocketException(error.str());
    }
    return ret;
}

int CommonSocket::SetMulticastTtl(int ttl)
{
    int ret = 0;
    if ((ret = setsockopt(_socket_id, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&ttl, sizeof(int))) == SOCKET_ERROR)
    {
        std::stringstream error;
        error << "[set_multicast_ttl] error";
        throw SocketException(error.str());
    }
    return ret;
}

int CommonSocket::SetTos(int tos)
{
    int ret = 0;
    if ((ret = setsockopt(_socket_id, IPPROTO_IP, IP_TOS, (char*)&tos, sizeof(int))) == SOCKET_ERROR)
    {
        std::stringstream error;
        error << "[set_tos] error";
        throw SocketException(error.str());
    }
    return ret;
}

int CommonSocket::GetTos(int& tos)
{
    int ret = 0;
    socklen_t len;
    if ((ret = getsockopt(_socket_id, IPPROTO_IP, IP_TOS, (char*)&tos, &len)) == SOCKET_ERROR)
    {
        std::stringstream error;
        error << "[get_tos] error";
        throw SocketException(error.str());
    }
    return ret;
}

int CommonSocket::SetTimeout(int sendtimeout, int recvtimeout)
{
    int ret = 0;
    if (sendtimeout >= 0)
    {
        if ((ret = setsockopt(_socket_id, SOL_SOCKET, SO_SNDTIMEO, (char*)&sendtimeout, sizeof(int))) == SOCKET_ERROR)
        {
            std::stringstream error;
            error << "[set_timeout] error";
            throw SocketException(error.str());
        }
    }
    if (recvtimeout >= 0)
    {
        if ((ret = setsockopt(_socket_id, SOL_SOCKET, SO_SNDTIMEO, (char*)&recvtimeout, sizeof(int))) == SOCKET_ERROR)
        {
            std::stringstream error;
            error << "[set_timeout] error";
            throw SocketException(error.str());
        }
    }
    return ret;
}

int CommonSocket::GetTimeout(int& sendtimeout, int& recvtimeout)
{
    int ret = 0;
    socklen_t len;
    if ((ret = getsockopt(_socket_id, SOL_SOCKET, SO_SNDTIMEO, (char*)&sendtimeout, &len)) == SOCKET_ERROR)
    {
        std::stringstream error;
        error << "[get_timeout] error";
        throw SocketException(error.str());
    }
    if ((ret = getsockopt(_socket_id, SOL_SOCKET, SO_SNDTIMEO, (char*)&recvtimeout, &len)) == SOCKET_ERROR)
    {
        std::stringstream error;
        error << "[get_timeout] error";
        throw SocketException(error.str());
    }
    return ret;
}

int CommonSocket::SetBuffsize(int sendbuffsize, int recvbuffsize)
{
    int ret = 0;
    if (sendbuffsize > 0)
    {
        if ((ret = setsockopt(_socket_id, SOL_SOCKET, SO_SNDBUF, (char*)&sendbuffsize, sizeof(int))) == SOCKET_ERROR)
        {
            std::stringstream error;
            error << "[set_buffsize] error";
            throw SocketException(error.str());
        }
    }
    if (recvbuffsize > 0)
    {
        if ((ret = setsockopt(_socket_id, SOL_SOCKET, SO_RCVBUF, (char*)&recvbuffsize, sizeof(int))) == SOCKET_ERROR)
        {
            std::stringstream error;
            error << "[set_buffsize] error";
            throw SocketException(error.str());
        }
    }

    return ret;
}

int CommonSocket::GetBuffsize(int& sendbuffsize, int& recvbuffsize)
{
    int ret = 0;
    socklen_t len;
    if ((ret = getsockopt(_socket_id, SOL_SOCKET, SO_SNDBUF, (char*)&sendbuffsize, &len)) == SOCKET_ERROR)
    {
        std::stringstream error;
        error << "[get_buffsize] error";
        throw SocketException(error.str());
    }
    if ((ret = getsockopt(_socket_id, SOL_SOCKET, SO_RCVBUF, (char*)&recvbuffsize, &len)) == SOCKET_ERROR)
    {
        std::stringstream error;
        error << "[get_buffsize] error";
        throw SocketException(error.str());
    }
    return ret;
}

int CommonSocket::SetDontfragment(bool isdf)
{
    int ret = 0;
    if ((ret = setsockopt(_socket_id, IPPROTO_IP, IP_DONTFRAGMENT, (const char*)&isdf, sizeof(isdf))) == SOCKET_ERROR)
    {
        std::stringstream error;
        error << "[set_dontfragment] error";
        throw SocketException(error.str());
    }
    return ret;
}
}

// Datagram
namespace MVComponent
{
template <typename DataType>
Datagram<DataType>::Datagram() : received_bytes(0), received_elements(0)
{
}

template <typename DataType>
template <typename T>
void Datagram<DataType>::operator= (const Datagram<T> &datagram)
{
    this->address = datagram.address;
    this->data = datagram.data;
}

#if __cplusplus >= 201103L
template <typename DataType>
template <typename T>
void Datagram<DataType>::operator= (Datagram<T>&& datagram)
{
    this->address = datagram.address;
    this->data = std::move(datagram.data);
}
#endif
}

// SocketException
namespace MVComponent
{
SocketException::SocketException(const std::string &message)
{
    this->_error = message;
}

SocketException::~SocketException() throw()
{
}

const char* SocketException::what() const throw()
{
    return this->_error.c_str();
}

unsigned long SocketException::GetError(std::string& err_msg)
{
#ifdef WINDOWS
    unsigned long dw = WSAGetLastError();
    char sz[128];
    LPVOID lpmsg;
    size_t len = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                NULL, dw, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                                // MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                (LPSTR)&lpmsg, 0, NULL);
    sprintf_s(sz, len+1, "%s", lpmsg);
    if (len > 2)
        sz[len - 2] = '\0';
    else
        sz[len] = '\0';
    LocalFree(lpmsg);
    err_msg = sz;
#else
    unsigned long dw = errno;
    err_msg = strerror(errno);
#endif

    return dw;
}

std::ostream& operator<< (std::ostream &out, SocketException &e)
{
    std::string err_msg;
    unsigned long dw = e.GetError(err_msg);
    out << e.what() << " (" << dw << ", " << err_msg << ")";
    return out;
}
}

// TCP
namespace MVComponent
{
class TCP::LockGuard
{
  private:
#ifdef WINDOWS
    HANDLE&          _mutex;
#else
    pthread_mutex_t& _mutex;
#endif

  public:
#ifdef WINDOWS
    LockGuard(HANDLE& m) : _mutex(m)
    {
        if (WaitForSingleObject(this->_mutex, INFINITE) != WAIT_OBJECT_0)
            throw std::exception();
    }
#else
    LockGuard(pthread_mutex_t& m) : _mutex(m)
    {
        if (pthread_mutex_lock(&this->_mutex))
            throw std::exception();
    }
#endif

#ifdef WINDOWS
    virtual ~LockGuard()
    {
        if (!ReleaseMutex(this->_mutex))
            throw std::exception();
    }
#else
    virtual ~LockGuard()
    {
        if (pthread_mutex_unlock(&this->_mutex))
            throw std::exception();
    }
#endif
};

TCP::TCP(void) : CommonSocket(SOCK_STREAM)
{
    this->_ispeer = false;
    this->_clients = std::vector<std::pair<int, Address> >();
#ifdef WINDOWS
    this->_clients_mutex = CreateMutex(NULL, false, NULL);
#else
    pthread_mutex_init(&this->_clients_mutex, NULL);
#endif
}

TCP::TCP(const TCP &tcp) : CommonSocket()
{
    this->_socket_id = tcp._socket_id;
    this->_opened = tcp._opened;
    this->_binded = tcp._binded;
    this->_socket_type = tcp._socket_type;
    this->_address = tcp._address;
    this->_ispeer = tcp._ispeer;
    this->_clients = tcp._clients;
#ifdef WINDOWS
    this->_clients_mutex = CreateMutex(NULL, false, NULL);
#else
    pthread_mutex_init(&this->_clients_mutex, NULL);
#endif
}

TCP::~TCP(void)
{
#ifdef WINDOWS
    CloseHandle(this->_clients_mutex);
#else
    pthread_mutex_destroy(&this->_clients_mutex);
#endif
}

int TCP::SetLingeroff()
{
    int ret = 0;
    linger socklinger;
    socklinger.l_onoff  = 0;
    socklinger.l_linger = 0;
    if ((ret = setsockopt(_socket_id, SOL_SOCKET, SO_LINGER, (char*)&socklinger,sizeof(linger))) == SOCKET_ERROR)
    {
        std::stringstream error;
        error << "[set_lingeroff] error";
        throw SocketException(error.str());
    }
    return ret;
}

int TCP::SetLingeron(short timeoutsec)
{
    int ret = 0;
    linger socklinger;
    socklinger.l_onoff  = 1;
    socklinger.l_linger = timeoutsec;
    if ((ret = setsockopt(_socket_id, SOL_SOCKET, SO_LINGER, (char*)&socklinger,sizeof(linger))) == SOCKET_ERROR)
    {
        std::stringstream error;
        error << "[set_lingeron] error";
        throw SocketException(error.str());
    }
    return ret;
}

int TCP::SetReuseaddr(bool isreuseaddr)
{
    int ret = 0;
    if ((ret = setsockopt(_socket_id, SOL_SOCKET, SO_REUSEADDR, (char*)&isreuseaddr, sizeof(bool))) == SOCKET_ERROR)
    {
        std::stringstream error;
        error << "[set_reuseaddr] error";
        throw SocketException(error.str());
    }
    return ret;
}

int TCP::SetNodelay(bool isnodelay)
{
    int ret = 0;
    if ((ret = setsockopt(_socket_id, IPPROTO_TCP, TCP_NODELAY, (const char*)&isnodelay, sizeof(bool))) == SOCKET_ERROR)
    {
        std::stringstream error;
        error << "[set_nodelay] error";
        throw SocketException(error.str());
    }
    return ret;
}

void TCP::Close(void)
{
    if (this->_opened)
    {
#ifdef WINDOWS
        closesocket(this->_socket_id);
#else
        shutdown(this->_socket_id, SHUT_RDWR);
        ::close(this->_socket_id);
#endif
        for (unsigned int i = 0; i < this->_clients.size(); ++i)
        {
#ifdef WINDOWS
            closesocket(this->_clients[i].first);
#else
            shutdown(this->_clients[i].first, SHUT_RDWR);
            ::close(this->_clients[i].first);
#endif
        }
        this->_clients.clear();
    }

    this->_opened = false;
    this->_binded = false;
}

Ip TCP::GetAddressIp(void)
{
    this->GetAddress();
    return this->_address.GetAddressIp();
}

Port TCP::GetAddressPort(void)
{
    this->GetAddress();
    return this->_address.GetAddressPort();
}

Address TCP::GetAddress(void)
{
    socklen_t len = sizeof(this->_address);
    if (!this->_ispeer) {
        if (getsockname(this->_socket_id, (struct sockaddr*)&this->_address, &len) < 0)
            throw SocketException("[get_address] getsockname() error");
    }
    return Address(this->_address);
}

void TCP::ListenOnPort(Port port, unsigned int listeners)
{
    if (this->_binded)
    {
        if (this->_address.GetAddressPort() != port)
            throw SocketException("[listen_on_port] Socket listen to a port different from binded");
    }
    else
        CommonSocket::BindOnPort(port);

    if (listen(this->_socket_id, listeners) != 0)
    {
        std::stringstream error;
        error << "[listen_on_port] with [port=" << port << "] [listeners=" << listeners << "] Cannot listen";
        throw SocketException(error.str());
    }

    this->_clients.resize(FD_SETSIZE);
    this->_clients.clear();
}

void TCP::ConnectTo(Address address)
{
    if (!this->_opened) this->Open();

    if (connect(this->_socket_id, (struct sockaddr*)&address, sizeof(struct sockaddr_in)) < 0)
    {
        std::stringstream error;
        error << "[connect_to] with [address=" << address << "] Cannot connect to the specified address";
        throw SocketException(error.str());
    }

    this->_binded = true;
}

TCP TCP::AcceptClient(void)
{
    TCP ret;
    socklen_t len = sizeof(struct sockaddr_in);

    ret._socket_id = accept(this->_socket_id, (struct sockaddr*)&ret._address, (socklen_t *)&len);
    ret._opened = true;
    ret._binded = true;
    ret._ispeer = true;

    return ret;
}

int TCP::Send(const char* buffer, size_t len)
{
    return this->Send<char>(buffer, len);
}

int TCP::Receive(char* buffer, size_t len)
{
    return this->Receive<char>(buffer, len);
}

int TCP::SendTimeout(unsigned int ms, const char* buffer, size_t len)
{
    return this->SendTimeout<char>(ms, buffer, len);
}

int TCP::ReceiveTimeout(unsigned int ms, char* buffer, size_t len)
{
    return this->ReceiveTimeout<char>(ms, buffer, len);
}

template <typename T>
int TCP::Send(const T* buffer, size_t len)
{
    if (!this->_binded) throw SocketException("[send] Socket not binded");
    if (!this->_opened) throw SocketException("[send] Socket not opened");

    len *= sizeof(T);
    if (len > SOCKET_MAX_BUFFER_BYTES)
    {
        std::stringstream error;
        error << "[send] [len=" << len << "] Data length higher then max buffer len ("
              << SOCKET_MAX_BUFFER_BYTES << ")";
        throw SocketException(error.str());
    }

    int ret;
    if ((ret = ::send(this->_socket_id, (const char*)buffer, len, 0)) == SOCKET_ERROR)
        throw SocketException("[send] Cannot send");
    return ret;
}

template <typename T>
int TCP::Receive(T* buffer, size_t len)
{
    if (!this->_binded) throw SocketException("[receive] Socket not binded");
    if (!this->_opened) throw SocketException("[receive] Socket not opened");

    len *= sizeof(T);
    if (len > SOCKET_MAX_BUFFER_BYTES)
    {
        std::stringstream error;
        error << "[receive] [len=" << len << "] Data length higher then max buffer len ("
              << SOCKET_MAX_BUFFER_BYTES << ")";
        throw SocketException(error.str());
    }

    int ret;
    ret = ::recv(this->_socket_id, (char *)buffer, len, 0);
    if (ret == 0 || (ret == SOCKET_ERROR
#ifdef WINDOWS
                     && WSAGetLastError() == WSAECONNRESET
#else
                     && errno == ECONNRESET
#endif
                     ))
    {
        ret = SOCKET_CLOSE;
    }

    return ret;
}

template <typename T>
int TCP::SendTimeout(unsigned int ms, const T* buffer, size_t len)
{
    if (!this->_binded) throw SocketException("[send_timeout] Socket not binded");
    if (!this->_opened) throw SocketException("[send_timeout] Socket not opened");

    len *= sizeof(T);
    if (len > SOCKET_MAX_BUFFER_BYTES)
    {
        std::stringstream error;
        error << "[send_timeout] [len=" << len << "] Data length higher then max buffer len ("
              << SOCKET_MAX_BUFFER_BYTES << ")";
        throw SocketException(error.str());
    }

    int ret = SOCKET_ERROR;
    int ready;
    fd_set wset;
    struct timeval timeout = {(time_t)(ms/1000), ms%1000};

    FD_ZERO(&wset);
    FD_SET(this->_socket_id, &wset);

    ready = ::select(this->_socket_id+1, NULL, &wset, NULL, &timeout);
    // error
    if (ready == SOCKET_ERROR || ready < 0)
    {
        throw SocketException("[send_timeout] select() return SOCKET_ERROR");
    }

    // timeout
    if (ready == 0)
        return 0;

    // something to write
    if (FD_ISSET(this->_socket_id, &wset))
    {
        if ((ret = ::send(this->_socket_id, (const char*)buffer, len, 0)) == SOCKET_ERROR)
            throw SocketException("[send_timeout] Cannot send");
    }

    return ret;
}

template <typename T>
int TCP::ReceiveTimeout(unsigned int ms, T* buffer, size_t len)
{
    if (!this->_binded) throw SocketException("[receive_timeout] Socket not binded");
    if (!this->_opened) throw SocketException("[receive_timeout] Socket not opened");

    len *= sizeof(T);
    if (len > SOCKET_MAX_BUFFER_BYTES)
    {
        std::stringstream error;
        error << "[receive_timeout] [len=" << len << "] Data length higher then max buffer len ("
              << SOCKET_MAX_BUFFER_BYTES << ")";
        throw SocketException(error.str());
    }

    int ret = SOCKET_ERROR;
    int ready;
    fd_set rset;
    struct timeval timeout = {(time_t)(ms/1000), ms%1000};

    FD_ZERO(&rset);
    FD_SET(this->_socket_id, &rset);

    ready = ::select(this->_socket_id+1, &rset, NULL, NULL, &timeout);
    // error
    if (ready == SOCKET_ERROR || ready < 0)
    {
        throw SocketException("[receive_timeout] select() return SOCKET_ERROR");
    }

    // timeout
    if (ready == 0)
        return SOCKET_TIMEOUT;

    // something to read
    if (FD_ISSET(this->_socket_id, &rset))
    {
        ret = ::recv(this->_socket_id, (char *)buffer, len, 0);
        if (ret == 0 || (ret == SOCKET_ERROR
#ifdef WINDOWS
                         && WSAGetLastError() == WSAECONNRESET
#else
                         && errno == ECONNRESET
#endif
                         ))
        {
            ret = SOCKET_CLOSE;
        }
    }

    return ret;
}

int TCP::AcceptAll(TCP& client) throw()
{
    socklen_t len = sizeof(struct sockaddr_in);

    if (this->_clients.size() >= FD_SETSIZE)
        return SOCKET_ERROR;

    client._socket_id = accept(this->_socket_id, (struct sockaddr*)&client._address, (socklen_t *)&len);

    {
        LockGuard lock(this->_clients_mutex);
        this->_clients.push_back(std::make_pair(client._socket_id, client._address));
    }
    client._opened = true;
    client._binded = true;
    client._ispeer = true;

#ifdef _DEBUG
    std::stringstream ss;
    ss << "in AcceptClient() clients number is: " << this->_clients.size()
       << "\t accepted client: " << client._address.GetAddressIp() << ":" << client._address.GetAddressPort() << std::endl;
    std::cout << ss.str();
#endif
    return client._socket_id;
}

template <typename T>
int TCP::ReceiveAll(TCP& client, unsigned int ms, T* buffer, size_t len) throw()
{
    int ready;
    int ret;
    SocketId maxfd = 0;
    fd_set client_rset;
    struct timeval tv = {(time_t)(ms/1000), ms%1000};

#ifdef _DEBUG
    std::stringstream ss;
#endif

    if (!this->_binded)
        return SOCKET_ERROR;
    if (!this->_opened)
        return SOCKET_ERROR;

    size_t clients_num; // for thread safe
    {
        LockGuard lock(this->_clients_mutex);
        clients_num = this->_clients.size();
        FD_ZERO(&client_rset);
        for (unsigned int i = 0; i < clients_num; ++i)
        {
            maxfd = (maxfd < this->_clients[i].first) ? this->_clients[i].first : maxfd;
            FD_SET(this->_clients[i].first, &client_rset);
        }
    }

    ready = ::select(maxfd+1, &client_rset, NULL, NULL, &tv);

    // error
    if (ready == SOCKET_ERROR || ready < 0)
        return SOCKET_ERROR;

    // timeout
    if (ready == 0)
        return SOCKET_TIMEOUT;

    // something to read
#ifdef _DEBUG
    ss << "select() return: " << ready << std::endl;
    std::cout << ss.str();
#endif
    for (unsigned int i = 0; i < clients_num; ++i)
    {
        if (FD_ISSET(this->_clients[i].first, &client_rset))
        {
            client._socket_id = this->_clients[i].first;
            client._address = this->_clients[i].second;
            client._opened = true;
            client._binded = true;
            client._ispeer = true;
            ret = ::recv(this->_clients[i].first, (char *)buffer, len, 0);
#ifdef _DEBUG
            ss << "recvfrom() return: " << ret << std::endl;
            std::cout << ss.str();
#endif
            if (ret == 0 || (ret == SOCKET_ERROR
#ifdef WINDOWS
                             && WSAGetLastError() == WSAECONNRESET
#else
                             && errno == ECONNRESET
#endif
                             ))
            {
                // Client socket closed
#ifdef WINDOWS
                closesocket(this->_clients[i].first);
#else
                shutdown(this->_clients[i].first, SHUT_RDWR);
                ::close(this->_clients[i].first);
#endif

#ifdef _DEBUG
                ss << "client (socket_id = " << this->_clients[i].first
                   << ", socket_address = " << this->_clients[i].second << ") closed" << std::endl;
                std::cout << ss.str();
#endif
                {
                    LockGuard lock(this->_clients_mutex);
                    this->_clients.erase(this->_clients.begin() + i);
                }
#ifdef _DEBUG
                ss << "in select_receive() clients number is: " << this->_clients.size() << std::endl;
                std::cout << ss.str();
#endif
                break;
            }
            else
            {
                // Receive something from client
            }
        }
    }
    return ret;
}
}

// UDP
namespace MVComponent
{
UDP::UDP(void) : CommonSocket(SOCK_DGRAM)
{
}

UDP::UDP(const UDP &udp) : CommonSocket()
{
    this->_socket_id = udp._socket_id;
    this->_opened = udp._opened;
    this->_binded = udp._binded;
}

Ip UDP::GetAddressIp(void)
{
    return (this->GetAddress()).GetAddressIp();
}

Port UDP::GetAddressPort(void)
{
    return (this->GetAddress()).GetAddressPort();
}

Address UDP::GetAddress(void)
{
    Address address;
    socklen_t len = sizeof(address);
    if (getsockname(this->_socket_id, (struct sockaddr*)&address, &len) < 0)
        throw SocketException("[get_address] getsockname() error");

    return address;
}

int UDP::Send(Ip ip, Port port, const char* data, size_t len)
{
    if (!this->_opened) this->Open();

    if (len > SOCKET_MAX_BUFFER_BYTES)
    {
        std::stringstream error;
        error << "[send] with [ip=" << ip << "] [port=" << port << "] [len=" << len
              << "] Data length higher then max buffer len (" << SOCKET_MAX_BUFFER_BYTES << ")";
        throw SocketException(error.str());
    }

    Address address(ip, port);
    int ret;

    if ((ret = sendto(this->_socket_id, (const char*)data, len, 0, (struct sockaddr*)&address, sizeof(struct sockaddr))) == -1)
    {
        std::stringstream error;
        error << "[send] with [ip=" << ip << "] [port=" << port << "] [data=" << data
              << "] [len=" << len << "] Cannot send";
        throw SocketException(error.str());
    }

    return ret;
}

int UDP::Send(const Address& address, const char* data, size_t len)
{
    return this->Send(address.GetAddressIp(), address.GetAddressPort(), data, len);
}

template <typename T>
int UDP::Send(Ip ip, Port port, const T *data, size_t len)
{
    if (!this->_opened) this->Open();

    len *= sizeof(T);
    if (len > SOCKET_MAX_BUFFER_BYTES)
    {
        std::stringstream error;
        error << "[send] with [ip=" << ip << "] [port=" << port << "] [len=" << len
              << "] Data length higher then max buffer len (" << SOCKET_MAX_BUFFER_BYTES << ")";
        throw SocketException(error.str());
    }

    Address address(ip, port);
    int ret;

    if ((ret = sendto(this->_socket_id, (const char*)data, len, 0, (struct sockaddr*)&address, sizeof(struct sockaddr))) == -1)
    {
        std::stringstream error;
        error << "[send] with [ip=" << ip << "] [port=" << port << "] [data=" << data
              << "] [len=" << len << "] Cannot send";
        throw SocketException(error.str());
    }

    return ret;
}

template <typename T>
int UDP::Send(const Address& address, const T *data, size_t len)
{
    return this->Send<T>(address.GetIp(), address.GetPort(), data, len);
}

template <typename T>
int UDP::Send(Ip ip, Port port, T data)
{
    return this->Send<T>(ip, port, &data, 1);
}

template <typename T>
int UDP::Send(const Address& address, T data)
{
    return this->Send<T>(address.GetIp(), address.GetPort(), &data, 1);
}

template <>
int UDP::Send<std::string>(Ip ip, Port port, std::string data)
{
    return this->Send<char>(ip, port, data.c_str(), data.length() + 1);
}

template <>
int UDP::Send<std::string>(const Address& address, std::string data)
{
    return this->Send<char>(address.GetAddressIp(), address.GetAddressPort(), data.c_str(), data.length() + 1);
}

int UDP::Receive(Address& address, char* data, unsigned int& len)
{
    if (!this->_opened) this->Open();
    if (!this->_binded) throw SocketException("[receive] Make the socket listening before receiving");

    if (len > SOCKET_MAX_BUFFER_BYTES)
    {
        std::stringstream error;
        error << "[receive] with [buffer=" << data << "] [len=" << len
              << "] Data length higher then max buffer length (" << SOCKET_MAX_BUFFER_BYTES << ")";
        throw SocketException(error.str());
    }

    int received_bytes;
    socklen_t size = sizeof(struct sockaddr);

    if ((received_bytes = recvfrom(this->_socket_id, (char*)data, len, 0, (struct sockaddr*)&address, (socklen_t*)&size)) == SOCKET_ERROR)
    {
        throw SocketException("[receive] Cannot receive");
    }

    len = (received_bytes > 0) ? received_bytes : 0;

    return received_bytes;
}

int UDP::ReceiveTimeout(unsigned int ms, Address& address, char* data, unsigned int& len)
{
    if (!this->_opened) this->Open();
    if (!this->_binded) throw SocketException("[receive_timeout] Make the socket listening before receiving");

    if (len > SOCKET_MAX_BUFFER_BYTES)
    {
        std::stringstream error;
        error << "[receive_timeout] with [buffer=" << data << "] [len=" << len
              << "] Data length higher then max buffer length (" << SOCKET_MAX_BUFFER_BYTES << ")";
        throw SocketException(error.str());
    }

    int received_bytes = 0;
    socklen_t size = sizeof(struct sockaddr);
    fd_set rset;
    int ready;
    struct timeval timeout = {(time_t)(ms/1000), ms%1000};

    FD_ZERO(&rset);
    FD_SET(this->_socket_id, &rset);

    ready = ::select(this->_socket_id+1, &rset, NULL, NULL, &timeout);
    if (ready == SOCKET_ERROR)
    {
        throw SocketException("[receive_timeout] select() return SOCKET_ERROR");
    }
    else if (ready > 0)
    {
        if (FD_ISSET(this->_socket_id, &rset))
        {
            received_bytes = recvfrom(this->_socket_id, (char*)data, len, 0, (struct sockaddr*)&address, (socklen_t*)&size);
            if (received_bytes == SOCKET_ERROR)
            {
                throw SocketException("[receive_timeout] Cannot receive");
            }

            len = (received_bytes > 0) ? received_bytes : 0;
        }
    }

    return received_bytes;
}

template <typename T>
inline int UDP::Receive(Address& address, T *data, size_t len, unsigned int& received_elements)
{
    if (!this->_opened) this->Open();
    if (!this->_binded) throw SocketException("[receive] Make the socket listening before receiving");

    len *= sizeof(T);
    if (len > SOCKET_MAX_BUFFER_BYTES)
    {
        std::stringstream error;
        error << "[receive] with [buffer=" << data << "] [len=" << len
              << "] Data length higher then max buffer length (" << SOCKET_MAX_BUFFER_BYTES << ")";
        throw SocketException(error.str());
    }

    int received_bytes;
    socklen_t size = sizeof(struct sockaddr);

    if ((received_bytes = recvfrom(this->_socket_id, (char*)data, len, 0, (struct sockaddr*)&address, (socklen_t*)&size)) == SOCKET_ERROR)
    {
        throw SocketException("[receive] Cannot receive");
    }

    received_elements = (unsigned int)(received_bytes / sizeof(T));

    return received_bytes;
}

template <typename T>
Datagram<T*> UDP::Receive(T *buffer, size_t len)
{
    Datagram<T*> ret;

    ret.received_bytes = this->Receive<T>(ret.address, buffer, len, ret.received_elements);
    ret.data = buffer;

    return ret;
}

template <typename T, size_t N>
Datagram<T[N]> UDP::Receive(size_t len)
{
    Datagram<T[N]> ret;
    ret.received_bytes = this->Receive<T>(ret.address, ret.data, len, ret.received_elements);
    return ret;
}

template <typename T>
Datagram<T> UDP::Receive(void)
{
    Datagram<T> ret;
    ret.received_bytes = this->Receive<T>(ret.address, &ret.data, 1, ret.received_elements);
    return ret;
}

template <>
Datagram<std::string> UDP::Receive<std::string>(void)
{
    Datagram<std::string> ret;
    char buffer[SOCKET_MAX_BUFFER_BYTES];

    ret.received_bytes = this->Receive<char>(ret.address, buffer, SOCKET_MAX_BUFFER_BYTES, ret.received_elements);
    ret.data = buffer;

    return ret;
}

template <typename T>
inline int UDP::ReceiveTimeout(unsigned int ms, Address& address, T* data, size_t len, unsigned int& received_elements)
{
    if (!this->_opened) this->Open();
    if (!this->_binded) throw SocketException("[receive_timeout] Make the socket listening before receiving");

    len *= sizeof(T);
    if (len > SOCKET_MAX_BUFFER_BYTES)
    {
        std::stringstream error;
        error << "[receive_timeout] with [buffer=" << data << "] [len=" << len
              << "] Data length higher then max buffer length (" << SOCKET_MAX_BUFFER_BYTES << ")";
        throw SocketException(error.str());
    }

    int received_bytes = 0;
    socklen_t size = sizeof(struct sockaddr);
    fd_set rset;
    int ready;
    struct timeval timeout = {(time_t)(ms/1000), ms%1000};

    FD_ZERO(&rset);
    FD_SET(this->_socket_id, &rset);

    ready = ::select(this->_socket_id+1, &rset, NULL, NULL, &timeout);
    if (ready == SOCKET_ERROR)
    {
        throw SocketException("[receive_timeout] select() return SOCKET_ERROR");
    }
    else if (ready > 0)
    {
        if (FD_ISSET(this->_socket_id, &rset))
        {
            received_bytes = recvfrom(this->_socket_id, (char*)data, len, 0, (struct sockaddr*)&address, (socklen_t*)&size);
            if (received_bytes == SOCKET_ERROR)
            {
                throw SocketException("[receive_timeout] Cannot receive");
            }

            received_elements = (unsigned int)(received_bytes / sizeof(T));
        }
    }

    return received_bytes;
}

template <typename T>
Datagram<T*> UDP::ReceiveTimeout(unsigned int ms, T *buffer, size_t len)
{
    Datagram<T*> ret;

    ret.receive_bytes = this->ReceiveTimeout<T>(ms, ret.address, buffer, len, ret.received_elements);
    ret.data = buffer;

    return ret;
}

template <typename T, size_t N>
Datagram<T[N]> UDP::ReceiveTimeout(unsigned int ms, size_t len)
{
    Datagram<T[N]> ret;

    ret.receive_bytes = this->ReceiveTimeout<T>(ms, ret.address, ret.data, len, ret.received_elements);

    return ret;
}

template <typename T>
Datagram<T> UDP::ReceiveTimeout(unsigned int ms)
{
    Datagram<T> ret;

    ret.receive_bytes = this->ReceiveTimeout<T>(ms, ret.address, ret.data, 1, ret.received_elements);

    return ret;
}

template <>
Datagram<std::string> UDP::ReceiveTimeout<std::string>(unsigned int ms)
{
    Datagram<std::string> ret;
    char buffer[SOCKET_MAX_BUFFER_BYTES];

    ret.received_bytes = this->ReceiveTimeout<char>(ms, ret.address, buffer, SOCKET_MAX_BUFFER_BYTES, ret.received_elements);
    ret.data = buffer;

    return ret;
}
}
