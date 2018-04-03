#include "../utils/socket.h"
#include "../utils/error.h"
#include <atomic>
#include <memory>
#include <MSWSock.h>

//#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)

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
		if (ret == SOCKET_ERROR){
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

	AbstractSocket::AbstractSocket(PROTO_TYPE proto_type)
		: _sock(INVALID_SOCKET),
		_err(SOCKET_ERROR),
		_proto_type(proto_type)
	{
	}

	AbstractSocket::AbstractSocket(SOCKET sock, PROTO_TYPE proto_type)
		: _sock(sock),
		_err(SOCKET_ERROR),
		_proto_type(proto_type)
	{
	}

	AbstractSocket::~AbstractSocket(){
		Close();
	}

	void AbstractSocket::InitEnv() {
		init_winsock();
	}

	void AbstractSocket::UninitEnv() {
		WSACleanup();
	}

	int AbstractSocket::SetNoDelay() {
		if (!isOpen())
			return 0;
		int err = 0;
		SetTCPNoDelay(_sock, true, err);
		return 0;
	}

	void AbstractSocket::SetNoblock() {
		if (!isOpen())
			return;
		SetAsyncSkt(_sock);
	}

	void AbstractSocket::Close()
	{
		if (_sock != INVALID_SOCKET){
			::closesocket(_sock);
			_sock = INVALID_SOCKET;
		}
	}

	bool AbstractSocket::isOpen() const
	{
		return _sock != INVALID_SOCKET;
	}

	bool AbstractSocket::isError() const
	{
#ifdef WIN32
		return _err == SOCKET_ERROR;
#else
#endif
	}

	int AbstractSocket::GetSockName(sockaddr* name, int* len) const
	{
		return getSockName(_sock, name, len);
	}

	int AbstractSocket::DuplicateSocket(int iSocket, DWORD pid, BYTE* pProtocolInfo, int iBuffLen, int* pDataLen) {
#ifdef WIN32
		WSAPROTOCOL_INFO info = { 0 };
		int err = ::WSADuplicateSocket(iSocket, pid, &info);
		if (0 != err){
			return UTILS_ERROR_FAIL;
		}
		if (NULL != pProtocolInfo){
			memcpy(pProtocolInfo, &info, min(sizeof(WSAPROTOCOL_INFO), iBuffLen));
		}
		if (NULL != pDataLen)
		{
			*pDataLen = sizeof(WSAPROTOCOL_INFO);
		}
		return UTILS_ERROR_SUCCESS;
#else
#endif
	}

	int AbstractSocket::GetDuplicateSocket(BYTE* pProtocolInfo, int iDataLen, int* pSocket)
	{
#ifdef WIN32
		if (NULL == pProtocolInfo || NULL == pSocket){
			return UTILS_ERROR_PAR;
		}
		if (iDataLen < sizeof(WSAPROTOCOL_INFO)){
			return UTILS_ERROR_PAR;
		}
		LPWSAPROTOCOL_INFO p = (LPWSAPROTOCOL_INFO)pProtocolInfo;
		SOCKET socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, p, 0, 0);
		if (INVALID_SOCKET == socket){
			return UTILS_ERROR_MEMORY;
		}

		*pSocket = socket;
		return UTILS_ERROR_SUCCESS;
#else
#endif
	}

	int AbstractSocket::DuplicateSocket(DWORD pid, BYTE* pProtocolInfo, int iBuffLen, int* pDataLen) {
		return DuplicateSocket(_sock, pid, pProtocolInfo, iBuffLen, pDataLen);
	}

	int AbstractSocket::GetDuplicateSocket(BYTE* pProtocolInfo, int iDataLen) {
		return GetDuplicateSocket(pProtocolInfo, iDataLen,(int*)&_sock);
	}

	int AbstractSocket::read(char* pBuff, int iBuffLen,
		const int ciReadLen /*= 0*/,
		unsigned int uiTimeOut /*= 5000*/)
	{
		if (isError()) {
			return 0;
		}
		if (!isConnect()){
			return 0;
		}
		int len = UTILS::read(_sock, pBuff, iBuffLen, _err, false, nullptr, nullptr, ciReadLen, uiTimeOut);;
		return len;
	}

	int AbstractSocket::read_from(char* pBuff, int iBuffLen,
	struct sockaddr *from, int *fromlen,
		const int ciReadLen /*= 0*/,
		unsigned int uiTimeOut /*= 5000*/)
	{
		if (isError()) {
			return 0;
		}
		int len = UTILS::read(_sock, pBuff, iBuffLen, _err, true, from, fromlen, ciReadLen, uiTimeOut);
		return len;
	}
	int AbstractSocket::write(const char* pBuff, int iBuffLen,
		unsigned int uiTimeOut /*= 5000*/)
	{
		if (isError()) {
			return 0;
		}
		if (!isConnect()){
			return 0;
		}
		int len = UTILS::write(_sock, pBuff, iBuffLen, _err, uiTimeOut);
		return len;
	}
	int AbstractSocket::write_to(const char* pBuff, int iBuffLen,
	struct sockaddr *to, int tolen,
		unsigned int uiTimeOut /*= 5000*/)
	{
		if (isError()) {
			return 0;
		}
		int len = UTILS::write(_sock, pBuff, iBuffLen, _err, true, to, tolen, uiTimeOut);
		return len;
	}

	//////////////////////////////////////////////////////////////////////////////
	// Socket ctors and dtor
	//////////////////////////////////////////////////////////////////////////////

	ClientSocket::ClientSocket(PROTO_TYPE proto_type)
		: AbstractSocket(proto_type)
		, _Connected(false)
	{
		if (proto_type == PROTO_TYPE_UDP){
			_err = 0;
		}
	}


	ClientSocket::ClientSocket(PROTO_TYPE proto_type, const char* ip, unsigned short port)
		: AbstractSocket(proto_type)
		, _Connected(false)
	{
		Connect(ip, port);
	}


	ClientSocket::ClientSocket(SOCKET sock_, PROTO_TYPE proto_type)
		: AbstractSocket(sock_, proto_type)
		, _Connected(false)
	{
		if (isOpen()){
			_err = 0;
		}
	}


	ClientSocket::~ClientSocket()
	{ }


	bool ClientSocket::isConnect() const {
		if (isError()) {
			return false;
		}
		return _Connected;
	}

	void ClientSocket::Attach(SOCKET sock){
		if (sock != INVALID_SOCKET) {
			_sock = sock;
			_err = 0;
			_Connected = true;
		}
	}

	bool ClientSocket::Connect(const char* ip,unsigned short port,
		unsigned int uiTimeOut /*= 5000*/)
	{
		if (isConnect()){
			return true;
		}
		_sock = ConnectSocket(_proto_type, ip, port, _err, uiTimeOut);
		if (_sock == INVALID_SOCKET){
			return false;
		}
		if (_proto_type == PROTO_TYPE_TCP){
			int err = 0;
			SetTCPNoDelay(_sock, true, err);
		}
		if (!isError()) {
			_Connected = true;
		}
		if (!isConnect()){
			CloseSocket(_sock);
			_sock = INVALID_SOCKET;
			return false;
		}
		SetNoblock();
		return true;
	}

	bool ClientSocket::Init(const char* ip, unsigned short port)
	{
		if (_proto_type != PROTO_TYPE_UDP){
			return false;
		}
		_sock = OpenSocket(PROTO_TYPE_UDP, ip == nullptr ? std::string() : ip, port, _err);;
		DWORD dwBytesReturned = 0;
		BOOL bNewBehavior = FALSE;
		DWORD status = WSAIoctl(_sock, SIO_UDP_CONNRESET, &bNewBehavior, sizeof(bNewBehavior), NULL, 0, &dwBytesReturned, NULL, NULL);
		SetNoblock();
		return true;
	}

	ServerSocket::ServerSocket()
		: AbstractSocket(PROTO_TYPE_TCP)
	{
	}

	ServerSocket::ServerSocket(PROTO_TYPE proto_type)
		: AbstractSocket(proto_type)
	{
		if (_proto_type == PROTO_TYPE_UDP){
			_err = 0;
		}
	}

	ServerSocket::ServerSocket(PROTO_TYPE proto_type, unsigned short port,
		bool accept_block/* = true*/,
		const std::string& host/* = std::string()*/)
		: AbstractSocket(proto_type)
	{
		// Initialize these here so that we do not try to close invalid handles
		// in dtor if the following `openSocket()` fails.
		if (_proto_type == PROTO_TYPE_UDP){
			_err = 0;
		}
		Init(proto_type,nullptr, port);
	}

	ServerSocket::~ServerSocket()
	{
	}

	int ServerSocket::Init(PROTO_TYPE proto_type,const char* ip, unsigned short port)
	{
		_err = 0;
		_proto_type = proto_type;
		_sock = OpenSocket(_proto_type, ip == nullptr ? std::string() : ip, port, _err);
#ifdef WIN32
		uint32_t argp;
		argp = 1;
		ioctlsocket(_sock, FIONBIO, (u_long*)&argp);
#else
		int flags = fcntl(fdListen, F_GETFL, 0);
		fcntl(fdListen, F_SETFL, flags | O_NONBLOCK); //设置成非阻塞模式；
#endif
		return 0;
	}

	void ServerSocket::UnInit()
	{
		if (_sock != INVALID_SOCKET) {
			CloseSocket(_sock);
			_sock = INVALID_SOCKET;
		}
	}
	bool ServerSocket::Accept(ClientSocket& clt)
	{
		if (!isOpen()){
			return false;
		}
		SOCKET clientSock = AcceptSocket(_sock, _err);
		if (clientSock != INVALID_SOCKET) {
			clt.SetPtotoType(PROTO_TYPE_TCP);
			clt.Attach(clientSock);
			return true;
		}
		return false;
	}

	SOCKET ServerSocket::Accept() {
		if (!isOpen()) {
			return false;
		}
		return AcceptSocket(_sock, _err);
	}

	SOCKET OpenSocket(PROTO_TYPE proto_type, const std::string& host, unsigned short port, int& error)
	{
		ADDRINFOT addr_info_hints{};
		PADDRINFOT ai = nullptr;
		std::unique_ptr<ADDRINFOT, ADDRINFOT_deleter> addr_info;
		std::string port_str = convertIntegerToString(port);
		int retval;

		init_winsock();

		addr_info_hints.ai_family = AF_INET;
		if (proto_type == PROTO_TYPE_TCP){
			addr_info_hints.ai_socktype = SOCK_STREAM;
			addr_info_hints.ai_protocol = IPPROTO_TCP;
		}
		else if (proto_type == PROTO_TYPE_UDP){
			addr_info_hints.ai_socktype = SOCK_DGRAM;
			addr_info_hints.ai_protocol = IPPROTO_UDP;
		}
		else{
			error = SOCKET_ERROR;
			return INVALID_SOCKET;
		}

		addr_info_hints.ai_flags = AI_PASSIVE;
		retval = GetAddrInfo(host.empty() ? nullptr : host.c_str(),
			port_str.c_str(),&addr_info_hints,&ai);
		if (retval != 0){
			error = SOCKET_ERROR;
			return INVALID_SOCKET;
		}

		addr_info.reset(ai);

		SOCKET skt = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
		if (skt == INVALID_SOCKET){
			error = SOCKET_ERROR;
			return INVALID_SOCKET;
		}

		if (bind(skt, ai->ai_addr, static_cast<int>(ai->ai_addrlen)) != 0){
			error = SOCKET_ERROR;
			return skt;
		}

#if 0
		sockaddr_in src2;
		int len = sizeof(sockaddr);
		getsockname(skt, (sockaddr*)&src2, &len);
		sockaddr_in* p = (sockaddr_in*)&src2;
		char IPdotdec[64];
		IPdotdec[0] = '\0';
		inet_ntop(AF_INET, &p->sin_addr, IPdotdec, 64);
		std::cout << "xxxxx : "<<IPdotdec << std::endl;
		std::cout << p->sin_port << " | " << port_str.data()<< std::endl;
#endif

		if (proto_type == PROTO_TYPE_TCP){

			if (::listen(skt, 64) != 0) {
				error = SOCKET_ERROR;
				return skt;
			}
		}
		return skt;
	}

	SOCKET ConnectSocket(PROTO_TYPE proto_type, const std::string& hostn, unsigned short port,
		int& error, bool bNonBlock /*= true*/, unsigned int uiTimeOut /*= 5000*/)
	{
		ADDRINFOT addr_info_hints{};
		PADDRINFOT ai = nullptr;
		std::unique_ptr<ADDRINFOT, ADDRINFOT_deleter> addr_info;
		std::string port_str = convertIntegerToString(port);
		DWORD dwTimeOut;
		SOCKET skt = INVALID_SOCKET;
		bool bSuccess = false;
		timeval tm;
		fd_set rest, west;
		int retval = 0,iErr = 0, len = sizeof(int);

		init_winsock();

		addr_info_hints.ai_family = AF_INET;
		if (proto_type == PROTO_TYPE_TCP){
			addr_info_hints.ai_socktype = SOCK_STREAM;
			addr_info_hints.ai_protocol = IPPROTO_TCP;
		}
		else if (proto_type == PROTO_TYPE_UDP){
			addr_info_hints.ai_socktype = SOCK_DGRAM;
			addr_info_hints.ai_protocol = IPPROTO_UDP;
		}
		else{
			error = SOCKET_ERROR;
			return INVALID_SOCKET;
		}
		addr_info_hints.ai_flags = AI_NUMERICSERV;
		retval = GetAddrInfo(hostn.c_str(),port_str.c_str(),&addr_info_hints,&ai);
		if (retval != 0){
			error = SOCKET_ERROR;
			return INVALID_SOCKET;
		}

		addr_info.reset(ai);

		for (ADDRINFOT * rp = ai; rp; rp = rp->ai_next)
		{
#if 1 //print ip
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
#endif //print ip
			skt = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
			if (skt == INVALID_SOCKET){
				continue;
			}
			if (bNonBlock){
#ifdef WIN32
				uint32_t argp;
				argp = 1;
				ioctlsocket(skt, FIONBIO, (u_long*)&argp);
#else
				int flags = fcntl(fdListen, F_GETFL, 0);
				fcntl(fdListen, F_SETFL, flags | O_NONBLOCK); //设置成非阻塞模式；
#endif
			}
			dwTimeOut = GetTickCount();
			do
			{
				if (GetTickCount() - dwTimeOut > uiTimeOut) {
					bSuccess = false;
					break;
				}
				retval = ::connect(skt, rp->ai_addr, static_cast<int>(rp->ai_addrlen));
				if (retval != SOCKET_ERROR) {
					bSuccess = true;
					break;
				}
				else {
					int iError = WSAGetLastError();
					if (iError == WSAEISCONN) {
						bSuccess = true;
						break;
					}
				}
				Sleep(10);
			} while (true);
			if(bSuccess){
				break; 
			}
		}

		if (skt == INVALID_SOCKET){
			error = SOCKET_ERROR;
			return INVALID_SOCKET;
		}
		if (!bSuccess) {
			error = SOCKET_ERROR;
			return skt;
		}
		bSuccess = false;
		if (bNonBlock){
			dwTimeOut = GetTickCount();
			do
			{
				if (GetTickCount() - dwTimeOut > uiTimeOut) {
					bSuccess = false;
					break;
				}
				tm.tv_sec = uiTimeOut / 1000;
				tm.tv_usec = uiTimeOut % 1000;

				FD_ZERO(&rest);
				FD_ZERO(&west);
				FD_SET(skt, &rest);
				FD_SET(skt, &west);

				if (select(skt + 1, &rest, &west, NULL, &tm) > 0) {
					//如果套接口及可写也可读，需要进一步判断  
					if (FD_ISSET(skt, &rest) && FD_ISSET(skt, &west)) {
						iErr = -1;
						if (getsockopt(skt, SOL_SOCKET, SO_ERROR, (char*)&iErr, (socklen_t *)&len) > 0) {
							if (iErr == 0) {
								bSuccess = true;
								break;
							}
						}
					}
					//如果套接口可写不可读,则链接完成  
					else if (FD_ISSET(skt, &west) && !FD_ISSET(skt, &rest)) {
						bSuccess = true;
						break;
					}
				}
				Sleep(10);
			} while (true);
		}

		if (!bSuccess){
			error = SOCKET_ERROR;
			return skt;
		}
		error = 0;
		return skt;
	}

	SOCKET AcceptSocket(SOCKET sock, int& error)
	{
		init_winsock();
		SOCKET connected_socket = ::accept(sock, nullptr, nullptr);
		if (connected_socket == INVALID_SOCKET) {
			error = SOCKET_ERROR;
		}
		else {
			error = 0;
		}
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

	long read(SOCKET sock, char* pBuff, int iBuffLen, int& error, 
		bool recvfrom /*= false*/, struct sockaddr *from /*= nullptr*/, int *fromlen /*= nullptr*/, const int ciReadLen /*= 0*/, unsigned int uiTimeOut/* = 5000*/)
	{
		int iDataLen = 0, iReadLen = 0;
		BOOL bRecvData = TRUE;
		DWORD dwTimeOut = GetTickCount();
		fd_set readfd, excepfd;
		struct timeval select_time;

		if (recvfrom){
			if (from == nullptr || fromlen == nullptr){
				return 0;
			}
		}
		do
		{
			if ((GetTickCount() - dwTimeOut) > (int)uiTimeOut){
				break;
			}
			if (ciReadLen > 0 && ciReadLen - iDataLen <= 0){//已经读取到指定长度数据.
				OutputDebugString("read 333");
				break;
			}
			if (!bRecvData){
				FD_ZERO(&readfd);
				FD_SET(sock, &readfd);

				excepfd = readfd;

				select_time.tv_sec = 0;
				select_time.tv_usec = 100000;
				int iRet = select(sock + 1, &readfd, NULL, &excepfd, &select_time);
				if (iRet > 0){
					iRet = FD_ISSET(sock, &excepfd);
					if (iRet){//error
						error = SOCKET_ERROR;
						OutputDebugString("33333333333");
						break;
					}
					//是否有数据
					iRet = FD_ISSET(sock, &readfd);
					if (iRet){
						bRecvData = TRUE;
					}
				}
				else {//如果没有根据指定读取数据大小,则读取到多少数据就返回.
					if (iDataLen > 0 && ciReadLen <= 0){
						OutputDebugString("4444444444444444");
						break;
					}
				}
			}
			else {
				if (recvfrom){
					iReadLen = ::recvfrom(sock, pBuff + iDataLen, ciReadLen > 0 ? ciReadLen - iDataLen : iBuffLen, 0, from, fromlen);
				}
				else{
					iReadLen = ::recv(sock, pBuff + iDataLen, ciReadLen > 0 ? ciReadLen - iDataLen : iBuffLen, 0);
				}
				
				bRecvData = FALSE;
				if (iReadLen > 0){
					iDataLen += iReadLen;
				}
				if (iReadLen == 0){//Server CloseSocket
					error = SOCKET_ERROR;
					OutputDebugString("1111111111");
					break;
				}
				if (iReadLen == SOCKET_ERROR){
#ifdef WIN32
					int iError = WSAGetLastError();
					if (iError == WSAEWOULDBLOCK){
						if (iDataLen > 0 && ciReadLen <= 0)
						{//如果没有根据指定读取数据大小
							break;
						}
						continue;
					}
#else
					if (errno == EAGAIN)//非阻塞的模式,所以当errno为EAGAIN时,表示当前缓冲区已无数据可读
					{
						if (iReadLen > 0){
							break;
						}
						continue;
					}
					iError = errno;
#endif
					char log[64];
					_snprintf_s(log, 64, "WSAGetLastError:%d", iError);
					OutputDebugString(log);
					error = SOCKET_ERROR;
					break;
				}
			}

		} while (iDataLen < iBuffLen);

		return iDataLen;
	}

	long write(SOCKET sock, const char* pBuff, int iBuffLen, int& error,
		bool sendto /*= false*/,struct sockaddr *to /*= nullptr*/, int tolen /*= 0*/, unsigned int uiTimeOut/* = 5000*/)
	{
		int iSendLen = 0, iError = 0, ret = 0;
		BOOL bSendData = TRUE;
		DWORD dwTimeOut = GetTickCount();
		fd_set writefd, excepfd;
		struct timeval select_time;
		if (sendto){
			if (to == nullptr || tolen <= 0){
				return 0;
			}
		}
		do
		{
			if ((GetTickCount() - dwTimeOut) > (int)uiTimeOut){
				break;
			}

			if (!bSendData){
				FD_ZERO(&writefd);
				FD_SET(sock, &writefd);

				excepfd = writefd;

				select_time.tv_sec = 0;
				select_time.tv_usec = 100000;
				int iRet = select(sock + 1, NULL, &writefd, &excepfd, &select_time);
				if (iRet > 0){
					iRet = FD_ISSET(sock, &excepfd);
					if (iRet){//error
						error = SOCKET_ERROR;
						break;
					}
					//是否有数据
					iRet = FD_ISSET(sock, &writefd);
					if (iRet){
						bSendData = TRUE;
					}
				}
			}
			else {
				if (sendto){
					ret = ::sendto(sock, pBuff + iSendLen, iBuffLen - iSendLen, 0, to, tolen);
				}
				else{
					ret = ::send(sock, pBuff + iSendLen, iBuffLen - iSendLen, 0);
				}
				
				bSendData = FALSE;
				if (ret > 0){
					iSendLen += ret;
				}

				if (ret == SOCKET_ERROR){
#ifdef WIN32
					iError = WSAGetLastError();
					if (iError == WSAEWOULDBLOCK){
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
					char log[64];
					_snprintf_s(log, 64, "WSAGetLastError:%d", iError);
					OutputDebugString(log);
					error = SOCKET_ERROR;
					break;
				}

			}

		} while (iSendLen < iBuffLen);

		return iSendLen;
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

	int SetTCPNoDelay(SOCKET sock, bool val, int& error)
	{
		int result=0,enabled = static_cast<int>(val);
		if ((result = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
			reinterpret_cast<char*>(&enabled),
			sizeof(enabled))) != 0)
		{
			error = getLastSocketError();
		}
		return result;
	}

	void SetAsyncSkt(SOCKET sock)
	{
		if (sock != INVALID_SOCKET) {
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
	int getSockName(SOCKET skt, sockaddr* name, int* iLen)
	{
		int iRet = getsockname(skt, name, iLen);
		return iRet;
	}
}