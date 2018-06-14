#include "../utils/net.h"
#include "../utils/error.h"
#include "internal.h"
#include <atomic>
#include <memory>
#include <MSWSock.h>

std::atomic_flag lock = ATOMIC_FLAG_INIT;
/**
* @brief 初始化启动进程使用Winsock DLL。
*	原子操作.
*/
void _InitSocket(){
	if (!lock.test_and_set()){
		WSADATA wsa;
		int ret = WSAStartup(MAKEWORD(2, 2), &wsa);
		if (ret != 0){
			lock.clear();
		}
	}
}
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

/**
* @brief UDP连接被重置错误
* 
* 在Windows 7系统上,A使用UDP socket,调用sendto函数向一个目标地址B发送数据,
* 但是目标地址B没有接收数据,
* 如果A此时立即调用recvfrom试图接收目标地址B发回的数据的话,
* recvfrom会立即返回-1，WSAGetLastError()返回10045.
*/
void UdpConnResetError(SOCKET skt)
{
	if (skt == INVALID_SOCKET){
		return;
	}
	DWORD dwBytesReturned = 0;
	BOOL bNewBehavior = FALSE;
	DWORD status = WSAIoctl(skt, SIO_UDP_CONNRESET, &bNewBehavior, sizeof(bNewBehavior), NULL, 0, &dwBytesReturned, NULL, NULL);
}

namespace UTILS{
	namespace NET{
		int UTILS_API InitSocket()
		{
			_InitSocket();
			return 0;
		}
		int UTILS_API UnInitSocket()
		{
			WSACleanup();
			return 0;
		}
		int UTILS_API CloseSocket(SOCKET skt)
		{
			if (skt != INVALID_SOCKET){
				closesocket(skt);
			}
			return 0;
		}
		SOCKET OpenSocket(TRANS_PROTOCOL_TYPE nType, const char* host, unsigned short port, int* error /*= nullptr*/)
		{
			ADDRINFOT stAddrInfoInts{};
			PADDRINFOT ai = nullptr;
			std::unique_ptr<ADDRINFOT, ADDRINFOT_deleter> plstAddrInfo;
			char szPort[32];
			_snprintf_s(szPort, _TRUNCATE, "%d", port);
			int iRetVal=0;

			stAddrInfoInts.ai_family = AF_INET;
			switch (nType)
			{
			case UTILS::NET::TRANS_PROTOCOL_TYPE_TCP:
				stAddrInfoInts.ai_socktype = SOCK_STREAM;
				stAddrInfoInts.ai_protocol = IPPROTO_TCP;
				break;
			case UTILS::NET::TRANS_PROTOCOL_TYPE_UDP:
				stAddrInfoInts.ai_socktype = SOCK_DGRAM;
				stAddrInfoInts.ai_protocol = IPPROTO_UDP;
				break;
			default:
				return INVALID_SOCKET;
			}

			//AI_PASSIVE  - 当hostName为NULL时，给出ADDR_ANY和IN6ADDR_ANY_INIT
			stAddrInfoInts.ai_flags = AI_PASSIVE;
			iRetVal = GetAddrInfo(host, szPort, &stAddrInfoInts, &ai);
			if (iRetVal != 0){
				MSG_INFO("ERROR err:%d ERR:%d LINE:%d", iRetVal, WSAGetLastError(), __LINE__);
				if (nullptr != error){
					*error = SOCKET_ERROR;
				}
				return INVALID_SOCKET;
			}

			plstAddrInfo.reset(ai);

#if 1
			SOCKET skt = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
#else
			SOCKET skt = WSASocketW(ai->ai_family, ai->ai_socktype, ai->ai_protocol, nullptr,
				0, WSA_FLAG_NO_HANDLE_INHERIT)
#endif
			if (skt == INVALID_SOCKET){
				if (nullptr != error){
					*error = SOCKET_ERROR;
				}
				return INVALID_SOCKET;
			}
			if (bind(skt, ai->ai_addr, static_cast<int>(ai->ai_addrlen)) != 0){
				MSG_INFO("ERROR ERR:%d LINE:%d", WSAGetLastError(), __LINE__);
				closesocket(skt);
				if (nullptr != error){
					*error = SOCKET_ERROR;
				}
				return INVALID_SOCKET;
			}

			if ((nType == UTILS::NET::TRANS_PROTOCOL_TYPE_TCP) && ::listen(skt, 16) != 0){
				MSG_INFO("ERROR ERR:%d LINE:%d", WSAGetLastError(), __LINE__);
				closesocket(skt);
				if (nullptr != error){
					*error = SOCKET_ERROR;
				}
				return INVALID_SOCKET;
			}
			if (nullptr != error){
				*error = 0;
			}
			return skt;
		}

