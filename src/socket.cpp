#include "../utils/socket.h"
#include <atomic>
#include <memory>

namespace UTILS
{
	struct ADDRINFOT_deleter
	{
		void operator()(ADDRINFOA * ptr) const
		{
			FreeAddrInfoA(ptr);
		}

		void operator()(ADDRINFOW * ptr) const
		{
			FreeAddrInfoW(ptr);
		}
	};

	static std::string convertIntegerToString(int v)
	{
		char szStr[64];
#ifdef WIN32
		_snprintf_s(szStr, _TRUNCATE, "%d", v);
#else
		snprintf(szStr, 64, "%d", v);
#endif
		return std::string(szStr);
	}

	static bool setSocketBlocking(SOCKET s)
	{
#ifdef WIN32
		u_long val = 0;
		int ret = ioctlsocket(s, FIONBIO, &val);
#else
#endif
		if (ret == SOCKET_ERROR)
		{
			//setLastSocketError(WSAGetLastError());
			return false;
		}
		else
			return true;
	}

	static bool socketEventHandlingCleanup(SOCKET s)
	{
		bool ret = setSocketBlocking(s);
		return ret;
	}

	enum WSInitStates
	{
		WS_UNINITIALIZED,
		WS_INITIALIZING,
		WS_INITIALIZED
	};

	static WSADATA wsa;
	static std::atomic<WSInitStates> winsock_state(WS_UNINITIALIZED);

	/*
	比较并交换被封装的值(strong)与参数 expected 所指定的值是否相等，如果：
	相等，则用 val 替换原子对象的旧值。
	不相等，则用原子对象的旧值替换 expected,
	*/
	static inline WSInitStates interlocked_compare_exchange(std::atomic<WSInitStates>& var,
		WSInitStates expected,
		WSInitStates desired)
	{
		var.compare_exchange_strong(expected, desired);
		return expected;
	}

	static void init_winsock_worker()
	{
#ifdef WIN32
		// Try to change the state to WS_INITIALIZING.
		WSInitStates val = interlocked_compare_exchange(winsock_state, WS_UNINITIALIZED, WS_INITIALIZING);
		switch (val)
		{
		case WS_UNINITIALIZED:
		{
			int ret = WSAStartup(MAKEWORD(2, 2), &wsa);
			if (ret != 0)
			{
				// Revert the state back to WS_UNINITIALIZED to unblock other
				// threads and let them throw exception.
				val = interlocked_compare_exchange(winsock_state, WS_INITIALIZING, WS_UNINITIALIZED);
				assert(val == WS_INITIALIZING);
				//OutputDebugString("Could not initialize WinSock.");
			}

			// WinSock is initialized, change the state to WS_INITIALIZED.
			val = interlocked_compare_exchange(winsock_state, WS_INITIALIZING, WS_INITIALIZED);
			assert(val == WS_INITIALIZING);
			return;
		}

		case WS_INITIALIZING:
			// Wait for state change.
			while (true)
			{
				switch (winsock_state)
				{
				case WS_INITIALIZED:
					return;

				case WS_INITIALIZING:
					if (!SwitchToThread())
						Sleep(0);
					continue;

				default:
					assert(0);
					//OutputDebugString("Unknown WinSock state.");
				}
			}

		case WS_INITIALIZED:
			// WinSock is already initialized.
			return;

		default:
			assert(0);
			//OutputDebugString("Unknown WinSock state.");
		}
#endif
	}
	static void init_winsock()
	{
		// Quick check first to avoid the expensive interlocked compare
		// and exchange.
		if (winsock_state == WS_INITIALIZED)
			return;
		else
			init_winsock_worker();
	}

	//////////////////////////////////////////////////////////////////////////////
	// AbstractSocket ctors and dtor
	//////////////////////////////////////////////////////////////////////////////

	AbstractSocket::AbstractSocket()
		: sock(INVALID_SOCKET),
		state(not_opened),
		err(0)
	{
	}

	AbstractSocket::AbstractSocket(SOCKET sock_, SocketState state_, int err_)
		: sock(sock_),
		state(state_),
		err(err_)
	{
		setAsyncSkt();
	}

	AbstractSocket::AbstractSocket(AbstractSocket && rhs) throw()
		: AbstractSocket()
	{
		swap(rhs);
	}

