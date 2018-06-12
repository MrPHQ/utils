#ifndef __UTILS_NET__
#define __UTILS_NET__
#include <utils/config.h>

/**
* @brief ����ͨ�ŷ�װ.
* 
* ע��: �ڲ�û�м���,�ⲿ�������м���.
*/
namespace UTILS{
	namespace NET{
		/**
		* @brief �׽���ͨ����Э�����.
		* ..
		*/
		enum TRANS_PROTOCOL_TYPE
		{
			TRANS_PROTOCOL_TYPE_NONE = 0,
			/**< ����Э��ʹ��TCP.*/
			TRANS_PROTOCOL_TYPE_TCP = 1,
			/**< ����Э��ʹ��UDP.*/
			TRANS_PROTOCOL_TYPE_UDP = 2
		};

		/*
		@brief ���������׽���.
			�������˴����׽���ʹ��.�ڲ��ᴴ��\��\�����׽���
		\param nType
			�μ�TRANS_PROTOCOL_TYPE
		\param host
			host. �󶨵���IP��ַ host == nullptr�� ��ʹ��[INADDR_ANY]
		\param port
			�˿�
		\param error
			������
		\return SOCKET
			������Ч����Ч�׽���
		*/
		SOCKET OpenSocket(TRANS_PROTOCOL_TYPE nType, const char* host, unsigned short port, int* error = nullptr);

		/*
		@brief �����׽���.
			�ͻ��˴����׽���ʹ��.�ڲ��ᴴ��\�����׽���
		\param nType
			�μ�TRANS_PROTOCOL_TYPE
		\param host
			IP��ַ. �󶨵���IP��ַ host == nullptr�� ��ʹ��[INADDR_ANY]
		\param port
			�˿�
		\param error
			������
		\param bConn
			���UDP��Ч,���[true]��UDP����˽�������,[false]����������
		\return SOCKET
			������Ч����Ч�׽���
		*/
		SOCKET ConnectSocket(TRANS_PROTOCOL_TYPE nType, const char* host, unsigned short port, int* error = nullptr, bool bConn = true);

		/*
		@brief �����׽���.
		\param skt
			��Ч���׽���
		\param pid
			��ʹ�ø����׽��ֵ�Ŀ����̵Ľ��̱�ʶ��.
		\param pProtocolInfo
			ָ���ɿͻ��˷���Ļ�������ָ�룬���С���԰���WSAPROTOCOL_INFO�ṹ.
		\param iBuffLen
			��������С
		\param pDataLen
			���ݴ�С
		*/
		int UTILS_API DuplicateSocket(SOCKET skt, DWORD pid, BYTE* pProtocolInfo, int iBuffLen, int* pDataLen);

		/*
		@brief ����Э����Ϣ�ṹ����,�����׽���.
		\param pProtocolInfo
			Э����Ϣ�ṹ
		\param iBuffLen
			��������С
		\param skt
			������׽���
		*/
		int UTILS_API CreateSocketFromDuplicate(BYTE* pProtocolInfo, int iDataLen, SOCKET& skt);

		/*
		@brief �����׽��������ӵĶԵ���ĵ�ַ.
		\param skt
			��Ч�׽���
		\param ip
			���IP��ַ�Ļ�����
		\param len
			��������С
		\param port
			�˿�
		*/
		int UTILS_API GetSktPeerInfo(SOCKET skt, char* ip, int len, int* port);

		/*
		@brief �����׽��ֵı�������.
		\param skt
			��Ч�׽���
		\param ip
			���IP��ַ�Ļ�����
		\param len
			��������С
		\param port
			�˿�
		*/
		int UTILS_API GetSktName(SOCKET skt, char* ip, int len, int* port);

		/*
		@brief ���[sockaddr]�ṹ��.
		\param ip
			���IP��ַ�Ļ�����
		\param port
			�˿�
		param addr
			���[sockaddr]�ṹ��.������
		*/
		int UTILS_API StuffSockAddr(TRANS_PROTOCOL_TYPE nType, char* ip, int port, sockaddr& addr);

		namespace SELECT{

