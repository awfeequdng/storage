
#include "DataQueue.h"


CDataQueue::CDataQueue(void)
{
    // 	m_hSemaphoneIn = CreateSemaphore(NULL,MAX_LENGTH_OF_DATA_QUEUE,MAX_LENGTH_OF_DATA_QUEUE,NULL);
    //
    // 	m_hSemaphoneOut = CreateSemaphore(NULL,0,MAX_LENGTH_OF_DATA_QUEUE,NULL);
}


CDataQueue::~CDataQueue(void)
{
    Clear();
}

void CDataQueue::AddTail( PDATA_INFO dataInfo )
{
    m_mtx.lock();
    m_DataQueue.push_back(dataInfo);
    m_mtx.unlock();
}

PDATA_INFO CDataQueue::GetHead()
{
    m_mtx.lock();
    PDATA_INFO dataInfo = NULL;

    if (!m_DataQueue.empty())
    {
        dataInfo = m_DataQueue.front();
    }

    m_mtx.unlock();
    return dataInfo;
}

PDATA_INFO CDataQueue::GetHeadRemove()
{
    m_mtx.lock();
    PDATA_INFO temp = NULL;
    PDATA_INFO dataInfo = NULL;

    if (!m_DataQueue.empty())
    {
        dataInfo = m_DataQueue.front();

        temp = new DATA_INFO;
        temp->ullOffset = dataInfo->ullOffset;
        temp->dwDataSize = dataInfo->dwDataSize;
        temp->dwOldSize = dataInfo->dwOldSize;
        temp->pData = new BYTE[dataInfo->dwDataSize];
        memcpy(temp->pData,dataInfo->pData,dataInfo->dwDataSize);

        if (dataInfo->szFileName)
        {
            temp->szFileName = new CHAR[strlen(dataInfo->szFileName)+1];
            strncpy(temp->szFileName,dataInfo->szFileName,strlen(dataInfo->szFileName)+1);

            delete []dataInfo->szFileName;
            dataInfo->szFileName = NULL;
        }

        if (dataInfo->pData)
        {
            delete []dataInfo->pData;
            dataInfo->pData = NULL;
        }

        delete dataInfo;
        dataInfo = NULL;

        m_DataQueue.pop_front();
    }

    m_mtx.unlock();
    return temp;
}


void CDataQueue::RemoveHead()
{
    m_mtx.lock();
    PDATA_INFO dataInfo = NULL;

    if (!m_DataQueue.empty())
    {
        dataInfo = m_DataQueue.front();
        if (dataInfo)
        {
            if (dataInfo->pData)
            {
                delete [] dataInfo->pData;
                dataInfo->pData = NULL;
            }

            if (dataInfo->szFileName)
            {
                delete []dataInfo->szFileName;
                dataInfo->szFileName = NULL;
            }

            delete dataInfo;
            dataInfo = NULL;


        }
        m_DataQueue.pop_front();
    }

    m_mtx.unlock();
}

void CDataQueue::Clear()
{
    m_mtx.lock();

    while (!m_DataQueue.empty())
    {
        PDATA_INFO dataInfo = m_DataQueue.front();

        if (dataInfo)
        {
            if (dataInfo->pData)
            {
                delete [] dataInfo->pData;
                dataInfo->pData = NULL;
            }

            if (dataInfo->szFileName)
            {
                delete []dataInfo->szFileName;
                dataInfo->szFileName = NULL;
            }

            delete dataInfo;
            dataInfo = NULL;
        }

        m_DataQueue.pop_front();
    }

    m_mtx.unlock();
}

int CDataQueue::GetCount()
{
    int nCount = m_DataQueue.size();
    return nCount;
}