	AbstractSocket::~AbstractSocket()
	{
		close();
	}

	//////////////////////////////////////////////////////////////////////////////
	// AbstractSocket methods
	//////////////////////////////////////////////////////////////////////////////

	void AbstractSocket::setAsyncSkt()
	{
		if (sock != INVALID_SOCKET)
		{
#ifdef WIN32
			uint32_t argp;
			argp = 1;
			ioctlsocket(sock, FIONBIO, (u_long*)&argp);
#else
			int flags = fcntl(fdListen, F_GETFL, 0);
			fcntl(fdListen, F_SETFL, flags | O_NONBLOCK); //设置成非阻塞模式；
#endif
		}
	}

	void AbstractSocket::close()
	{
		if (sock != INVALID_SOCKET)
		{
			::closesocket(sock);
			sock = INVALID_SOCKET;
			state = not_opened;
		}
	}

	void
		AbstractSocket::shutdown()
	{
		/*if (sock != INVALID_SOCKET)
		shutdown(sock);*/
		//::shutdown(to_os_socket(sock), SD_BOTH);
	}

	bool AbstractSocket::isOpen() const
	{
		return sock != INVALID_SOCKET;
	}

	bool AbstractSocket::isConnect() const
	{
		return state == ok && err == ok;
	}

	bool AbstractSocket::isError() const
	{
#ifdef WIN32
		return err != ok;
#else
#endif
	}

	AbstractSocket& AbstractSocket::operator= (AbstractSocket&& rhs) throw()
	{
		swap(rhs);
		return *this;
	}

	void AbstractSocket::swap(AbstractSocket& rhs)
	{
		using std::swap;

		swap(sock, rhs.sock);
		swap(state, rhs.state);
		swap(err, rhs.err);
	}


	//////////////////////////////////////////////////////////////////////////////
	// Socket ctors and dtor
	//////////////////////////////////////////////////////////////////////////////

	Socket::Socket()
		: AbstractSocket()
	{ }


	Socket::Socket(const std::string& address, unsigned short port,
		bool udp /*= false*/, bool ipv6 /*= false */)
		: AbstractSocket()
	{
		connect(address, port, udp, ipv6);
	}


	Socket::Socket(SOCKET sock_, SocketState state_, int err_)
		: AbstractSocket(sock_, state_, err_)
	{ }


	Socket::Socket(Socket && other) throw()
		: AbstractSocket(std::move(other))
	{ }


	Socket::~Socket()
	{ }

	Socket& Socket::operator= (Socket && other) throw()
	{
		swap(other);
		return *this;
	}


	//////////////////////////////////////////////////////////////////////////////
	// Socket methods
	//////////////////////////////////////////////////////////////////////////////
	bool Socket::connect(const std::string& address,
		unsigned short port,
		unsigned int uiTimeOut /*= 5000*/,
		bool udp /*= false*/,
		bool ipv6 /*= false*/)
	{
		if (isConnect())
		{
			return true;
		}
		err = ok;
		sock = ConnectSocket(address, port, udp, ipv6, state, err, true, uiTimeOut);
		if (sock == INVALID_SOCKET)
		{
			return false;
		}

		if (!udp && setTCPNoDelay(sock, true, err) != 0)
		{
			return false;
		}

		if (!isConnect())
		{
			return false;
		}
		setAsyncSkt();
		return true;
	}

	bool Socket::read(SocketBuffer& buffer,
		const int ciReadLen /*= 0*/,
		unsigned int uiTimeOut /*= 5000*/)
	{
		long retval = UTILS::read(sock, buffer, err, ciReadLen, uiTimeOut);
		return (retval > 0);
	}

	bool Socket::write(SocketBuffer& buffer,
		unsigned int uiTimeOut /*= 5000*/)
	{
		long retval = UTILS::write(sock, buffer, err, uiTimeOut);
		return (retval > 0);
	}

	bool Socket::write(const std::string& buffer,
		unsigned int uiTimeOut /*= 5000*/)
	{
		long retval = UTILS::write(sock, buffer, err, uiTimeOut);
		return retval > 0;
	}

	bool Socket::write(const char* buff,
		int len,
		unsigned int uiTimeOut /*= 5000*/)
	{
		SocketBuffer buffer((char*)buff, len);
		return UTILS::write(sock, buffer, err, uiTimeOut);
	}

