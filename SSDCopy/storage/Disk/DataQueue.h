//#pragma once
#ifndef __DATAQUEUE_H_
#define __DATAQUEUE_H_

#include <list>
#include "GlobalData/GlobalDef.h"
#include <thread>
#include <mutex>

typedef struct _STRUCT_DATA_INFO
{
    PCHAR    szFileName; //在WPD中记录的是ObjectID
	ULONGLONG ullOffset;
	DWORD     dwDataSize;
	DWORD     dwOldSize;   // 用于记录压缩之前数据大小
	LPBYTE    pData;
}DATA_INFO,*PDATA_INFO;
typedef std::list<PDATA_INFO> DATA_QUEUE;
#define MAX_LENGTH_OF_DATA_QUEUE  200
#define SEMAPHONE_TIME_OUT        2000
//class CCriticalSection
//{
//public:
//    CCriticalSection(){}
//    void Unlock(){}
//    void Lock(){}
//};

class CDataQueue
{
public:
	CDataQueue(void);
	~CDataQueue(void);

	int GetCount();
	void AddTail(PDATA_INFO dataInfo);
	PDATA_INFO GetHead();
	PDATA_INFO GetHeadRemove();
	void RemoveHead();
	void Clear();

private:
	DATA_QUEUE m_DataQueue;
    std::mutex m_mtx;
};

typedef std::list<CDataQueue *> CDataQueueList;

#endif //__DATAQUEUE_H_