			/*
			@brief ��TCPЭ���׽����϶�ȡ����.
				��С��ʱʱ�� 10����
			\param skt
				��Ч���׽���
			\param pBuff
				�������ݻ�����.
			\param iBuffLen
				�������ݻ�������С
			\param iReadLen
				��Ҫ��ȡ�����ݴ�С
			\param error
				���������
			\param uiTimeOut
				��ʱʱ��, =0,Ҳ�ᱣ֤[select]���һ��
			\return ��ȡ�����ݴ�С
			*/
			unsigned int ReadFromTcp(SOCKET skt, char* pBuff, int iBuffLen, const int iReadLen = 0, int* error = nullptr, unsigned int uiTimeOut = 5000);
			/*
			@brief ��UDPЭ���׽����϶�ȡ����.
				��С��ʱʱ�� 10����
			\param skt
				��Ч���׽���
			\param pBuff
				�������ݻ�����.
			\param iBuffLen
				�������ݻ�������С
			\param from
				�Զ˵�ַ��Ϣ
			\param fromlen
				��ַ��Ϣ��С
			\param error
				���������
			\param uiTimeOut
				��ʱʱ��,=0,Ҳ�ᱣ֤[select]���һ��
			\return ��ȡ�����ݴ�С
			*/
			unsigned int ReadFromUdp(SOCKET skt, char* pBuff, int iBuffLen, struct sockaddr& from, int fromlen, int* error = nullptr, unsigned int uiTimeOut = 5000);
			/*
			@brief ��TCPЭ���׽�����д������.
				��С��ʱʱ�� 10����
			\param skt
				��Ч���׽���
			\param pBuff
				�������ݻ�����.
			\param iBuffLen
				�������ݻ�������С
			\param error
				���������
			\param uiTimeOut
				��ʱʱ��,=0,Ҳ�ᱣ֤[select]���һ��
			\return д������ݴ�С
			*/
			unsigned int WriteFromTcp(SOCKET skt, const char* pBuff, int iBuffLen, int* error = nullptr, unsigned int uiTimeOut = 5000);
			/*
			@brief ��UDPЭ���׽�����д������.
				��С��ʱʱ�� 10����
			\param skt
				��Ч���׽���
			\param pBuff
				�������ݻ�����.
			\param iBuffLen
				�������ݻ�������С
			\param from
				�Զ˵�ַ��Ϣ
			\param fromlen
				��ַ��Ϣ��С
			\param error
				���������
			\param uiTimeOut
				��ʱʱ��,=0,Ҳ�ᱣ֤[select]���һ��
			\return д������ݴ�С
			*/
			unsigned int WriteFromUDP(SOCKET skt, const char* pBuff, int iBuffLen, struct sockaddr& to, int tolen, int* error = nullptr, unsigned int uiTimeOut = 5000);

			/**
			* @brief �׽���ͨ�Ż���.
			* ʹ��[select]ģ��
			*/
			class UTILS_API CNet
			{
			protected:
				CNet();
				CNet(UTILS::NET::TRANS_PROTOCOL_TYPE);
				/*
				@brief �׽���ͨ����Э�����.
				*/
				CNet(SOCKET skt, UTILS::NET::TRANS_PROTOCOL_TYPE);
				virtual ~CNet() = 0;
			public:

				/*
				@brief �ر��׽���.
				*/
				virtual void Close();

				/*
				@brief �Ƿ���Ч�׽���.
					skt != INVALID_SOCKET
				*/
				virtual bool IsValidSkt() const;

				/*
				@brief ����socket api�Ƿ��ʹ���.
				*/
				virtual bool IsError() const;

				/*
				@brief ��ȡUDP�Զ˵�ַ��Ϣ.
				\param stAddr
					���ջ�����
				*/
				virtual int GetPeerAddrInfo(struct sockaddr& stAddr);

				/*
				@brief �����׽���ѡ��.
					�����μ�windows��[setsockopt]
				*/
				int SetSktOpt(int level,int optname, const char* optval,int optlen);

				/*
				@brief �����׽���ѡ��.
					�����μ�windows��[getsockopt]
				*/
				int GetSktOpt(int level,int optname, char* optval, int *optlen);

				/*
				@brief �����׽��ַ�����ģʽ.
				*/
				int SetSktNoBlock();
				/*
				@brief ����TCP�׽��ֵ�Nagle�㷨.
					��ѡ����Ĭ������±����ã�����ΪFALSE����
				*/
				int SetTcpNoDelay();

				/*
				@brief ����ÿ���׽��ַ��ͻ������ռ�����.
					Ĭ�ϴ�С��8k
					�������λ0,������ֱ�ӷ��͵�ϵͳ������
				*/
				int SetSktSendBufferSize(unsigned int uiSize);