	ServerSocket::ServerSocket(unsigned short port,
		bool accept_block/* = true*/,
		bool udp /*= false*/,
		bool ipv6 /*= false*/,
		const std::string& host/* = std::string()*/)
	{
		// Initialize these here so that we do not try to close invalid handles
		// in dtor if the following `openSocket()` fails.

		sock = OpenSocket(host, port, udp, ipv6, state, err);
		if (accept_block)
		{
#ifdef WIN32
			uint32_t argp;
			argp = 1;
			ioctlsocket(sock, FIONBIO, (u_long*)&argp);
#else
			int flags = fcntl(fdListen, F_GETFL, 0);
			fcntl(fdListen, F_SETFL, flags | O_NONBLOCK); //设置成非阻塞模式；
#endif
		}
	}

	ServerSocket::ServerSocket(ServerSocket&& other) throw()
		: AbstractSocket(std::move(other))
	{
	}

	ServerSocket::~ServerSocket()
	{
	}

	ServerSocket& ServerSocket::operator= (ServerSocket&& other) throw()
	{
		swap(other);
		return *this;
	}

	void ServerSocket::swap(ServerSocket& other)
	{
		AbstractSocket::swap(other);
	}

	Socket ServerSocket::accept()
	{
		if (!isOpen())
		{
			return Socket();
		}

		SocketState st = not_opened;
		SOCKET clientSock = AcceptSocket(sock, st, err);
		int eno = 0;
		if (clientSock == INVALID_SOCKET)
			eno = getLastSocketError();

		return Socket(clientSock, st, eno);
	}

	//////////////////////////////////////////////////////////////////////////////
	// SocketBuffer ctors and dtor
	//////////////////////////////////////////////////////////////////////////////

	SocketBuffer::SocketBuffer(unsigned int maxsize_)
		: maxsize(maxsize_),
		size(0),
		pos(0),
		bHeap(true),
		buffer(new char[maxsize])
	{
	}

	SocketBuffer::SocketBuffer(char* src, int len)
		: maxsize(len),
		size(len),
		pos(0),
		bHeap(false),
		buffer(src)
	{
		assert(buffer != NULL);
	}

	SocketBuffer::~SocketBuffer()
	{
		if (bHeap)
		{
			delete[] buffer;
		}
	}


	//////////////////////////////////////////////////////////////////////////////
	// SocketBuffer methods
	//////////////////////////////////////////////////////////////////////////////

	unsigned char SocketBuffer::readByte()
	{
		if (pos >= maxsize) {
			//OutputDebugString("SocketBuffer::readByte()- end of buffer reached");
			return 0;
		}
		else if ((pos + sizeof(unsigned char)) > maxsize) {
			//OutputDebugString("SocketBuffer::readByte()- Attempt to read beyond end of buffer");
			return 0;
		}

		unsigned char ret = static_cast<unsigned char>(buffer[pos]);
		pos += sizeof(unsigned char);

		return ret;
	}

	unsigned short SocketBuffer::readShort()
	{
		if (pos >= maxsize) {
			//OutputDebugString("SocketBuffer::readShort()- end of buffer reached");
			return 0;
		}
		else if ((pos + sizeof(unsigned short)) > maxsize) {
			//OutputDebugString("SocketBuffer::readShort()- Attempt to read beyond end of buffer");
			return 0;
		}

		unsigned short ret;
		std::memcpy(&ret, buffer + pos, sizeof(ret));
		ret = ntohs(ret);
		pos += sizeof(unsigned short);

		return ret;
	}

	unsigned int SocketBuffer::readInt()
	{
		if (pos >= maxsize) {
			//OutputDebugString("SocketBuffer::readInt()- end of buffer reached");
			return 0;
		}
		else if ((pos + sizeof(unsigned int)) > maxsize) {
			//OutputDebugString("SocketBuffer::readInt()- Attempt to read beyond end of buffer");
			return 0;
		}

		unsigned int ret;
		std::memcpy(&ret, buffer + pos, sizeof(ret));
		ret = ntohl(ret);
		pos += sizeof(unsigned int);

		return ret;
	}

	void SocketBuffer::appendByte(unsigned char val)
	{
		if ((pos + sizeof(unsigned char)) > maxsize) {
			//OutputDebugString("SocketBuffer::appendByte()- Attempt to write beyond end of buffer");
		}

		buffer[pos] = static_cast<char>(val);
		pos += sizeof(unsigned char);
		size = pos;
	}