		SOCKET ConnectSocket(TRANS_PROTOCOL_TYPE nType, const char* host, unsigned short port, int* error /*= nullptr*/, bool bConn /*= true*/)
		{
			ADDRINFOT stAddrInfoInts{};
			PADDRINFOT ai = nullptr;
			std::unique_ptr<ADDRINFOT, ADDRINFOT_deleter> plstAddrInfo;
			char szPort[32];
			_snprintf_s(szPort, _TRUNCATE, "%d", port);
			int iRetVal = 0;

			stAddrInfoInts.ai_family = AF_INET;
			switch (nType)
			{
			case UTILS::NET::TRANS_PROTOCOL_TYPE_TCP:
				stAddrInfoInts.ai_socktype = SOCK_STREAM;
				stAddrInfoInts.ai_protocol = IPPROTO_TCP;
				break;
			case UTILS::NET::TRANS_PROTOCOL_TYPE_UDP:
				stAddrInfoInts.ai_socktype = SOCK_DGRAM;
				stAddrInfoInts.ai_protocol = IPPROTO_UDP;
				break;
			default:
				MSG_INFO("ERROR LINE:%d", __LINE__);
				return INVALID_SOCKET;
			}

			stAddrInfoInts.ai_flags = AI_NUMERICSERV;
			iRetVal = GetAddrInfo(host, szPort, &stAddrInfoInts, &ai);
			if (iRetVal != 0){
				MSG_INFO("ERROR ERR:%d LINE:%d", WSAGetLastError(), __LINE__);
				if (nullptr != error){
					*error = SOCKET_ERROR;
				}
				return INVALID_SOCKET;
			}

			plstAddrInfo.reset(ai);
			iRetVal = 0;
			SOCKET skt = INVALID_SOCKET;
			for (ADDRINFOT * rp = ai; rp; rp = rp->ai_next)
			{
#if 1
				skt = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
#else
				skt = WSASocketW(rp->ai_family, rp->ai_socktype, rp->ai_protocol,
					nullptr, 0, WSA_FLAG_NO_HANDLE_INHERIT);
#endif
				if (skt == INVALID_SOCKET)
					continue;

				if (!bConn){
					break;
				}
				while (((iRetVal = ::connect(skt, rp->ai_addr, static_cast<int>(rp->ai_addrlen))) == SOCKET_ERROR) && (WSAGetLastError() == WSAEINTR)){
					MSG_INFO("connect LINE:%d", __LINE__);
				}
				if (iRetVal == SOCKET_ERROR){
					MSG_INFO("ERROR ERR:%d LINE:%d",WSAGetLastError(), __LINE__);
					closesocket(skt);
					if (nullptr != error){
						*error = SOCKET_ERROR;
					}
					break;
				}
			}
			if ((iRetVal == 0) && (skt != INVALID_SOCKET)){
				if (nullptr != error){
					*error = 0;
				}
			}
			else{
				MSG_INFO("ERROR ERR:%d LINE:%d", WSAGetLastError(), __LINE__);
			}
			return skt;
		}
		
