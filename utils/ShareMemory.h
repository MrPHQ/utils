#ifndef __UTILS_SHAREMEMORY__
#define __UTILS_SHAREMEMORY__

#include <utils/config.h>
#include <utils/Thread.h>

// Start of namespace section

namespace UTILS
{
#define PROC_FLAG 0x0f0a0b0c
	/// ����ͨ�����ݰ�.
	struct CommunicatorPacket_t
	{
		DWORD dwFlag; //PROC_FLAG
		DWORD dwType; //���ݰ�����
		int iLen; //���ݳ���
		BYTE bData[4];	//���ݶ�
	};
	/// ����ͨѶ������.
	enum CommunicatorType {
		CT_READER = 0,	//ֻ��
		CT_WRITER	//д��
	};
#ifdef WIN32
	typedef struct _SHARE_MEMORY_PROPERTY
	{
		/// �������ݶ�����.
		char szShareSegName[128];
		/// ���ݱ���¼�����.
		char szChangeEventName[128];
		/// ���ݶλ���������.
		char szMutexName[128];
	}SHARE_MEMORY_PROPERTY,*PSHARE_MEMORY_PROPERTY;
#endif
/**
\brief
	MyUtilException class to Exception.
*/
class UTILS_API CShareMemory
{
public:
	CShareMemory();
	~CShareMemory();

	/**
	\brief
		��ʼ������ͨѶ��
	\param nType
		����.
	\param buff
		.
	\param iBuffLen
		.
	\param dwLength
		�����ڴ���󳤶�
	\return
		.
	*/
	int	Init(CommunicatorType nType, PSHARE_MEMORY_PROPERTY pProperty,DWORD dwLength);
	/**
	\brief
		�ͷ���Դ
	*/
	void UnInit();
	/**
	\brief
		������д�빲���棬��ʼ��ΪCT_WRITERʱ��Ч
	*/
	int WriteData(CommunicatorPacket_t* pData);
	/**
	\brief
		ע���ȡ��������д������ݵĻص���������ʼ��ΪCT_READERʱ��Ч
	*/
	/*void RegisterDataCallback(std::function<void(CommunicatorPacket_t* lpPacket, void* pUsrPar)> pCallback, 
		void* pUsrPar);*/
	/**
	\brief
		������ݣ���ʼ��ΪCT_READERʱ��Ч
	*/
	void ResetData();

protected:
	/**
	\brief
		���ݶ�ȡ�̴߳�����
	*/
	static void  ReadDataThread(BOOL& bRun, HANDLE hWait, void* pUsrPar);
	void ReadData(BOOL& bRun, HANDLE hWait);

protected:
	/// �Ƿ��Ѿ���ʼ��.
	BOOL m_bInit; 
#ifdef WIN32
	/// ͨѶ������.
	CommunicatorType m_nType;
	/// ����
	HANDLE m_hFileMapping;
	/// ���ݱ��֪ͨ.
	HANDLE m_hChangeEvent;
	/// ���ݻ�����.
	HANDLE m_hDataMutex;
	/// ���ݶ�ȡ�̣߳���������Ϊ��ȡģʽʱ����
	CThreadBox m_read_thr;
	/// ��������ص�����
	//std::function<void(CommunicatorPacket_t* lpPacket, void* pUsrPar)> m_pfnOutPutData;
	void* m_pUsrpar;
	/// �����ڴ����ַ.
	BYTE* m_pBasePointer; 
#endif
};

} // End of  namespace section

#endif