	void SocketBuffer::appendShort(unsigned short val)
	{
		if ((pos + sizeof(unsigned short)) > maxsize) {
			//OutputDebugString("SocketBuffer::appendShort()-Attempt to write beyond end of buffer");
		}

		unsigned short s = htons(val);
		std::memcpy(buffer + pos, &s, sizeof(s));
		pos += sizeof(s);
		size = pos;
	}

	void SocketBuffer::appendInt(unsigned int val)
	{
		if ((pos + sizeof(unsigned int)) > maxsize) {
			//OutputDebugString("SocketBuffer::appendInt()- Attempt to write beyond end of buffer");
			return;
		}

		int i = htonl(val);
		std::memcpy(buffer + pos, &i, sizeof(i));
		pos += sizeof(i);
		size = pos;
	}

	void SocketBuffer::appendString(const std::string& str)
	{
		std::size_t const strlen = str.length();
		std::size_t const sizeOfChar = sizeof(TCHAR) == 1 ? 1 : 2;

		if ((pos + strlen * sizeOfChar) > maxsize)
		{
			//OutputDebugString("SocketBuffer::appendString()- Attempt to write beyond end of buffer");
			return;
		}

#ifndef UNICODE
		std::memcpy(&buffer[pos], str.data(), strlen);
		pos += strlen;
		size = pos;
#else
		for (tstring::size_type i = 0; i < strlen; ++i) {
			appendShort(static_cast<unsigned short>(str[i]));
		}
#endif
	}

	void SocketBuffer::appendBuffer(const SocketBuffer& buf)
	{
		if ((pos + buf.getSize()) > maxsize) {
			//OutputDebugString("SocketBuffer::appendBuffer()- Attempt to write beyond end of buffer");
			return;
		}

		std::memcpy(&buffer[pos], buf.buffer, buf.getSize());
		pos += buf.getSize();
		size = pos;
	}

	SOCKET OpenSocket(unsigned short port, bool udp, bool ipv6, SocketState& state, int& error)
	{
		return OpenSocket(std::string(), port, udp, ipv6, state, error);
	}

	SOCKET OpenSocket(const std::string& host,
		unsigned short port, bool udp, bool ipv6, SocketState& state, int& error)
	{
		ADDRINFOT addr_info_hints{};
		PADDRINFOT ai = nullptr;
		std::unique_ptr<ADDRINFOT, ADDRINFOT_deleter> addr_info;
		int const family = ipv6 ? AF_INET6 : AF_INET;
		int const socket_type = udp ? SOCK_DGRAM : SOCK_STREAM;
		int const protocol = udp ? IPPROTO_UDP : IPPROTO_TCP;
		std::string port_str = convertIntegerToString(port);
		int retval;

		init_winsock();

		addr_info_hints.ai_family = family;
		addr_info_hints.ai_socktype = socket_type;
		addr_info_hints.ai_protocol = protocol;
		addr_info_hints.ai_flags = AI_PASSIVE;
		retval = GetAddrInfo(host.empty() ? nullptr : host.c_str(),
			port_str.c_str(),
			&addr_info_hints,
			&ai);
		if (retval != 0)
		{
			error = retval;
			return INVALID_SOCKET;
		}

		addr_info.reset(ai);

		SOCKET skt = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
		if (skt == INVALID_SOCKET)
			goto _error;

		/*char log[64];
		sockaddr_in* p = (sockaddr_in*)ai->ai_addr;
		_snprintf_s(log, _TRUNCATE, "~~~~~~~~~~.%s", inet_ntoa(p->sin_addr));
		OutputDebugString(log);*/

		if (bind(skt, ai->ai_addr, static_cast<int>(ai->ai_addrlen)) != 0)
			goto _error;

		if (::listen(skt, 64) != 0)
			goto _error;

		state = ok;
		return skt;

	_error:
		error = getLastSocketError();
		return INVALID_SOCKET;
	}

