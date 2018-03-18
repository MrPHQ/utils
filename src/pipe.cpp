#include "../../utils/utils/pipe.h"
#include "../utils/error.h"
#include "../utils/api.h"

namespace UTILS
{
	AbstractPipe::AbstractPipe()
		: _pipe(NULL),
		_err(-1)
	{
		_pipe_name[0] = '\0';
	}

	AbstractPipe::AbstractPipe(HANDLE pipe)
		: _pipe(pipe),
		_err(-1)
	{
		_pipe_name[0] = '\0';
	}

	AbstractPipe::~AbstractPipe(){
		Close();
	}

	void AbstractPipe::Close()
	{
		if (_pipe != NULL){
			::CloseHandle(_pipe);
			_pipe = NULL;
		}
	}

	bool AbstractPipe::isOpen() const
	{
		return _pipe != NULL;
	}

	bool AbstractPipe::isError() const
	{
#ifdef WIN32
		return _err != 0;
#else
#endif
	}

	ClientPipe::ClientPipe()
		: AbstractPipe()
		, _Connected(false)
	{ }


	ClientPipe::ClientPipe(const char* ip, unsigned short port)
		: AbstractPipe()
		, _Connected(false)
	{
		Connect(ip, port);
	}


	ClientPipe::ClientPipe(HANDLE pipe)
		: AbstractPipe(pipe)
		, _Connected(false)
	{
		if (isOpen()){
			_err = 0;
		}
	}


	ClientPipe::~ClientPipe()
	{ }


	bool ClientPipe::isConnect() const {
		if (isError()) {
			return false;
		}
		return _Connected;
	}

	void ClientPipe::Attach(HANDLE pipe){
		if (pipe != NULL) {
			_pipe = pipe;
			_err = 0;
			_Connected = true;
		}
	}

	bool ClientPipe::Connect(const char* pipe_name,unsigned int uiTimeOut /*= 5000*/)
	{
		if (isConnect()){
			return true;
		}

		if (!WaitNamedPipe(pipe_name, uiTimeOut)){
			return false;
		}

		_pipe = CreateFile(pipe_name, 
			GENERIC_READ |  // read and write access   
			GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (_pipe == INVALID_HANDLE_VALUE){
			return false;
		}

		if (!isError()) {
			_Connected = true;
		}
		return true;
	}

	int ClientPipe::read(char* pBuff, int iBuffLen,
		const int ciReadLen /*= 0*/)
	{
		if (isError()) {
			return 0;
		}
		DWORD len = 0;
		BOOL fSuccess = ReadFile(
			_pipe,    // pipe handle 
			pBuff,    // buffer to receive reply 
			iBuffLen,  // size of buffer 
			&len,  // number of bytes read  
			NULL);    // not overlapped
		if (!fSuccess) {
			return 0;
		}
		return len;
	}

	int ClientPipe::write(const char* pBuff, int iBuffLen)
	{
		if (isError()) {
			return 0;
		}
		DWORD len = 0;
		BOOL fSuccess = WriteFile(
			_pipe, // pipe handle 
			pBuff, // message 
			iBuffLen, // message length 
			&len, // bytes written 
			NULL); // not overlapped 
		if (!fSuccess){
			return 0;
		}
		return len;
	}

	ServerPipe::ServerPipe()
	{
	}

	ServerPipe::ServerPipe(const char* pipe_name)
	{
		// Initialize these here so that we do not try to close invalid handles
		// in dtor if the following `openSocket()` fails.

		Init(pipe_name);
	}

	ServerPipe::~ServerPipe()
	{
	}

	int ServerPipe::Init(const char* pipe_name)
	{
		if (pipe_name == nullptr) {
			return -1;
		}
		API::Strcpy(_pipe_name, sizeof(_pipe_name), pipe_name);
		return 0;
	}

	void ServerPipe::UnInit()
	{
		Close();
	}
	bool ServerPipe::Accept(const char* pipe_name, ClientPipe& clt)
	{
		if (!isOpen()){
			return false;
		}
		HANDLE hPipe = AcceptPipe(pipe_name, _err);
		if (hPipe != NULL) {
			clt.Attach(hPipe);
			return true;
		}
		return false;
	}

	HANDLE ServerPipe::Accept(const char* pipe_name) {
		if (!isOpen()) {
			return false;
		}
		return AcceptPipe(pipe_name, _err);
	}

	HANDLE AcceptPipe(const char* lpszPipename, int& error,
		DWORD dwOpenMode /*= PIPE_ACCESS_DUPLEX*/,
		DWORD dwPipeMode /*= PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT*/,
		DWORD nOutBufferSize /*= 1024 * 4*/,
		DWORD nInBufferSize /*= 1024 * 4*/)
	{
		if (nullptr == lpszPipename) {
			return NULL;
		}
		HANDLE hPipe = NULL;
		/*HANDLE hPipe = CreateNamedPipe(
			lpszPipename,             // pipe name 指向管道名称的指针 形式必须为\\.\pipe\pipeName 
			PIPE_ACCESS_DUPLEX,       // read/write access 管道打开模式
			PIPE_TYPE_MESSAGE |       // message type pipe 管道模式
			PIPE_READMODE_MESSAGE |   // message-read mode   
			PIPE_WAIT,                // blocking mode   
			PIPE_UNLIMITED_INSTANCES, // max. instances 最大实例数 
			BUFSIZE,                  // output buffer size 输出缓存大小
			BUFSIZE,                  // input buffer size 输入缓存大小
			0,                        // client time-out 超时设置
			NULL);                    // default security attribute   */
		while (true)
		{
			hPipe = CreateNamedPipe(lpszPipename,
				dwOpenMode,
				dwPipeMode,
				PIPE_UNLIMITED_INSTANCES,
				nOutBufferSize,
				nInBufferSize,
				0,
				NULL);
			if (hPipe == INVALID_HANDLE_VALUE)
			{
				API::SleepTime(10);
				continue;
			}
			//在客户同服务器建立连接后，ConnectNamedPipe（）才会返回
			BOOL fConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
			if (fConnected) {
				break;
			}
		}
		return hPipe;
	}
}