				/*
				@brief ����ÿ���׽��ֽ��ջ������ռ�����.
					Ĭ�ϴ�С��8k
				*/
				int SetSktRecvBufferSize(unsigned int uiSize);

				/*
				@brief �׽��ִ��ݹرճ�ʱ.
				\param uiTime
					��ʱ(��). 0-��ֹ���ݹر�.
				*/
				int SetSktCloseLinger(unsigned int uiTime);

				/*
				@brief �����׽��ְ󶨵�����ʹ�õĵ�ַ�Ͷ˿�.
					SO_EXCLUSIVEADDRUSEѡ����Է�ֹ�������.
				\param bReuse
					true-����.
				*/
				int SetSktReuseAddr(bool bReuse);

				/*
				@brief ��д����..
				\param uiTimeOut
					��ʱʱ��,=0,Ҳ�ᱣ֤[select]���һ��
				*/
				unsigned int Read(char* pBuff, int iBuffLen, const int iReadLen = 0, unsigned int uiTimeOut = 5000);
				unsigned int Read(char* pBuff, int iBuffLen, struct sockaddr& from, int fromlen, unsigned int uiTimeOut = 5000);
				unsigned int Write(const char* pBuff, int iBuffLen, unsigned int uiTimeOut = 5000);
				unsigned int Write(const char* pBuff, int iBuffLen, struct sockaddr& to, int tolen, unsigned int uiTimeOut = 5000);
			protected:
				/*
				@brief ���ô�����.
				\param iErrorCode
					������
				\param lien
					����λ��
				*/
				void SetErrorCode(int iErrorCode, int line);
			protected:
				/**< ͨ���׽���.*/
				SOCKET m_Skt;
				/**< ������.*/
				int m_ErrorCode;
				/**< ����Э������.*/
				UTILS::NET::TRANS_PROTOCOL_TYPE m_nTransProType;
			};

			class UTILS_API CNetClient
				: public CNet
			{
			public:
				CNetClient();
				CNetClient(UTILS::NET::TRANS_PROTOCOL_TYPE);
				CNetClient(SOCKET skt, UTILS::NET::TRANS_PROTOCOL_TYPE);
				virtual ~CNetClient();

				/*
				@brief �����׽���.
					�����׽���ʹ��.�ڲ��ᴴ��\�����׽���
				\param nType
					�μ�TRANS_PROTOCOL_TYPE
				\param host
					IP��ַ. �󶨵���IP��ַ host == nullptr�� ��ʹ��[INADDR_ANY]
				\param port
					�˿�
				\param error
					������
				\param bConn
					���UDP��Ч,���[true]��UDP����˽�������,[false]����������
				\return SOCKET
					������Ч����Ч�׽���
				*/
				int ConnectSocket(TRANS_PROTOCOL_TYPE nType, const char* host, unsigned short port, int* error = nullptr, bool bConn = true);

				/*
				@brief �����׽���.
					�ӿ��ڲ��Զ�����Ϊ [TRANS_PROTOCOL_TYPE_TCP]
				\param skt
					��Ч�׽���
				*/
				void Attach(SOCKET skt);
			};

			class UTILS_API CNetServer
				: public CNet
			{
			public:
				CNetServer();
				CNetServer(UTILS::NET::TRANS_PROTOCOL_TYPE);
				virtual ~CNetServer();

				/*
				@brief ���������׽���.
					�������˴����׽���ʹ��.�ڲ��ᴴ��\��\�����׽���
				\param nType
					�μ�TRANS_PROTOCOL_TYPE
				\param host
					host. �󶨵���IP��ַ host == nullptr�� ��ʹ��[INADDR_ANY]
				\param port
					�˿�
				\param error
					������
				*/
				int OpenSocket(TRANS_PROTOCOL_TYPE nType, const char* host, unsigned short port, int* error = nullptr);

				/*
				@brief ���տͻ�������.
					
				\param pClientIns
					���ӳɹ�,���ؿͻ���ʵ��
				\return bool
					�пͻ������ӷ���true,���򷵻�false
				*/
				bool Accept(CNetClient*& pClientIns);
				/*
				@brief ���տͻ�������.
					
				\return SOCKET
					�пͻ������ӷ�����Ч socket,���򷵻�INVALID_SOCKET
				*/
				SOCKET Accept();

			};
			//int ShutdownSocket(SOCKET sock);
		}
	}
}
#endif