	SOCKET ConnectSocket(const std::string& hostn, unsigned short port, bool udp, bool ipv6,
		SocketState& state, int& error, bool bNonBlock /*= true*/, unsigned int uiTimeOut /*= 5000*/)
	{
		ADDRINFOT addr_info_hints{};
		PADDRINFOT ai = nullptr;
		std::unique_ptr<ADDRINFOT, ADDRINFOT_deleter> addr_info;
		int const family = ipv6 ? AF_INET6 : AF_INET;
		int const socket_type = udp ? SOCK_DGRAM : SOCK_STREAM;
		int const protocol = udp ? IPPROTO_UDP : IPPROTO_TCP;
		std::string port_str = convertIntegerToString(port);
		int retval;

		init_winsock();

		addr_info_hints.ai_family = family;
		addr_info_hints.ai_socktype = socket_type;
		addr_info_hints.ai_protocol = protocol;
		addr_info_hints.ai_flags = AI_NUMERICSERV;
		retval = GetAddrInfo(hostn.c_str(),
			port_str.c_str(),
			&addr_info_hints,
			&ai);
		if (retval != 0)
		{
			error = retval;
			return INVALID_SOCKET;
		}

		addr_info.reset(ai);
		SOCKET skt = INVALID_SOCKET;
		bool bSuccess = false;
		for (ADDRINFOT * rp = ai; rp; rp = rp->ai_next)
		{
#if 1
			struct sockaddr_in*  name = (sockaddr_in*)rp->ai_addr;
			char ip[64];
#ifdef WIN32
			//strncpy_s(ip, 63, inet_ntoa(name->sin_addr), _TRUNCATE);
			char IPdotdec[64];
			IPdotdec[0] = '\0';
			inet_ntop(AF_INET, &name->sin_addr, IPdotdec, 64);
			strncpy_s(ip, _TRUNCATE, IPdotdec, _TRUNCATE);
#else
			snprintf(ip, 63, "%s", inet_ntoa(name.sin_addr));
#endif
			OutputDebugString(ip);
#endif
			skt = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
			if (skt == INVALID_SOCKET)
			{
				continue;
			}
			if (bNonBlock)
			{
#ifdef WIN32
				uint32_t argp;
				argp = 1;
				ioctlsocket(skt, FIONBIO, (u_long*)&argp);
#else
				int flags = fcntl(fdListen, F_GETFL, 0);
				fcntl(fdListen, F_SETFL, flags | O_NONBLOCK); //设置成非阻塞模式；
#endif
			}
			retval = ::connect(skt, rp->ai_addr, static_cast<int>(rp->ai_addrlen));
			/*while (
			(retval = ::connect(skt, rp->ai_addr,static_cast<int>(rp->ai_addrlen))) == -1
			&& (WSAGetLastError() == WSAEINTR))
			;*/
			if (retval != SOCKET_ERROR)
			{
				bSuccess = true;
				break;
			}
		}

		if (skt == INVALID_SOCKET)
		{
			error = getLastSocketError();
			return INVALID_SOCKET;
		}

		if (bNonBlock)
		{
			timeval tm;
			fd_set rest, west;
			int iErr = 0, len = sizeof(int);
			tm.tv_sec = uiTimeOut / 1000;
			tm.tv_usec = uiTimeOut % 1000;

			FD_ZERO(&rest);
			FD_ZERO(&west);
			FD_SET(skt, &rest);
			FD_SET(skt, &west);

			if (select(skt + 1, &rest, &west, NULL, &tm) > 0)
			{
				//如果套接口及可写也可读，需要进一步判断  
				if (FD_ISSET(skt, &rest) && FD_ISSET(skt, &west))
				{
					iErr = -1;
					if (getsockopt(skt, SOL_SOCKET, SO_ERROR, (char*)&iErr, (socklen_t *)&len) > 0)
					{
						if (iErr == 0)
						{
							bSuccess = true;
						}
					}
				}
				//如果套接口可写不可读,则链接完成  
				else if (FD_ISSET(skt, &west) && !FD_ISSET(skt, &rest))
				{
					bSuccess = true;
				}
			}
		}

		if (!bSuccess)
		{
			error = getLastSocketError();
			return skt;
		}

		state = ok;
		error = ok;
		return skt;
	}

	SOCKET AcceptSocket(SOCKET sock, SocketState& state, int& error)
	{
		init_winsock();
		SOCKET connected_socket = ::accept(sock, nullptr, nullptr);
		if (connected_socket != INVALID_SOCKET)
			state = ok;
		else
			error = getLastSocketError();

		return connected_socket;
	}