		int UTILS_API DuplicateSocket(SOCKET iSocket, DWORD pid, BYTE* pProtocolInfo, int iBuffLen, int* pDataLen) {
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

		int UTILS_API CreateSocketFromDuplicate(BYTE* pProtocolInfo, int iDataLen, SOCKET& skt)
		{
#ifdef WIN32
			if (NULL == pProtocolInfo){
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
			skt = socket;
			return UTILS_ERROR_SUCCESS;
#else
#endif
		}

		int UTILS_API GetSktPeerInfo(SOCKET skt, char* ip, int len, int* port)
		{
			int iRet, iLen;
			struct sockaddr_in name;
			iLen = (int)sizeof(struct sockaddr_in);
			iRet = getpeername(skt, (struct sockaddr*)&name, &iLen);
			if (iRet != SOCKET_ERROR)
			{
				if (port != NULL){
					*port = ntohs(name.sin_port);
				}
				char szIP[64];
				szIP[0] = '\0';
				inet_ntop(AF_INET, &name.sin_addr, szIP, 64);
				if (nullptr != ip){
					strncpy_s(ip, len - 1, szIP, min(_TRUNCATE, (len - 1 < 0 ? 0 : len - 1)));
				}
			}
			return iRet;
		}

		int UTILS_API GetSktName(SOCKET skt, char* ip, int len, int* port)
		{
			int iRet, iLen;
			struct sockaddr_in name;
			iLen = (int)sizeof(struct sockaddr_in);
			iRet = getsockname(skt, (struct sockaddr*)&name, &iLen);
			if (iRet != SOCKET_ERROR)
			{
				if (port != NULL){
					*port = ntohs(name.sin_port);
				}
				char szIP[64];
				szIP[0] = '\0';
				inet_ntop(AF_INET, &name.sin_addr, szIP, 64);
				if (ip != nullptr){
					strncpy_s(ip, len - 1, szIP, min(_TRUNCATE, (len - 1 < 0 ? 0 : len - 1)));
				}
			}
			return iRet;
		}

		int UTILS_API GetSktNameEx(sockaddr& addr, char* ip, int len, int* port)
		{
			sockaddr_in* pInfo = (sockaddr_in*)&addr;
			if (port != NULL){
				*port = ntohs(pInfo->sin_port);
			}
			char szIP[64];
			szIP[0] = '\0';
			inet_ntop(AF_INET, &pInfo->sin_addr, szIP, 64);
			if (ip != nullptr){
				strncpy_s(ip, len - 1, szIP, min(_TRUNCATE, (len - 1 < 0 ? 0 : len - 1)));
			}
			return 0;
		}

		int UTILS_API StuffSockAddr(TRANS_PROTOCOL_TYPE nType, char* ip, int port, sockaddr& addr)
		{
			ADDRINFOT stAddrInfoInts{};
			PADDRINFOT ai = nullptr;
			std::unique_ptr<ADDRINFOT, ADDRINFOT_deleter> plstAddrInfo;
			char szPort[32];
			_snprintf_s(szPort, _TRUNCATE, "%d", port);
			int iRetVal = 0;

			stAddrInfoInts.ai_family = AF_INET;
			switch (nType)
			{
			case UTILS::NET::TRANS_PROTOCOL_TYPE_TCP:
				stAddrInfoInts.ai_socktype = SOCK_STREAM;
				stAddrInfoInts.ai_protocol = IPPROTO_TCP;
				break;
			case UTILS::NET::TRANS_PROTOCOL_TYPE_UDP:
				stAddrInfoInts.ai_socktype = SOCK_DGRAM;
				stAddrInfoInts.ai_protocol = IPPROTO_UDP;
				break;
			default:
				return UTILS_ERROR_PAR;
			}

			stAddrInfoInts.ai_flags = AI_PASSIVE;
			iRetVal = GetAddrInfo(ip, szPort, &stAddrInfoInts, &ai);
			if (iRetVal != 0){
				MSG_INFO("ERROR err:%d ERR:%d LINE:%d", iRetVal, WSAGetLastError(), __LINE__);
				return INVALID_SOCKET;
			}

			plstAddrInfo.reset(ai);
			memcpy(&addr, ai->ai_addr, min(static_cast<int>(ai->ai_addrlen), sizeof(sockaddr)));
			return UTILS_ERROR_FAIL;
		}

		namespace SELECT{

			unsigned int ReadFromTcp(SOCKET skt, char* pBuff, int iBuffLen, const int iReadLen/* = 0*/, int* error /*= nullptr*/, unsigned int uiTimeOut /*= 5000*/)
			{
				int iDataLen = 0, iRet = 0, iError =0;
				BOOL bRecvData = FALSE; //是否有数据可以接收
				DWORD dwTimeOut = GetTickCount();
				fd_set fdRead, fdExcep;
				struct timeval stTimeOut;
				if (nullptr == pBuff || iBuffLen <= 0){
					return UTILS_ERROR_PAR;
				}
				do
				{
					//如果没有根据指定读取数据大小,则读取到多少数据就返回.
					if (iReadLen <= 0){
						if (iDataLen > 0){
							break;
						}
					}
					else{
						//已经读取到指定大小的数据
						if (iReadLen - iDataLen <= 0){
							break;
						}
					}

					if (!bRecvData){
						FD_ZERO(&fdRead);
						FD_ZERO(&fdExcep);
						FD_SET(skt, &fdRead);
						FD_SET(skt, &fdExcep);

						stTimeOut.tv_sec = 0;
						stTimeOut.tv_usec = 10000;
						iRet = select(-1, &fdRead, NULL, &fdExcep, &stTimeOut);
						if (iRet > 0){
							iRet = FD_ISSET(skt, &fdExcep);
							if (iRet){//error
								iError = SOCKET_ERROR;
								MSG_INFO("SOCKET err:%d LINE:%d", WSAGetLastError(), __LINE__);
								break;
							}
							//是否有数据
							iRet = FD_ISSET(skt, &fdRead);
							if (iRet){
								bRecvData = TRUE;
							}
						}
					}
					else {
						iRet = ::recv(skt, pBuff + iDataLen, iReadLen > 0 ? iReadLen - iDataLen : iBuffLen, 0);
						bRecvData = FALSE;
						if (iRet > 0){
							iDataLen += iRet;
						}
						if (iRet == 0){//Server CloseSocket
							iError = SOCKET_ERROR;
							MSG_INFO("SOCKET err LINE:%d", __LINE__);
							break;
						}
						if (iRet == SOCKET_ERROR){
							int iErr = WSAGetLastError();
							if (iErr != WSAEWOULDBLOCK){//没有数据
								iError = SOCKET_ERROR;
								MSG_INFO("SOCKET err:%d LINE:%d", WSAGetLastError(), __LINE__);
								break;
							}
						}
					}

					if ((!bRecvData) && abs((int)(GetTickCount() - dwTimeOut)) >= (int)uiTimeOut){
						//超时退出
						break;
					}

				} while (iDataLen < iBuffLen);
				if (nullptr != error){
					*error = iError;
				}
				return iDataLen;
			}

			unsigned int ReadFromUdp(SOCKET skt, char* pBuff, int iBuffLen, struct sockaddr& from, int fromlen, int* error /*= nullptr*/, unsigned int uiTimeOut /*= 5000*/)
			{
				int iDataLen = 0, iRet = 0, iError = 0;
				BOOL bRecvData = FALSE; //是否有数据可以接收
				DWORD dwTimeOut = GetTickCount();
				fd_set fdRead, fdExcep;
				struct timeval stTimeOut;

				if (nullptr == pBuff || iBuffLen <= 0){
					return UTILS_ERROR_PAR;
				}
				do
				{
					if (iDataLen > 0){
						//读取到数据直接返回.
						break;
					}

					if (!bRecvData){
						FD_ZERO(&fdRead);
						FD_ZERO(&fdExcep);
						FD_SET(skt, &fdRead);
						FD_SET(skt, &fdExcep);

						stTimeOut.tv_sec = 0;
						stTimeOut.tv_usec = 10000;
						iRet = select(-1, &fdRead, NULL, &fdExcep, &stTimeOut);
						if (iRet > 0){
							iRet = FD_ISSET(skt, &fdExcep);
							if (iRet){//error
								iError = SOCKET_ERROR;
								MSG_INFO("SOCKET err:%d LINE:%d", WSAGetLastError(), __LINE__);
								break;
							}
							//是否有数据
							iRet = FD_ISSET(skt, &fdRead);
							if (iRet){
								bRecvData = TRUE;
							}
						}
					}
					else {
						iRet = ::recvfrom(skt, pBuff, iBuffLen, 0, &from, &fromlen);
						bRecvData = FALSE;
						if (iRet > 0){
							iDataLen = iRet;
						}
						if (iRet == 0){//Server CloseSocket
							iError = SOCKET_ERROR;
							MSG_INFO("SOCKET err:%d LINE:%d", WSAGetLastError(), __LINE__);
							break;
						}
						if (iRet == SOCKET_ERROR){
							int iErr = WSAGetLastError();
							if (iErr != WSAEWOULDBLOCK){//没有数据
								iError = SOCKET_ERROR;
								MSG_INFO("SOCKET err:%d LINE:%d", WSAGetLastError(), __LINE__);
								break;
							}
						}
					}
					if ((!bRecvData) && abs((int)(GetTickCount() - dwTimeOut)) >= (int)uiTimeOut){
						//超时退出
						break;
					}

				} while (iDataLen < iBuffLen);
				if (nullptr != error){
					*error = iError;
				}
				return iDataLen;
			}

			unsigned int WriteFromTcp(SOCKET skt, const char* pBuff, int iBuffLen, int* error /*= nullptr*/, unsigned int uiTimeOut/* = 5000*/)
			{
				int iSendLen = 0, iRet = 0, iError = 0;
				BOOL bSendData = TRUE; //是否可以发送数据
				DWORD dwTimeOut = GetTickCount();
				fd_set fdWrite, fdExcep;
				struct timeval stTimeOut;

				if (nullptr == pBuff || iBuffLen <= 0){
					return UTILS_ERROR_PAR;
				}
				do
				{
					if (iBuffLen - iSendLen <= 0){
						//数据发送完成.
						break;
					}

					if (!bSendData){
						FD_ZERO(&fdWrite);
						FD_ZERO(&fdExcep);
						FD_SET(skt, &fdWrite);
						FD_SET(skt, &fdExcep);

						stTimeOut.tv_sec = 0;
						stTimeOut.tv_usec = 10000;
						int iRet = select(-1, NULL, &fdWrite, &fdExcep, &stTimeOut);
						if (iRet > 0){
							iRet = FD_ISSET(skt, &fdExcep);
							if (iRet){//error
								iError = SOCKET_ERROR;
								MSG_INFO("SOCKET err:%d LINE:%d", WSAGetLastError(), __LINE__);
								break;
							}
							//是否有数据
							iRet = FD_ISSET(skt, &fdWrite);
							if (iRet){
								bSendData = TRUE;
							}
						}
					}
					else {
						iRet = ::send(skt, pBuff + iSendLen, iBuffLen - iSendLen, 0);
						bSendData = FALSE;
						if (iRet > 0){
							iSendLen += iRet;
						}
						if (iRet == SOCKET_ERROR){
							int iErr = WSAGetLastError();
							if (iErr != WSAEWOULDBLOCK){
								iError = SOCKET_ERROR;
								MSG_INFO("SOCKET err:%d LINE:%d", WSAGetLastError(), __LINE__);
								break;
							}
						}
					}

					if ((!bSendData) && abs((int)(GetTickCount() - dwTimeOut)) >= (int)uiTimeOut){
						//超时退出
						break;
					}
				} while (iSendLen < iBuffLen);
				if (nullptr != error){
					*error = iError;
				}
				return iSendLen;
			}

			unsigned int WriteFromUDP(SOCKET skt, const char* pBuff, int iBuffLen, struct sockaddr& to, int tolen, int* error /*= nullptr*/, unsigned int uiTimeOut /*= 5000*/)
			{
				int iSendLen = 0, iRet = 0, iError = 0;
				BOOL bSendData = TRUE; //是否可以发送数据
				DWORD dwTimeOut = GetTickCount();
				fd_set fdWrite, fdExcep;
				struct timeval stTimeOut;

				if (nullptr == pBuff || iBuffLen <= 0){
					return UTILS_ERROR_PAR;
				}
				do
				{
					if (iBuffLen - iSendLen <= 0){
						//数据发送完成.
						break;
					}

					if (!bSendData){
						FD_ZERO(&fdWrite);
						FD_ZERO(&fdExcep);
						FD_SET(skt, &fdWrite);
						FD_SET(skt, &fdExcep);

						stTimeOut.tv_sec = 0;
						stTimeOut.tv_usec = 10000;
						int iRet = select(-1, NULL, &fdWrite, &fdExcep, &stTimeOut);
						if (iRet > 0){
							iRet = FD_ISSET(skt, &fdExcep);
							if (iRet){//error
								iError = SOCKET_ERROR;
								MSG_INFO("SOCKET err:%d LINE:%d", WSAGetLastError(), __LINE__);
								break;
							}
							//是否有数据
							iRet = FD_ISSET(skt, &fdWrite);
							if (iRet){
								bSendData = TRUE;
							}
						}
					}
					else {
						iRet = ::sendto(skt, pBuff + iSendLen, iBuffLen - iSendLen, 0, &to, tolen);
						bSendData = FALSE;
						if (iRet > 0){
							iSendLen += iRet;
						}
						if (iRet == SOCKET_ERROR){
							int iErr = WSAGetLastError();
							if (iErr != WSAEWOULDBLOCK){
								iError = SOCKET_ERROR;
								MSG_INFO("SOCKET err:%d LINE:%d", WSAGetLastError(), __LINE__);
								break;
							}
						}
					}
					if ((!bSendData) && abs((int)(GetTickCount() - dwTimeOut)) >= (int)uiTimeOut){
						//超时退出
						break;
					}

				} while (iSendLen < iBuffLen);
				if (nullptr != error){
					*error = iError;
				}
				return iSendLen;
			}

			CNet::CNet()
				:m_nTransProType(TRANS_PROTOCOL_TYPE_NONE), m_ErrorCode(0), m_Skt(INVALID_SOCKET)
			{
				_InitSocket();
			}
			CNet::CNet(UTILS::NET::TRANS_PROTOCOL_TYPE nType)
				: m_nTransProType(nType), m_ErrorCode(0), m_Skt(INVALID_SOCKET)
			{

			}
			CNet::CNet(SOCKET skt, UTILS::NET::TRANS_PROTOCOL_TYPE nType)
				: m_nTransProType(nType), m_ErrorCode(0), m_Skt(skt)
			{

			}

			CNet::~CNet()
			{

			}

			void CNet::Close()
			{
				if (m_Skt != INVALID_SOCKET){
					::closesocket(m_Skt);
					//shutdown(_sock, SD_BOTH);
					m_Skt = INVALID_SOCKET;
				}
			}

			bool CNet::IsValidSkt() const
			{
				return m_Skt != INVALID_SOCKET;
			}

			bool CNet::IsError() const
			{
				return (m_ErrorCode != 0) || (m_Skt == INVALID_SOCKET);
			}

			UTILS::NET::TRANS_PROTOCOL_TYPE CNet::GetTransProtocol() const
			{
				return m_nTransProType;
			}

			int CNet::SetSktOpt(int level, int optname, const char* optval, int optlen)
			{
				return setsockopt(m_Skt, level, optname, optval, optlen);
			}

			int CNet::GetSktOpt(int level, int optname, char* optval, int *optlen)
			{
				return getsockopt(m_Skt, level, optname, optval, optlen);
			}

			int CNet::GetPeerAddrInfo(struct sockaddr& stAddr)
			{
				return UTILS_ERROR_NOSUPPOERT;
			}

			void CNet::SetErrorCode(int iErrorCode, int line)
			{
				m_ErrorCode = iErrorCode;
				if (m_ErrorCode != 0){
					MSG_INFO("SOCKET err:%d win.err:%d LINE:%d", m_ErrorCode, WSAGetLastError(), line);
				}
			}

			int CNet::SetSktNoBlock()
			{
				uint32_t argp;
				argp = 1;
				int err = ioctlsocket(m_Skt, FIONBIO, (u_long*)&argp);
				if (err != 0){
					SetErrorCode(SOCKET_ERROR,__LINE__);
				}
				return 0;
			}

			int CNet::SetTcpNoDelay()
			{
				if (m_nTransProType == UTILS::NET::TRANS_PROTOCOL_TYPE_TCP){
					int err = 0, v = static_cast<int>(true);
					if ((err = setsockopt(m_Skt, IPPROTO_TCP, TCP_NODELAY,reinterpret_cast<char*>(&v),sizeof(v))) != 0){
						SetErrorCode(SOCKET_ERROR, __LINE__);
					}
					return err;
				}
				return 0;
			}

			int CNet::SetSktSendBufferSize(unsigned int uiSize)
			{
				int err = setsockopt(m_Skt, SOL_SOCKET, SO_SNDBUF, (const char*)&uiSize, sizeof(int));
				if (err != 0){
					SetErrorCode(SOCKET_ERROR, __LINE__);
				}
				return err;
			}

			int CNet::SetSktRecvBufferSize(unsigned int uiSize)
			{
				int err = setsockopt(m_Skt, SOL_SOCKET, SO_RCVBUF, (const char*)&uiSize, sizeof(int));
				if (err != 0){
					SetErrorCode(SOCKET_ERROR, __LINE__);
				}
				return err;
			}

			int CNet::SetSktCloseLinger(unsigned int uiTime)
			{
				linger Linger;
				int err = 0;
				Linger.l_onoff = (uiTime > 0 ? 1 : 0);
				Linger.l_linger = uiTime;
				err = setsockopt(m_Skt, SOL_SOCKET, SO_LINGER, (char*)&Linger, sizeof(Linger));
				if (err != 0){
					SetErrorCode(SOCKET_ERROR, __LINE__);
				}
				return err;
			}

			int CNet::SetSktReuseAddr(bool bReuse)
			{
				int err = 0, v = static_cast<int>(bReuse);
				if ((err = setsockopt(m_Skt, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&v), sizeof(v))) != 0){
					SetErrorCode(SOCKET_ERROR, __LINE__);
				}
				return err;
			}

			unsigned int CNet::Read(char* pBuff, int iBuffLen, const int iReadLen /*= 0*/, unsigned int uiTimeOut /*= 5000*/)
			{
				int iError = UTILS_ERROR_PAR, iErrorCode =0;
				switch (m_nTransProType)
				{
				case UTILS::NET::TRANS_PROTOCOL_TYPE_NONE:
					break;
				case UTILS::NET::TRANS_PROTOCOL_TYPE_TCP:
					iError = ReadFromTcp(m_Skt, pBuff, iBuffLen, iReadLen, &iErrorCode, uiTimeOut);
					break;
				case UTILS::NET::TRANS_PROTOCOL_TYPE_UDP:{
					struct sockaddr stAddr;
					iError = ReadFromUdp(m_Skt, pBuff, iBuffLen, stAddr, sizeof(struct sockaddr), &iErrorCode, uiTimeOut);
				}
					break;
				default:
					break;
				}
				if (iErrorCode != 0){
					SetErrorCode(iErrorCode, __LINE__);
				}
				return iError;
			}

			unsigned int CNet::ReadFromUDP(char* pBuff, int iBuffLen, struct sockaddr& from, int fromlen, unsigned int uiTimeOut /*= 5000*/)
			{
				int iErrorCode = 0;
				int err = ReadFromUdp(m_Skt, pBuff, iBuffLen, from, fromlen, &iErrorCode, uiTimeOut);
				if (iErrorCode != 0){
					SetErrorCode(iErrorCode, __LINE__);
				}
				return err;
			}

			unsigned int CNet::Write(const char* pBuff, int iBuffLen, unsigned int uiTimeOut /*= 5000*/)
			{
				int iErrorCode = 0;
				unsigned int len = WriteFromTcp(m_Skt, pBuff, iBuffLen, &iErrorCode, uiTimeOut);
				if (iErrorCode != 0){
					SetErrorCode(iErrorCode, __LINE__);
				}
				return len;
			}

			unsigned int CNet::WriteToUDP(const char* pBuff, int iBuffLen, struct sockaddr& to, int tolen, unsigned int uiTimeOut /*= 5000*/)
			{
				int iErrorCode = 0;
				unsigned int len = WriteFromUDP(m_Skt, pBuff, iBuffLen, to, tolen, &iErrorCode, uiTimeOut);
				if (iErrorCode != 0){
					SetErrorCode(iErrorCode, __LINE__);
				}
				return len;
			}


			CNetClient::CNetClient():CNet(){

			}
			CNetClient::CNetClient(UTILS::NET::TRANS_PROTOCOL_TYPE nType)
				:CNet(nType)
			{

			}
			CNetClient::CNetClient(SOCKET skt, UTILS::NET::TRANS_PROTOCOL_TYPE nType)
				: CNet(skt,nType)
			{

			}

			CNetClient::~CNetClient()
			{

			}

			int CNetClient::ConnectSocket(TRANS_PROTOCOL_TYPE nType, const char* host, unsigned short port, int* error /*= nullptr*/, bool bConn /*= true*/)
			{
				if (nType != m_nTransProType){
					m_nTransProType = nType;
				}
				SetErrorCode(0,__LINE__);
				int iErrorCode = 0;
				SOCKET skt = UTILS::NET::ConnectSocket(m_nTransProType, host, port, &iErrorCode, bConn);
				if (iErrorCode != 0){
					SetErrorCode(iErrorCode, __LINE__);
					return -1;
				}
				m_Skt = skt;
				return 0;
			}

			void CNetClient::Attach(SOCKET skt)
			{
				SetErrorCode(0, __LINE__);
				m_nTransProType = TRANS_PROTOCOL_TYPE_TCP;
				if (m_Skt != INVALID_SOCKET){
					Close();
				}
				m_Skt = skt;
			}

			CNetServer::CNetServer() :CNet(){

			}
			CNetServer::CNetServer(UTILS::NET::TRANS_PROTOCOL_TYPE nType)
				:CNet(nType)
			{

			}

			CNetServer::~CNetServer()
			{

			}

			int CNetServer::OpenSocket(TRANS_PROTOCOL_TYPE nType, const char* host, unsigned short port, int* error /*= nullptr*/)
			{
				if (nType != m_nTransProType){
					m_nTransProType = nType;
				}
				SetErrorCode(0, __LINE__);
				int iErrorCode = 0;
				SOCKET skt = UTILS::NET::OpenSocket(m_nTransProType, host, port, &iErrorCode);
				if (iErrorCode != 0){
					SetErrorCode(iErrorCode, __LINE__);
					return -1;
				}
				m_Skt = skt;
				return 0;
			}

			bool CNetServer::Accept(CNetClient*& pClientIns)
			{
				SOCKET skt = ::accept(m_Skt, nullptr, nullptr);
				if (skt != INVALID_SOCKET) {
					pClientIns = new CNetClient(skt,TRANS_PROTOCOL_TYPE_TCP);
					if (pClientIns == nullptr){
						return false;
					}
					return true;
				}
				return false;
			}

			SOCKET CNetServer::Accept()
			{
				return ::accept(m_Skt, nullptr, nullptr);
			}
		}
	}
}