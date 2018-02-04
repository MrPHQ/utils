#ifndef __UTILS_SOCKET__
#define __UTILS_SOCKET__
#include <utils/config.h>

namespace UTILS
{
	enum SocketState {
		ok,
		not_opened,
		bad_address,
		connection_failed,
		broken_pipe,
		invalid_access_mode,
		message_truncated,
		accept_interrupted
	};

	class SocketBuffer;
	class UTILS_API AbstractSocket
	{
	public:
		AbstractSocket();
		/*
		/pref

		sock,如果套接字有效，直接内部设置为了异步socket
		*/
		AbstractSocket(SOCKET sock,
			SocketState state,
			int err);
		AbstractSocket(AbstractSocket const &) = delete;
		AbstractSocket(AbstractSocket &&) throw();
		virtual ~AbstractSocket() = 0;

		/// Close socket
		virtual void close();
		virtual bool isOpen() const;
		virtual bool isConnect() const;
		virtual bool isError() const;
		virtual void shutdown();
		AbstractSocket & operator= (AbstractSocket && rhs) throw();

		void swap(AbstractSocket&);

		void setAsyncSkt();

	protected:
		SOCKET sock;
		SocketState state;
		int err;
	};

	class UTILS_API Socket : public AbstractSocket
	{
	public:
		// ctor and dtor
		Socket();
		Socket(SOCKET sock,
			SocketState state,
			int err);
		Socket(const std::string& address,
			unsigned short port,
			bool udp = false,
			bool ipv6 = false);
		Socket(Socket &&) throw();
		virtual ~Socket();

		Socket& operator= (Socket &&)throw();

		// methods
		virtual bool connect(const std::string& address,
			unsigned short port,
			unsigned int uiTimeOut = 5000,
			bool udp = false,
			bool ipv6 = false);
		virtual bool read(SocketBuffer& buffer,
			const int ciReadLen = 0,
			unsigned int uiTimeOut = 5000);
		virtual bool write(SocketBuffer& buffer,
			unsigned int uiTimeOut = 5000);
		virtual bool write(const std::string& buffer,
			unsigned int uiTimeOut = 5000);
		virtual bool write(const char* buff,
			int len,
			unsigned int uiTimeOut = 5000);

		template <typename... Args>
		static bool write(Socket & socket, Args &&... args)
		{
			SocketBuffer const * const buffers[sizeof... (args)]{
				(&args)... };
			return socket.write(sizeof... (args), buffers);
		}
	};

	class UTILS_API ServerSocket : public AbstractSocket
	{
	public:
		ServerSocket(unsigned short port,
			bool accept_block = true,
			bool udp = false,
			bool ipv6 = false,
			const std::string& host = std::string());
		ServerSocket(ServerSocket &&) throw();
		virtual ~ServerSocket();

		ServerSocket& operator = (ServerSocket &&) throw();

		Socket accept();
		void swap(ServerSocket&);
	};

	class UTILS_API SocketBuffer
	{
	public:
		explicit SocketBuffer(unsigned int max);
		explicit SocketBuffer(char*, int);
		virtual ~SocketBuffer();

		char *getBuffer() const { return buffer; }
		unsigned int getMaxSize() const { return maxsize; }
		unsigned int getSize() const { return size; }
		void setSize(unsigned int s) { size = s; }
		unsigned int getPos() const { return pos; }
		void setPos(unsigned int s) { pos = s; }

		unsigned char readByte();
		unsigned short readShort();
		unsigned int readInt();

		void appendByte(unsigned char val);
		void appendShort(unsigned short val);
		void appendInt(unsigned int val);
		void appendString(const std::string& str);
		void appendBuffer(const SocketBuffer& buffer);

	private:
		/// 缓存区大小
		unsigned int maxsize;
		/// 数据大小
		unsigned int size;
		/// 数据操作位置
		unsigned int pos;
		/// 缓存区
		char *buffer;
		bool bHeap;
		SocketBuffer(SocketBuffer const & rhs);
		SocketBuffer& operator= (SocketBuffer const& rhs);
	};


	/**
	/pref

	*/

	SOCKET OpenSocket(unsigned short port,
		bool udp,
		bool ipv6,
		SocketState& state,
		int& error);
	SOCKET OpenSocket(const std::string& host,
		unsigned short port,
		bool udp,
		bool ipv6,
		SocketState& state,
		int& error);

	SOCKET ConnectSocket(const std::string& hostn,
		unsigned short port,
		bool udp,
		bool ipv6,
		SocketState& state,
		int& error,
		bool bNonBlock = true,
		unsigned int uiTimeOut = 5000);
	SOCKET AcceptSocket(SOCKET sock,
		SocketState& state,
		int& error);
	int CloseSocket(SOCKET sock);
	int ShutdownSocket(SOCKET sock);

	long read(SOCKET sock,
		SocketBuffer& buffer,
		int& error,
		const int ciReadLen = 0,
		unsigned int uiTimeOut = 5000);
	long write(SOCKET sock,
		SocketBuffer& buffer,
		int& error,
		unsigned int uiTimeOut = 5000);
	long write(SOCKET sock,
		const std::string& buffer,
		int& error,
		unsigned int uiTimeOut = 5000);

	std::string getHostname(bool fqdn);
	int setTCPNoDelay(SOCKET, bool, int& error);
	int getPeerInfo(SOCKET skt, char* ip, int len, int* port);

	static inline int getLastSocketError()
	{
#ifdef WIN32	
		return WSAGetLastError();
#else
		return errno;
#endif
	}
}
#endif