	int CloseSocket(SOCKET sock)
	{
		return ::closesocket(sock);
	}

	int ShutdownSocket(SOCKET sock)
	{
		return ::shutdown(sock, SD_BOTH);
	}

	long read(SOCKET sock, SocketBuffer& buffer, int& error, const int ciReadLen /*= 0*/, unsigned int uiTimeOut/* = 5000*/)
	{
		int iError = 0, iDataLen = 0, iReadLen = 0;
		BOOL bRecvData = TRUE;
		DWORD dwTimeOut = GetTickCount();
		fd_set readfd, excepfd;
		struct timeval select_time;

		if ((int)(buffer.getMaxSize() - buffer.getSize()) < ciReadLen)
		{
			return false;
		}
		do
		{
			if (abs((int)(GetTickCount() - dwTimeOut)) > (int)uiTimeOut)
			{
				break;
			}
			if (ciReadLen > 0 && ciReadLen - iDataLen <= 0)
			{
				break;
			}
			if (!bRecvData)
			{
				FD_ZERO(&readfd);
				FD_SET(sock, &readfd);

				excepfd = readfd;

				select_time.tv_sec = 0;
				select_time.tv_usec = 100000;
				int iRet = select(sock + 1, &readfd, NULL, &excepfd, &select_time);
				if (iRet > 0)
				{
					iRet = FD_ISSET(sock, &excepfd);
					if (iRet)
					{
						//error
						OutputDebugString("...");
						break;
					}
					//是否有数据
					iRet = FD_ISSET(sock, &readfd);
					if (iRet)
					{
						bRecvData = TRUE;
					}
				}
				else {//如果没有根据指定读取数据大小,则读取到多少数据就返回.
					if (iDataLen > 0 && ciReadLen <= 0)
					{
						break;
					}
				}
			}
			else {
				iReadLen = ::recv(sock,
					buffer.getBuffer() + buffer.getSize(),
					ciReadLen > 0 ? ciReadLen - iDataLen : static_cast<int>(buffer.getMaxSize() - buffer.getSize()),
					0);
				bRecvData = FALSE;
				if (iReadLen > 0)
				{
					buffer.setSize(buffer.getSize() + iReadLen);
					iDataLen += iReadLen;
				}
				if (iReadLen == 0)
				{
					error = getLastSocketError();
					break;
				}
				if (iReadLen == SOCKET_ERROR)
				{
#ifdef WIN32
					iError = WSAGetLastError();
					if (iError == WSAEWOULDBLOCK)
					{
						if (iDataLen > 0 && ciReadLen <= 0)
						{//如果没有根据指定读取数据大小
							break;
						}
						continue;
					}
#else
					if (errno == EAGAIN)//非阻塞的模式,所以当errno为EAGAIN时,表示当前缓冲区已无数据可读
					{
						if (iReadLen > 0)
						{
							break;
						}
						continue;
					}
					iError = errno;
#endif
					error = getLastSocketError();
					break;
				}
			}

		} while (iDataLen < static_cast<long>(buffer.getMaxSize()));

		return iDataLen;
	}

	long write(SOCKET sock, SocketBuffer& buffer, int& error, unsigned int uiTimeOut/* = 5000*/)
	{
		int iSendLen = 0, iError = 0, ret = 0;
		BOOL bSendData = TRUE;
		DWORD dwTimeOut = GetTickCount();
		fd_set writefd, excepfd;
		struct timeval select_time;

		do
		{
			if (abs((int)(GetTickCount() - dwTimeOut)) > (int)uiTimeOut)
			{
				break;
			}

			if (buffer.getPos() >= buffer.getSize())
			{
				break;
			}

			if (!bSendData)
			{
				FD_ZERO(&writefd);
				FD_SET(sock, &writefd);

				excepfd = writefd;

				select_time.tv_sec = 0;
				select_time.tv_usec = 100000;
				int iRet = select(sock + 1, NULL, &writefd, &excepfd, &select_time);
				if (iRet > 0)
				{
					iRet = FD_ISSET(sock, &excepfd);
					if (iRet)
					{
						//error
						OutputDebugString("..");
						break;
					}
					//是否有数据
					iRet = FD_ISSET(sock, &writefd);
					if (iRet)
					{
						bSendData = TRUE;
					}
				}
			}
			else {
				ret = ::send(sock,
					buffer.getBuffer() + buffer.getPos(),
					static_cast<int>(buffer.getSize() - buffer.getPos()),
					0);
				bSendData = FALSE;
				if (ret > 0)
				{
					buffer.setPos(buffer.getPos() + ret);
					iSendLen += ret;
				}

				if (ret == SOCKET_ERROR)
				{
#ifdef WIN32
					iError = WSAGetLastError();
					if (iError == WSAEWOULDBLOCK)
					{
						continue;
					}

#else 
					iError = errno;
					if (iError == EAGAIN)//当socket是非阻塞时,如返回此错误,表示写缓冲队列已满,
					{
						continue;
					}
					//EINTR
#endif
					error = getLastSocketError();
					break;
				}

			}

		} while (buffer.getPos() < buffer.getSize());

		return iSendLen;
	}

	long write(SOCKET sock, const std::string& buffer, int& error, unsigned int uiTimeOut/* = 5000*/)
	{
		SocketBuffer buff((char*)buffer.data(), buffer.length());
		return write(sock, buff, error, uiTimeOut);
	}

	std::string getHostname(bool fqdn)
	{
		init_winsock();

		char const* hostname = "unknown";
		int ret;
		// The initial size is based on information in the Microsoft article
		// <https://msdn.microsoft.com/en-us/library/ms738527(v=vs.85).aspx>
		std::vector<char> hn(256, 0);

		while (true)
		{
			ret = ::gethostname(&hn[0], static_cast<int>(hn.size()) - 1);
			if (ret == 0)
			{
				hostname = &hn[0];
				break;
			}
			else
			{
				int const wsaeno = WSAGetLastError();
				if (wsaeno == WSAEFAULT && hn.size() <= 1024 * 8)
					// Out buffer was too short. Retry with buffer twice the size.
					hn.resize(hn.size() * 2, 0);
				else
				{
					OutputDebugString("Failed to get own hostname. Error: ");
					return std::string(hostname);
				}
			}
		}

		if (ret != 0 || (ret == 0 && !fqdn))
			return std::string(hostname);
		return std::string();
		/*ADDRINFOT addr_info_hints{};
		addr_info_hints.ai_family = AF_INET;
		// The AI_FQDN flag is available only on Windows 7 and later.
		#if defined (AI_FQDN)
		if (verifyWindowsVersionAtLeast(6, 1))
		addr_info_hints.ai_flags = AI_FQDN;
		else
		#endif
		addr_info_hints.ai_flags = AI_CANONNAME;

		std::unique_ptr<ADDRINFOT, ADDRINFOT_deleter> addr_info;
		ADDRINFOT * ai = nullptr;
		ret = GetAddrInfo(LOG4CPLUS_C_STR_TO_TSTRING(hostname).c_str(), nullptr,
		&addr_info_hints, &ai);
		if (ret != 0)
		{
		WSASetLastError(ret);
		helpers::getLogLog().error(
		LOG4CPLUS_TEXT("Failed to resolve own hostname. Error: ")
		+ convertIntegerToString(ret));
		return LOG4CPLUS_STRING_TO_TSTRING(hostname);
		}

		addr_info.reset(ai);
		return addr_info->ai_canonname;*/
	}

	int setTCPNoDelay(SOCKET sock, bool val, int& error)
	{
		int result;
		int enabled = static_cast<int>(val);
		if ((result = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
			reinterpret_cast<char*>(&enabled),
			sizeof(enabled))) != 0)
		{
			error = getLastSocketError();
		}
		return result;
	}

	int getPeerInfo(SOCKET skt, char* ip, int len, int* port)
	{
		int iRet, iLen;
		struct sockaddr_in name;
		iLen = (int)sizeof(struct sockaddr_in);
		iRet = getpeername(skt, (struct sockaddr*)&name, &iLen);
		if (iRet != SOCKET_ERROR)
		{
#ifdef WIN32
			//strncpy_s(ip, len, inet_ntoa(name.sin_addr), _TRUNCATE);
			char IPdotdec[64];
			IPdotdec[0] = '\0';
			inet_ntop(AF_INET, &name.sin_addr, IPdotdec, 64);
			strncpy_s(ip, len - 1, IPdotdec, _TRUNCATE);
#else
			snprintf(ip, len, "%s", inet_ntoa(name.sin_addr));
#endif
		}
		return iRet;
	}
}