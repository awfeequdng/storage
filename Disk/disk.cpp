#include "disk.h"
#include <string.h>

Disk::Disk()
{

}

BOOL Disk::ReadSectors(HANDLE hDevice, ULONGLONG ullStartSector, DWORD dwSectors, DWORD dwBytesPerSector, LPBYTE lpSectBuff, LPOVERLAPPED lpOverlap, DWORD *pdwErrorCode, DWORD dwTimeOut)
{

}

BOOL Disk::WriteSectors(HANDLE hDevice, ULONGLONG ullStartSector, DWORD dwSectors, DWORD dwBytesPerSector, LPBYTE lpSectBuff, LPOVERLAPPED lpOverlap, DWORD *pdwErrorCode, DWORD dwTimeOut)
{
    ULONGLONG ullOffset = ullStartSector * dwBytesPerSector;
    DWORD dwLen = dwSectors * dwBytesPerSector;
    DWORD dwWriteLen = 0;
    DWORD dwErrorCode = 0;

    if (lpOverlap)
    {
        lpOverlap->Offset = LODWORD(ullOffset);
        lpOverlap->OffsetHigh = HIDWORD(ullOffset);
    }


    if (!WriteFile(hDevice,lpSectBuff,dwLen,dwWriteLen,lpOverlap))
    {
        dwErrorCode = ::GetLastError();

        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

BOOL Disk::WriteFile(HANDLE hDevice, LPBYTE lpSectBuff, DWORD dwLen, DWORD &dwWriteLen, LPOVERLAPPED lpOverlap)
{
    ULONGLONG offset;
    offset=((lpOverlap->OffsetHigh)<<32)|(lpOverlap->Offset);
    int n;
    if((n = pwrite(hDevice,lpSectBuff,dwLen,offset))>0)
    {
        dwWriteLen = n;
        return TRUE;
    }
    else
    {
       if(n<0)
       {
            return FALSE;
       }
       else
       {
            dwWriteLen = n;
            return TRUE;
       }
    }
}

BOOL Disk::ReadFileAsyn(HANDLE hFile, ULONGLONG ullOffset, DWORD &dwSize, LPBYTE lpBuffer, LPOVERLAPPED lpOverlap, PDWORD pdwErrorCode, DWORD dwTimeOut)
{

}

BOOL Disk::WriteFileAsyn(HANDLE hFile, ULONGLONG ullOffset, DWORD &dwSize, LPBYTE lpBuffer, LPOVERLAPPED lpOverlap, PDWORD pdwErrorCode, DWORD dwTimeOut)
{

}

BOOL Disk::ReadSectors(HANDLE hDevice, ULONGLONG ullStartSector, DWORD dwSectors, DWORD dwBytesPerSector, LPBYTE lpSectBuff, DWORD *pdwErrorCode, DWORD dwTimeOut)
{
    ULONGLONG ullOffset = ullStartSector * dwBytesPerSector;
    DWORD dwLen = dwSectors * dwBytesPerSector;
    DWORD dwReadLen = 0;
    DWORD dwErrorCode = 0;

    if (!ReadFile(hDevice,lpSectBuff,dwLen,dwReadLen,ullOffset))
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

BOOL Disk::ReadFile(HANDLE hDevice, LPBYTE lpSectBuff, DWORD dwLen, DWORD &dwReadLen,ULONGLONG offset)
{
    int n;
    if((n = pread(hDevice,lpSectBuff,dwLen,offset))>0)
    {
        dwReadLen = n;
        return TRUE;
    }
    else
    {
       if(n<0)
       {
            return FALSE;
       }
       else
       {
            dwReadLen = n;
            return TRUE;
       }
    }
}

BOOL Disk::WriteSectors(HANDLE hDevice, ULONGLONG ullStartSector, DWORD dwSectors, DWORD dwBytesPerSector, LPBYTE lpSectBuff, DWORD *pdwErrorCode, DWORD dwTimeOut)
{
    ULONGLONG ullOffset = ullStartSector * dwBytesPerSector;
    DWORD dwLen = dwSectors * dwBytesPerSector;
    DWORD dwWriteLen = 0;
    DWORD dwErrorCode = 0;

    if (!WriteFile(hDevice,lpSectBuff,dwLen,dwWriteLen,ullOffset))
    {

        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

BOOL Disk::WriteFile(HANDLE hDevice, LPBYTE lpSectBuff, DWORD dwLen, DWORD &dwWriteLen, ULONGLONG offset)
{
    int n;
    if((n = pwrite(hDevice,lpSectBuff,dwLen,offset))>0)
    {
        dwWriteLen = n;
        return TRUE;
    }
    else
    {
       if(n<0)
       {
            return FALSE;
       }
       else
       {
            dwWriteLen = n;
            return TRUE;
       }
    }
}

BOOL Disk::ReadFileAsyn(HANDLE hFile, ULONGLONG ullOffset, DWORD &dwSize, LPBYTE lpBuffer, PDWORD pdwErrorCode, DWORD dwTimeOut)
{

}

BOOL Disk::WriteFileAsyn(HANDLE hFile, ULONGLONG ullOffset, DWORD &dwSize, LPBYTE lpBuffer, PDWORD pdwErrorCode, DWORD dwTimeOut)
{

}

BootSector Disk::GetBootSectorType(const PBYTE pXBR)
{
    return BootSector_UNKNOWN;
}

PartitionStyle Disk::GetPartitionStyle(const PBYTE pByte, BootSector bootSector)
{
    return PartitionStyle_UNKNOWN;
}

FileSystem Disk::GetFileSystem(const PBYTE pDBR, ULONGLONG ullStartSector)
{

    return FileSystem_UNKNOWN;
}

BOOL Disk::BriefAnalyze()
{
    return FALSE;
}

BOOL Disk::AppendEffDataList(const PBYTE pDBR, FileSystem fileSystem, ULONGLONG ullStartSector, ULONGLONG ullMasterSectorOffset, ULONGLONG ullSectors, BOOL bMBR)
{
    return FALSE;
}

ULONGLONG Disk::ReadOffset(const PBYTE pByte, DWORD offset, BYTE bytes)
{

}

BOOL Disk::IsFATValidCluster(const PBYTE pByte, DWORD cluster, BYTE byFATBit)
{
    return FALSE;
}

BOOL Disk::IsNTFSValidCluster(const PBYTE pByte, DWORD cluster)
{
    return FALSE;
}

BOOL Disk::IsExtXValidBlock(const PBYTE pByte, DWORD block)
{
    return FALSE;
}

void Disk::SetValidSize(ULONGLONG ullSize)
{

}

ULONGLONG Disk::GetValidSize()
{

}

BOOL Disk::IsAllFailed(ErrorType &errType, PDWORD pdwErrorCode)
{
    bool bAllFailed = true;
    PortList::const_iterator it = m_TargetPorts->begin();
    PortList::const_iterator it_end = m_TargetPorts->end();

    while (it != it_end)
    {
        CPort *pPort = (*it);
        if (pPort->IsConnected())
        {
            if (pPort->GetResult())
            {
                bAllFailed = false;
                break;
            }
            else
            {
                errType = pPort->GetErrorCode(pdwErrorCode);
            }
        }
        it++;
    }

    return bAllFailed;

}

void Disk::AddDataQueueList(PDATA_INFO dataInfo)
{
    POSITION pos1 = m_DataQueueList.GetHeadPosition();
    POSITION pos2 = m_TargetPorts->GetHeadPosition();
    CDataQueueList::const_iterator it1 = m_DataQueueList.begin();
    CDataQueueList::const_iterator it1_end = m_DataQueueList.end();
    PortList::const_iterator it2 = m_TargetPorts->begin();
    PortList::const_iterator it2_end = m_TargetPorts->end();

    while (it1 != it1_end && it2!= it2_end)
    {
        CDataQueue *dataQueue = (*it1);
        CPort *port = (*it2);

        if (port->IsConnected() && port->GetResult() && port->GetPortState() == PortState_Active)
        {
            PDATA_INFO data = new DATA_INFO;
            ZeroMemory(data,sizeof(DATA_INFO));

            if (dataInfo->szFileName)
            {
                data->szFileName = new TCHAR[strlen(dataInfo->szFileName) + 1];
                strncpy(data->szFileName,dataInfo->szFileName,strlen(dataInfo->szFileName) + 1);
            }

            data->dwDataSize = dataInfo->dwDataSize;
            data->dwOldSize = dataInfo->dwOldSize;
            data->ullOffset = dataInfo->ullOffset;
            data->pData = new BYTE[dataInfo->dwDataSize];
            memcpy(data->pData,dataInfo->pData,dataInfo->dwDataSize);
            dataQueue->AddTail(data);
        }
        it1++;
        it2++;
    }
}

BOOL Disk::IsReachLimitQty(int limit)
{
    bool bReached = false;

    CDataQueueList::const_iterator it1 = m_DataQueueList.begin();
    CDataQueueList::const_iterator it1_end = m_DataQueueList.end();
    PortList::const_iterator it2 = m_TargetPorts->begin();
    PortList::const_iterator it2_end = m_TargetPorts->end();
    while (it1 !=  it1_end && it2 != it2_end )
    {
        CDataQueue *dataQueue = (*it1);
        CPort *port = (*it2);
        if (port->IsConnected() && port->GetResult() && dataQueue->GetCount() > limit)
        {
            bReached = TRUE;
            break;
        }
        it1++;
        it2++;
    }

    return (bReached || m_CompressQueue.GetCount() > limit);

}

BOOL Disk::IsTargetsReady()
{
    PortList::const_iterator it2 = m_TargetPorts->begin();
    PortList::const_iterator it2_end = m_TargetPorts->end();
    bool bReady = true;
    while (it2 != it2_end)
    {
        CPort *port = (*it2);

        if (port->IsConnected() && port->GetPortState() != PortState_Active)
        {
            bReady = false;

            break;
        }
        it2++;

    }
    return bReady;

}

void Disk::ClearDataQueueList()
{
    CDataQueueList::const_iterator it1 = m_DataQueueList.begin();
    CDataQueueList::const_iterator it1_end = m_DataQueueList.end();
    while (it1 != it1_end)
    {
        CDataQueue *dataQueue = (*it1);

        if (dataQueue)
        {
            dataQueue->Clear();
        }
        it1++;
    }
}

int Disk::EnumFile(CString strSource, int level)
{

}

UINT Disk::GetCurrentTargetCount()
{

}

BOOL Disk::QuickClean(CPort *port, PDWORD pdwErrorCode)
{
    return FALSE;
}

BOOL Disk::QuickClean(HANDLE hDisk, CPort *port, PDWORD pdwErrorCode)
{
    BOOL bResult = TRUE;
    BYTE *pByte = new BYTE[CLEAN_LENGTH];
    ZeroMemory(pByte,CLEAN_LENGTH);

    DWORD dwBytesPerSector = 0;
    ULONGLONG ullSectorNums = GetNumberOfSectors(hDisk,&dwBytesPerSector,NULL);
    DWORD dwSectors = CLEAN_LENGTH/dwBytesPerSector;
    DWORD dwSize = 0;

    if (!WriteSectors(hDisk,0,dwSectors,dwBytesPerSector,pByte,pdwErrorCode))
    {
        bResult = FALSE;
    }

    if (!WriteSectors(hDisk,ullSectorNums-dwSectors,dwSectors,dwBytesPerSector,pByte,port->GetOverlapped(FALSE),pdwErrorCode))
    {
        bResult = FALSE;
    }

    delete []pByte;

    return bResult;
}

DWORD Disk::ReadDiskThreadProc(LPVOID lpParm)
{

}

DWORD Disk::ReadImageThreadProc(LPVOID lpParm)
{

}

DWORD Disk::WriteDiskThreadProc(LPVOID lpParm)
{

}

DWORD Disk::WriteImageThreadProc(LPVOID lpParm)
{

}

DWORD Disk::VerifyThreadProc(LPVOID lpParm)
{

}

DWORD Disk::CompressThreadProc(LPVOID lpParm)
{

}

DWORD Disk::UnCompressThreadProc(LPVOID lpParm)
{

}

DWORD Disk::CleanDiskThreadProc(LPVOID lpParm)
{

}

DWORD Disk::VerifySectorThreadProc(LPVOID lpParm)
{

}

DWORD Disk::ReadFilesThreadProc(LPVOID lpParm)
{

}

DWORD Disk::WriteFilesThreadProc(LPVOID lpParm)
{

}

DWORD Disk::VerifyFilesThreadProc(LPVOID lpParm)
{

}

DWORD Disk::VerifyFilesByteThreadProc(LPVOID lpParm)
{

}

DWORD Disk::FormatDiskThreadProc(LPVOID lpParm)
{

}

DWORD Disk::SearchUserLogThreadProc(LPVOID lpParm)
{

}

DWORD Disk::DeleteChangeThreadProc(LPVOID lpParm)
{

}

DWORD Disk::EnumFileThreadProc(LPVOID lpParm)
{

}

DWORD Disk::ComputeHashThreadProc(LPVOID lpParm)
{

}

DWORD Disk::FullRWTestThreadProc(LPVOID lpParm)
{

}

DWORD Disk::FadePickerThreadProc(LPVOID lpParm)
{

}

DWORD Disk::SpeedCheckThreadProc(LPVOID lpParm)
{

}

DWORD Disk::CompareCleanThreadProc(LPVOID lpParm)
{

}

DWORD Disk::CopyFilesAsyncThreadProc(LPVOID lpParm)
{

}

BOOL Disk::OnCopyFilesAsync()
{
    return FALSE;
}

BOOL Disk::OnCopyDisk()
{
    return FALSE;
}

BOOL Disk::OnCopyImage()
{
    return FALSE;
}

BOOL Disk::OnMakeImage()
{
    return FALSE;
}

BOOL Disk::OnCompareDisk()
{
    return FALSE;
}

BOOL Disk::OnCleanDisk()
{
    return FALSE;
}

BOOL Disk::OnCopyFiles()
{
    return FALSE;
}

BOOL Disk::OnFormatDisk()
{
    return FALSE;
}

BOOL Disk::OnDiffCopy()
{
    return FALSE;
}

BOOL Disk::OnFullRWTest()
{
    return FALSE;
}

BOOL Disk::OnFadePickerTest()
{
    return FALSE;
}

BOOL Disk::OnSpeedCheck()
{
    return FALSE;
}

BOOL Disk::ReadDisk()
{
    BOOL bResult = TRUE;
    DWORD dwErrorCode = 0;
    ErrorType errType = ErrorType_System;
    ULONGLONG ullReadSectors = 0;
    ULONGLONG ullRemainSectors = 0;
    ULONGLONG ullStartSectors = 0;
    DWORD dwSectors = m_nBlockSectors;
    DWORD dwLen = m_nBlockSectors * m_dwBytesPerSector;

    // 计算精确速度
//	LARGE_INTEGER freq,t0,t1,t2,t3;
    double dbTimeNoWait = 0.0,dbTimeWait = 0.0;

    // 如果要先擦除，此时需要等待擦除结束

//	QueryPerformanceFrequency(&freq);

    m_MasterPort->Active();

    // 等待其他线程创建好,最多等5次
    Sleep(100);

    int nTimes = 5;
    while (!IsTargetsReady() && nTimes > 0)
    {
        Sleep(100);
        nTimes--;
    }

//    POSITION pos = m_EffList.GetHeadPosition();

    EFF_LIST::const_iterator itEff = m_EffList.begin();
    EFF_LIST::const_iterator itEff_end = m_EffList.end();

    PBYTE pByte = new BYTE[dwLen];
    ZeroMemory(pByte,dwLen);
//    memset(pByte,0,dwLen);

    while (itEff != itEff_end && !*m_lpCancel && bResult && m_MasterPort->GetPortState() == PortState_Active)
    {
        EFF_DATA effData = (*itEff);
        itEff++;

        ullReadSectors = 0;
        ullRemainSectors = 0;
        ullStartSectors = effData.ullStartSector;
        dwSectors = m_nBlockSectors;
        dwLen = m_nBlockSectors * m_dwBytesPerSector;

        while (bResult && !*m_lpCancel && ullReadSectors < effData.ullSectors && ullStartSectors < m_ullSectorNums)
        {
//			QueryPerformanceCounter(&t0);

            // 判断队列是否达到限制值
            while (IsReachLimitQty(MAX_LENGTH_OF_DATA_QUEUE) && !*m_lpCancel && !IsAllFailed(errType,&dwErrorCode))
            {
                Sleep(5);
            }

            if (*m_lpCancel)
            {
                dwErrorCode = CustomError_Cancel;
                errType = ErrorType_Custom;
                bResult = FALSE;
                break;
            }

            if (IsAllFailed(errType,&dwErrorCode))
            {
                bResult = FALSE;
                break;
            }

            ullRemainSectors = effData.ullSectors - ullReadSectors;

            if (ullRemainSectors + ullStartSectors > m_ullSectorNums)
            {
                ullRemainSectors = m_ullSectorNums - ullStartSectors;
            }

            if (ullRemainSectors < m_nBlockSectors)
            {
                dwSectors = (DWORD)ullRemainSectors;

                dwLen = dwSectors * effData.wBytesPerSector;
            }

            ZeroMemory(pByte,dwLen);

            QueryPerformanceCounter(&t1);
            if (!ReadSectors(m_hMaster,ullStartSectors,dwSectors,effData.wBytesPerSector,pByte,m_MasterPort->GetOverlapped(TRUE),&dwErrorCode))
            {
                bResult = FALSE;


//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d,Speed=%.2f,system errorcode=%ld,%s")
//                    ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum(),m_MasterPort->GetRealSpeed(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
                break;
            }
            else
            {
//                QueryPerformanceCounter(&t2);

                PDATA_INFO dataInfo = new DATA_INFO;
                ZeroMemory(dataInfo,sizeof(DATA_INFO));
                dataInfo->ullOffset = ullStartSectors * effData.wBytesPerSector;
                dataInfo->dwDataSize = dwLen;
                dataInfo->pData = new BYTE[dwLen];
                memcpy(dataInfo->pData,pByte,dwLen);

                if (m_WorkMode == WorkMode_ImageMake && m_bDataCompress)
                {
                    m_CompressQueue.AddTail(dataInfo);
                }
                else if (m_WorkMode == WorkMode_ImageMake)
                {
                    // 不压缩，加上文件头和文件尾
                    PDATA_INFO compressData = new DATA_INFO;
                    ZeroMemory(compressData,sizeof(DATA_INFO));

                    compressData->dwDataSize = dataInfo->dwDataSize + sizeof(ULONGLONG) + sizeof(DWORD) + 1;
                    compressData->dwOldSize = dataInfo->dwDataSize;
                    compressData->pData = new BYTE[compressData->dwDataSize];
                    ZeroMemory(compressData->pData,compressData->dwDataSize);

                    compressData->pData[compressData->dwDataSize - 1] = 0xED; //结束标志

                    memcpy(compressData->pData,&dataInfo->ullOffset,sizeof(ULONGLONG));
                    memcpy(compressData->pData + sizeof(ULONGLONG),&compressData->dwDataSize,sizeof(DWORD));
                    memcpy(compressData->pData + sizeof(ULONGLONG) + sizeof(DWORD),pByte,dwLen);

                    AddDataQueueList(compressData);

                    delete []compressData->pData;
                    delete compressData;

                    delete []dataInfo->pData;
                    delete dataInfo;

                }
                else
                {
                    AddDataQueueList(dataInfo);
                    delete []dataInfo->pData;
                    delete dataInfo;
                }

                if (m_bComputeHash)
                {
                    m_pMasterHashMethod->update((void *)pByte,dwLen);
                }


                ullStartSectors += dwSectors;
                ullReadSectors += dwSectors;

//                QueryPerformanceCounter(&t3);

//                dbTimeNoWait = (double)(t2.QuadPart - t1.QuadPart) / (double)freq.QuadPart; // 秒
//                dbTimeWait = (double)(t3.QuadPart - t0.QuadPart) / (double)freq.QuadPart; // 秒

//                m_MasterPort->AppendUsedWaitTimeS(dbTimeWait);
//                m_MasterPort->AppendUsedNoWaitTimeS(dbTimeNoWait);
                m_MasterPort->AppendCompleteSize(dwLen);


            }

        }
    }

    delete []pByte;

    if (*m_lpCancel)
    {
        bResult = FALSE;
        dwErrorCode = CustomError_Cancel;
        errType = ErrorType_Custom;

//        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d,Speed=%.2f,custom errorcode=0x%X,user cancelled.")
//            ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum(),m_MasterPort->GetRealSpeed(),dwErrorCode);
    }

    // 先设置为停止状态
    // 先设置为停止状态
    if (bResult)
    {
        m_MasterPort->SetPortState(PortState_Stop);
    }
    else
    {
        m_MasterPort->SetResult(FALSE);
        m_MasterPort->SetPortState(PortState_Fail);
        m_MasterPort->SetErrorCode(errType,dwErrorCode);
    }

    // 所有数据都拷贝完
    while (!m_bCompressComplete)
    {
        Sleep(100);
    }

    if (!m_MasterPort->GetResult())
    {
        bResult = FALSE;
        errType = m_MasterPort->GetErrorCode(&dwErrorCode);
    }

    m_MasterPort->SetResult(bResult);
    m_MasterPort->SetEndTime(CTime::GetCurrentTime());

    if (bResult)
    {
        if (!m_bCompare || m_CompareMethod == CompareMethod_Hash)
        {
            m_MasterPort->SetPortState(PortState_Pass);
        }

        if (m_bVerify)
        {
            m_MasterPort->SetPortState(PortState_Pass);
        }

        if (m_bComputeHash)
        {
            m_MasterPort->SetHash(m_pMasterHashMethod->digest(),m_pMasterHashMethod->getHashLength());

            for (int i = 0; i < m_pMasterHashMethod->getHashLength();i++)
            {
                CString strHash;
                strHash.Format(_T("%02X"),m_pMasterHashMethod->digest()[i]);
                m_strMasterHash += strHash;
            }

            CString strHashMethod(m_pMasterHashMethod->getHashMetod());
//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - %s,HashValue=%s")
//                ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum(),strHashMethod,m_strMasterHash);

        }
    }
    else
    {
        m_MasterPort->SetPortState(PortState_Fail);
        m_MasterPort->SetErrorCode(errType,dwErrorCode);
    }

    return bResult;
}

BOOL Disk::ReadImage()
{
//    if (m_bServerFirst)
//	{
//		return ReadRemoteImage();
//	}
//	else
//	{
//		return ReadLocalImage();
//	}
}

BOOL Disk::WriteDisk(CPort *port, CDataQueue *pDataQueue)
{
    BOOL bResult = TRUE;
    DWORD dwErrorCode = 0;
    ErrorType errType = ErrorType_System;

    port->Active();

    // 是否容量完全匹配
    if (m_bMustSameCapacity && !m_bAllowCapGap)
    {
        if (port->GetTotalSize() != m_ullCapacity)
        {
//            port->SetEndTime(CTime::GetCurrentTime());
            port->SetResult(FALSE);
            port->SetErrorCode(ErrorType_Custom,CustomError_Master_Target_Size_Not_Same);
            port->SetPortState(PortState_Fail);
//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Stop copy,custom errorcode=0x%X,the size not same with the master")
//                ,port->GetPortName(),port->GetDiskNum(),CustomError_Master_Target_Size_Not_Same);

            return FALSE;
        }
    }

    // 判断容量
    if (port->GetTotalSize() < m_ullValidSize)
    {
        // 比允许的误差还要小
        if (!m_bAllowCapGap || port->GetTotalSize() < m_ullValidSize * (100-m_nCapGapPercent)/100)
        {
//            port->SetEndTime(CTime::GetCurrentTime());
            port->SetResult(FALSE);
            port->SetErrorCode(ErrorType_Custom,CustomError_Target_Small);
            port->SetPortState(PortState_Fail);
//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Stop copy,custom errorcode=0x%X,target is small")
//                ,port->GetPortName(),port->GetDiskNum(),CustomError_Target_Small);

            return FALSE;
        }
    }
    else
    {
        //有效资料的最大扇区有没有超过子盘容量
        POSITION pos = m_EffList.GetHeadPosition();
        EFF_LIST::const_iterator it = m_EffList.begin();
        EFF_LIST::const_iterator it_end = m_EffList.end();

        while (it != it_end)
        {
            EFF_DATA effData = (*it);
            it++;

            if ( (effData.ullStartSector + effData.ullSectors) * effData.wBytesPerSector > port->GetTotalSize() )
            {
//                port->SetEndTime(CTime::GetCurrentTime());
                port->SetResult(FALSE);
                port->SetErrorCode(ErrorType_Custom,CustomError_Target_Small);
                port->SetPortState(PortState_Fail);
//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Stop copy,custom errorcode=0x%X,target is small")
//                    ,port->GetPortName(),port->GetDiskNum(),CustomError_Target_Small);

                return FALSE;
            }
        }
    }

    HANDLE hDisk/* = GetHandleOnPhysicalDrive(port->GetDiskNum(),FILE_FLAG_OVERLAPPED,&dwErrorCode)*/;

    if (hDisk < 0)
    {
//        port->SetEndTime(CTime::GetCurrentTime());
        port->SetResult(FALSE);
        port->SetErrorCode(ErrorType_System,dwErrorCode);
        port->SetPortState(PortState_Fail);
//        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - OpenDisk error,system errorcode=%ld,%s")
//            ,port->GetPortName(),port->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

        return FALSE;
    }

    SetDiskAtrribute(hDisk,FALSE,TRUE,&dwErrorCode);

    // 计算精确速度
//    LARGE_INTEGER freq,t0,t1,t2;
    double dbTimeNoWait = 0.0,dbTimeWait = 0.0;

    DWORD dwBytesPerSector = port->GetBytesPerSector();

//    QueryPerformanceFrequency(&freq);

    // 是否需要先执行全盘擦除
    if (!m_bCleanDiskFirst)
    {
        QuickClean(hDisk,port,&dwErrorCode);
    }

//	BOOL bWriteLog = TRUE;

    while (!*m_lpCancel && m_MasterPort->GetResult() && port->GetResult() && bResult && !port->IsKickOff())
    {
        QueryPerformanceCounter(&t0);


        while(pDataQueue->GetCount() <= 0 && !*m_lpCancel && m_MasterPort->GetResult()
            && (m_MasterPort->GetPortState() == PortState_Active || !m_bCompressComplete)
            && port->GetResult() && !port->IsKickOff())
        {
            //SwitchToThread();
            Sleep(5);
        }


        if (!m_MasterPort->GetResult())
        {
            errType = m_MasterPort->GetErrorCode(&dwErrorCode);
            bResult = FALSE;
            break;
        }

        if (!port->GetResult())
        {
            errType = port->GetErrorCode(&dwErrorCode);
            bResult = FALSE;
            break;
        }

        if (port->IsKickOff())
        {
            bResult = FALSE;
            dwErrorCode = CustomError_Speed_Too_Slow;
            errType = ErrorType_Custom;
            break;
        }

        if (*m_lpCancel)
        {
            dwErrorCode = CustomError_Cancel;
            errType = ErrorType_Custom;
            bResult = FALSE;
            break;
        }

        if (pDataQueue->GetCount() <= 0 && m_MasterPort->GetPortState() != PortState_Active && m_bCompressComplete)
        {
            dwErrorCode = 0;
            bResult = TRUE;
            break;
        }


        PDATA_INFO dataInfo = pDataQueue->GetHeadRemove();

        if (dataInfo == NULL)
        {
            /*
            if (pDataQueue->GetCount() <= 0 && m_MasterPort->GetPortState() != PortState_Active && m_bCompressComplete)
            {
                dwErrorCode = 0;
                bResult = TRUE;
                break;
            }
            */

            continue;
        }

        // 如果子盘的容量小于有效资料的大小，并且已经达到允许误差了，不用继续拷贝，但是母口还在继续丢资料，所以要continue
        if (port->GetTotalSize() < m_ullValidSize)
        {
            if (m_bAllowCapGap && dataInfo->ullOffset >= port->GetTotalSize())
            {
                //为了保证进度条显示正确，实际完成资料量要继续添加
                port->AppendCompleteSize(dataInfo->dwDataSize);
                delete []dataInfo->pData;
                delete dataInfo;
                continue;
            }
        }

        ULONGLONG ullStartSectors = dataInfo->ullOffset / dwBytesPerSector;
        DWORD dwSectors = dataInfo->dwDataSize / dwBytesPerSector;

        // 打印一个扇区
// 		if (bWriteLog)
// 		{
// 			CString strSector,strByte;
// 			CUtils::WriteLogFile(m_hLogFile,FALSE,_T("Port %s - Sector %d"),port->GetPortName(),ullStartSectors);
// 			for (int i = 0; i < 512;i++)
// 			{
// 				strByte.Format(_T("%02X "),dataInfo->pData[i]);
// 				strSector += strByte;
//
// 				if ((i+1) % 16 == 0 )
// 				{
// 					CUtils::WriteLogFile(m_hLogFile,FALSE,strSector);
// 					strSector.Empty();
// 				}
//
// 			}
//
// 			bWriteLog = FALSE;
// 		}

        QueryPerformanceCounter(&t1);
        if (!WriteSectors(hDisk,ullStartSectors,dwSectors,dwBytesPerSector,dataInfo->pData,port->GetOverlapped(FALSE),&dwErrorCode))
        {
            bResult = FALSE;

            delete []dataInfo->pData;
            delete dataInfo;

            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d,Speed=%.2f,system errorcode=%ld,%s")
                ,port->GetPortName(),port->GetDiskNum(),port->GetRealSpeed(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
            break;
        }
        else
        {
            QueryPerformanceCounter(&t2);

            dbTimeWait = (double)(t2.QuadPart - t0.QuadPart) / (double)freq.QuadPart;

            dbTimeNoWait = (double)(t2.QuadPart - t1.QuadPart) / (double)freq.QuadPart;

            port->AppendUsedWaitTimeS(dbTimeWait);
            port->AppendUsedNoWaitTimeS(dbTimeNoWait);
            port->AppendCompleteSize(dataInfo->dwDataSize);

            delete []dataInfo->pData;
            delete dataInfo;

            /*
            if (port->GetCompleteSize() >= port->GetValidSize())
            {
                break;
            }
            */
        }

    }

    CloseHandle(hDisk);

    if (*m_lpCancel)
    {
        bResult = FALSE;
        dwErrorCode = CustomError_Cancel;
        errType = ErrorType_Custom;

        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port=%s,Disk %d,Speed=%.2f,custom errorcode=0x%X,user cancelled.")
            ,port->GetPortName(),port->GetDiskNum(),port->GetRealSpeed(),dwErrorCode);
    }

    if (port->IsKickOff())
    {
        bResult = FALSE;
        dwErrorCode = CustomError_Speed_Too_Slow;
        errType = ErrorType_Custom;

        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d,Speed=%.2f,custom errorcode=0x%X,speed too slow.")
            ,port->GetPortName(),port->GetDiskNum(),port->GetRealSpeed(),CustomError_Speed_Too_Slow);
    }

    if (!m_MasterPort->GetResult())
    {
        bResult = FALSE;
        errType = m_MasterPort->GetErrorCode(&dwErrorCode);
    }

    if (!port->GetResult())
    {
        errType = port->GetErrorCode(&dwErrorCode);
        bResult = FALSE;
    }


    port->SetResult(bResult);
    port->SetEndTime(CTime::GetCurrentTime());

    if (bResult)
    {
        if (m_bCompare)
        {
            port->SetPortState(PortState_Active);
        }
        else
        {
            port->SetPortState(PortState_Pass);
        }

    }
    else
    {
        port->SetPortState(PortState_Fail);
        port->SetErrorCode(errType,dwErrorCode);
    }

    return bResult;
}

BOOL Disk::WriteImage(CPort *port, CDataQueue *pDataQueue)
{
    if (m_bServerFirst)
    {
        return WriteRemoteImage(port,pDataQueue);
    }
    else
    {
        return WriteLocalImage(port,pDataQueue);
    }
}

BOOL Disk::VerifyDisk(CPort *port, CHashMethod *pHashMethod)
{
    return FALSE;
}

BOOL Disk::VerifyDisk(CPort *port, CDataQueue *pDataQueue)
{
    return FALSE;
}

BOOL Disk::Compress()
{
    m_bCompressComplete = FALSE;
    BOOL bResult = TRUE;

    // 计算精确速度
//    LARGE_INTEGER freq,t0,t1,t2,t3;
    double dbTimeNoWait = 0.0,dbTimeWait = 0.0;

    // 等待其他线程创建好,最多等5次
    Sleep(100);

    int nTimes = 5;
    while (!IsTargetsReady() && nTimes > 0)
    {
        Sleep(100);
        nTimes--;
    }

//    QueryPerformanceFrequency(&freq);
    while (!*m_lpCancel && m_MasterPort->GetResult())
    {
//        QueryPerformanceCounter(&t0);
        while (m_CompressQueue.GetCount() <= 0
            && !*m_lpCancel
            && m_MasterPort->GetResult()
            && m_MasterPort->GetPortState() == PortState_Active)
        {
            //SwitchToThread();
            Sleep(5);
        }

        if (!m_MasterPort->GetResult() || *m_lpCancel )
        {
            bResult = FALSE;
            break;
        }

        if (m_CompressQueue.GetCount() <= 0 && m_MasterPort->GetPortState() != PortState_Active)
        {
            bResult = TRUE;
            break;
        }

//        QueryPerformanceCounter(&t1);
        PDATA_INFO dataInfo = m_CompressQueue.GetHeadRemove();
        if (dataInfo == NULL)
        {
            continue;
        }

        DWORD dwSourceLen = sizeof(ULONGLONG) + sizeof(DWORD) + dataInfo->dwDataSize;
        BYTE *pBuffer = new BYTE[dwSourceLen];
        ZeroMemory(pBuffer,dwSourceLen);
        memcpy(pBuffer,&dataInfo->ullOffset,sizeof(ULONGLONG));
        memcpy(pBuffer + sizeof(ULONGLONG),&dataInfo->dwDataSize,sizeof(DWORD));
        memcpy(pBuffer + sizeof(ULONGLONG) + sizeof(DWORD),dataInfo->pData,dataInfo->dwDataSize);

        delete []dataInfo->pData;

        DWORD dwDestLen = MAX_COMPRESS_BUF;
        BYTE *pDest = new BYTE[MAX_COMPRESS_BUF];
        ZeroMemory(pDest,MAX_COMPRESS_BUF);

        int ret = compress2(pDest,&dwDestLen,pBuffer,dwSourceLen,m_iCompressLevel);

        if (ret == Z_OK)
        {
//            QueryPerformanceCounter(&t2);

            PDATA_INFO compressData = new DATA_INFO;
            ZeroMemory(compressData,sizeof(DATA_INFO));

            compressData->dwDataSize = dwDestLen + sizeof(ULONGLONG) + sizeof(DWORD) + 1;
            compressData->dwOldSize = dataInfo->dwDataSize;
            compressData->pData = new BYTE[compressData->dwDataSize];
            ZeroMemory(compressData->pData,compressData->dwDataSize);

            compressData->pData[compressData->dwDataSize - 1] = 0xED; //结束标志
            memcpy(compressData->pData + sizeof(ULONGLONG),&compressData->dwDataSize,sizeof(DWORD));
            memcpy(compressData->pData + sizeof(ULONGLONG) + sizeof(DWORD),pDest,dwDestLen);

            AddDataQueueList(compressData);

            delete []compressData->pData;
            delete compressData;

//            QueryPerformanceCounter(&t3);

//            dbTimeNoWait = (double)(t2.QuadPart - t1.QuadPart) / (double)freq.QuadPart; // 秒
//            dbTimeWait = (double)(t3.QuadPart - t0.QuadPart) / (double)freq.QuadPart; // 秒

        }
        else if (ret == Z_DATA_ERROR)
        {
            bResult = FALSE;
            m_MasterPort->SetResult(FALSE);
            m_MasterPort->SetPortState(PortState_Fail);
            m_MasterPort->SetErrorCode(ErrorType_Custom,CustomError_Compress_Error);

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Compress data error."));
        }
        else if (ret == Z_MEM_ERROR)
        {
            bResult = FALSE;
            m_MasterPort->SetResult(FALSE);
            m_MasterPort->SetPortState(PortState_Fail);
            m_MasterPort->SetErrorCode(ErrorType_Custom,CustomError_Compress_Error);

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Compress memory error."));
        }
        else if (ret == Z_BUF_ERROR)
        {
            bResult = FALSE;
            m_MasterPort->SetResult(FALSE);
            m_MasterPort->SetPortState(PortState_Fail);
            m_MasterPort->SetErrorCode(ErrorType_Custom,CustomError_Compress_Error);

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Compress buffer error."));
        }
        else
        {
            bResult = FALSE;
            m_MasterPort->SetResult(FALSE);
            m_MasterPort->SetPortState(PortState_Fail);
            m_MasterPort->SetErrorCode(ErrorType_Custom,CustomError_Compress_Error);

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Compress data error."));
        }

        delete []pBuffer;
        delete []pDest;
        delete dataInfo;
    }

    m_bCompressComplete = TRUE;

    return bResult;
}

BOOL Disk::Uncompress()
{
    m_bCompressComplete = FALSE;

    BOOL bResult = TRUE;
    // 计算精确速度
//	LARGE_INTEGER freq,t0,t1,t2,t3;
    double dbTimeNoWait = 0.0,dbTimeWait = 0.0;

    // 等待其他线程创建好,最多等5次
    Sleep(100);

    int nTimes = 5;
    while (!IsTargetsReady() && nTimes > 0)
    {
        Sleep(100);
        nTimes--;
    }

//	QueryPerformanceFrequency(&freq);

    while (!*m_lpCancel && m_MasterPort->GetResult() && bResult)
    {
//		QueryPerformanceCounter(&t0);
        while (m_CompressQueue.GetCount() <= 0
            && !*m_lpCancel
            && m_MasterPort->GetResult()
            && m_MasterPort->GetPortState() == PortState_Active)
        {
            Sleep(5);
        }

        if (!m_MasterPort->GetResult() || *m_lpCancel)
        {
            bResult = FALSE;
            break;
        }

        if (m_CompressQueue.GetCount() <= 0 && m_MasterPort->GetPortState() != PortState_Active)
        {
            bResult = TRUE;
            break;
        }

//        QueryPerformanceCounter(&t1);
        PDATA_INFO dataInfo = m_CompressQueue.GetHeadRemove();

        if (dataInfo == NULL)
        {
            continue;
        }

        DWORD dwDestLen = MAX_COMPRESS_BUF;
        BYTE *pDest = new BYTE[MAX_COMPRESS_BUF];
        ZeroMemory(pDest,MAX_COMPRESS_BUF);

        int ret = uncompress(pDest,&dwDestLen,dataInfo->pData,dataInfo->dwDataSize);

        delete []dataInfo->pData;

        if (ret == Z_OK)
        {
//            QueryPerformanceCounter(&t2);

            PDATA_INFO uncompressData = new DATA_INFO;
            ZeroMemory(uncompressData,sizeof(DATA_INFO));
            uncompressData->ullOffset = *(PULONGLONG)pDest;
            uncompressData->dwDataSize = *(PDWORD)(pDest + sizeof(ULONGLONG));

            uncompressData->pData = new BYTE[uncompressData->dwDataSize];
            ZeroMemory(uncompressData->pData,uncompressData->dwDataSize);
            memcpy(uncompressData->pData,pDest + sizeof(ULONGLONG) + sizeof(DWORD),uncompressData->dwDataSize);

            AddDataQueueList(uncompressData);

            EFF_DATA effData;
            effData.ullStartSector = uncompressData->ullOffset/BYTES_PER_SECTOR;
            effData.ullSectors = uncompressData->dwDataSize/BYTES_PER_SECTOR;
            effData.wBytesPerSector = BYTES_PER_SECTOR;
            m_EffList.AddTail(effData);

            if (m_bComputeHash)
            {
                m_pMasterHashMethod->update((void *)uncompressData->pData,uncompressData->dwDataSize);
            }

            delete []uncompressData->pData;
            delete uncompressData;

//            QueryPerformanceCounter(&t3);

//            dbTimeNoWait = (double)(t2.QuadPart - t1.QuadPart) / (double)freq.QuadPart; // 秒
//            dbTimeWait = (double)(t3.QuadPart - t0.QuadPart) / (double)freq.QuadPart;

        }
        else if (ret == Z_DATA_ERROR)
        {
            bResult = FALSE;
            m_MasterPort->SetResult(FALSE);
            m_MasterPort->SetPortState(PortState_Fail);
            m_MasterPort->SetErrorCode(ErrorType_Custom,CustomError_UnCompress_Error);

            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Uncompress data error."));
        }
        else if (ret == Z_MEM_ERROR)
        {
            bResult = FALSE;
            m_MasterPort->SetResult(FALSE);
            m_MasterPort->SetPortState(PortState_Fail);
            m_MasterPort->SetErrorCode(ErrorType_Custom,CustomError_UnCompress_Error);

            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Uncompress memory error."));
        }
        else if (ret == Z_BUF_ERROR)
        {
            bResult = FALSE;
            m_MasterPort->SetResult(FALSE);
            m_MasterPort->SetPortState(PortState_Fail);
            m_MasterPort->SetErrorCode(ErrorType_Custom,CustomError_UnCompress_Error);

            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Uncompress buffer error."));
        }
        else
        {
            bResult = FALSE;
            m_MasterPort->SetResult(FALSE);
            m_MasterPort->SetPortState(PortState_Fail);
            m_MasterPort->SetErrorCode(ErrorType_Custom,CustomError_UnCompress_Error);

            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Uncompress data error."));
        }

        delete []pDest;
        delete dataInfo;
    }

    m_bCompressComplete = TRUE;

    return bResult;
}

BOOL Disk::CleanDisk(CPort *port)
{
    return FALSE;
}

BOOL Disk::FullRWTest(CPort *port)
{
    return FALSE;
}

BOOL Disk::FadePicker(CPort *port)
{
    return FALSE;
}

BOOL Disk::SpeedCheck(CPort *port)
{
    return FALSE;
}

BOOL Disk::CompareClean(CPort *port)
{
    return FALSE;
}

BOOL Disk::ReadFiles()
{
    return FALSE;
}

BOOL Disk::ReadLocalFiles()
{
    return FALSE;
}

BOOL Disk::WriteFiles(CPort *port, CDataQueue *pDataQueue)
{
    return FALSE;
}

BOOL Disk::VerifyFiles(CPort *port, CHashMethod *pHashMethod)
{
    return FALSE;
}

BOOL Disk::FormatDisk(CPort *port)
{
    return FALSE;
}

BOOL Disk::ReadLocalImage()
{
    return FALSE;
}

BOOL Disk::ReadRemoteImage()
{
    return FALSE;
}

BOOL Disk::WriteLocalImage(CPort *port, CDataQueue *pDataQueue)
{
    BOOL bResult = TRUE;
    DWORD dwErrorCode = 0;
    ErrorType errType = ErrorType_System;
    ULONGLONG ullPkgIndex = 0;
    ULONGLONG ullOffset = SIZEOF_IMAGE_HEADER;
    DWORD dwLen = 0;

    port->Active();

    HANDLE hFile = GetHandleOnFile(port->GetFileName(),CREATE_ALWAYS,FILE_FLAG_OVERLAPPED,&dwErrorCode);

    if (hFile < 0)
    {
//        port->SetEndTime(CTime::GetCurrentTime());
        port->SetResult(FALSE);
        port->SetErrorCode(ErrorType_System,dwErrorCode);
        port->SetPortState(PortState_Fail);
//        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Create image file error,filename=%s,system errorcode=%ld,%s")
//            ,port->GetFileName(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

        return FALSE;
    }

    // 计算精确速度
//    LARGE_INTEGER freq,t0,t1,t2;
    double dbTimeNoWait = 0.0,dbTimeWait = 0.0;

//    QueryPerformanceFrequency(&freq);

    while (!*m_lpCancel && m_MasterPort->GetResult() && port->GetResult())
    {
//        QueryPerformanceCounter(&t0);
        while(pDataQueue->GetCount() <=0 && !*m_lpCancel && m_MasterPort->GetResult()
            && (m_MasterPort->GetPortState() == PortState_Active || !m_bCompressComplete)
            && port->GetResult())
        {
            Sleep(5);
        }

        if (!m_MasterPort->GetResult())
        {
            errType = m_MasterPort->GetErrorCode(&dwErrorCode);
            bResult = FALSE;
            break;
        }

        if (!port->GetResult())
        {
            errType = port->GetErrorCode(&dwErrorCode);
            bResult = FALSE;
            break;
        }

        if (*m_lpCancel)
        {
            dwErrorCode = CustomError_Cancel;
            errType = ErrorType_Custom;
            bResult = FALSE;
            break;
        }

        if (pDataQueue->GetCount() <= 0 && m_MasterPort->GetPortState() != PortState_Active && m_bCompressComplete)
        {
            dwErrorCode = 0;
            bResult = TRUE;
            break;
        }

        PDATA_INFO dataInfo = pDataQueue->GetHeadRemove();

        if (dataInfo == NULL)
        {
            continue;
        }

        if (m_bDataCompress)
        {
            *(PULONGLONG)dataInfo->pData = ullPkgIndex;
        }

        ullPkgIndex++;

//        QueryPerformanceCounter(&t1);

        dwLen = dataInfo->dwDataSize;

        if (!WriteFileAsyn(hFile,ullOffset,dwLen,dataInfo->pData,port->GetOverlapped(FALSE),&dwErrorCode))
        {
            bResult = FALSE;

            delete []dataInfo->pData;
            delete dataInfo;

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Write image file error,filename=%s,Speed=%.2f,system errorcode=%ld,%s")
//                ,port->GetFileName(),port->GetRealSpeed(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

            break;
        }

//        QueryPerformanceCounter(&t2);
//        dbTimeWait = (double)(t2.QuadPart - t0.QuadPart) / (double)freq.QuadPart;
//        dbTimeNoWait = (double)(t2.QuadPart - t1.QuadPart) / (double)freq.QuadPart;

        ullOffset += dwLen;

        port->AppendUsedNoWaitTimeS(dbTimeNoWait);
        port->AppendUsedWaitTimeS(dbTimeNoWait);

        // 压缩的数据比实际数据短，不能取压缩后的长度，要不压缩之前的长度
        port->AppendCompleteSize(dataInfo->dwOldSize);

        delete []dataInfo->pData;
        delete dataInfo;

    }

    if (*m_lpCancel)
    {
        bResult = FALSE;
        dwErrorCode = CustomError_Cancel;
        errType = ErrorType_Custom;

//        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Write image file error,filename=%s,Speed=%.2f,custom errorcode=0x%X,user cancelled.")
//            ,port->GetFileName(),port->GetRealSpeed(),dwErrorCode);
    }

    if (!m_MasterPort->GetResult())
    {
        bResult = FALSE;
        errType = m_MasterPort->GetErrorCode(&dwErrorCode);
    }

    if (!port->GetResult())
    {
        errType = port->GetErrorCode(&dwErrorCode);
        bResult = FALSE;
    }

    // 写IMAGE头
    if (bResult)
    {
        LARGE_INTEGER liSize = {0};
        GetFileSizeEx(hFile,&liSize);
        // 随机生成一个数作为IMAGEID

        IMAGE_HEADER imgHead;
        ZeroMemory(&imgHead,sizeof(IMAGE_HEADER));
        memcpy(imgHead.szImageFlag,IMAGE_FLAG,strlen(IMAGE_FLAG));
        imgHead.ullImageSize = liSize.QuadPart;
        memcpy(imgHead.szAppVersion,APP_VERSION,strlen(APP_VERSION));

        if (m_nImageType)
        {
            imgHead.dwImageType = QUICK_IMAGE;
        }
        else
        {
            imgHead.dwImageType = FULL_IMAGE;
        }

        imgHead.dwMaxSizeOfPackage = MAX_COMPRESS_BUF;
        imgHead.ullCapacitySize = m_MasterPort->GetTotalSize();
        imgHead.dwBytesPerSector = m_MasterPort->GetBytesPerSector();
        memcpy(imgHead.szZipVer,ZIP_VERSION,strlen(ZIP_VERSION));
        imgHead.byUnCompress = m_bDataCompress ? 0 : 1;
        imgHead.ullPkgCount = ullPkgIndex;
        imgHead.ullValidSize = m_MasterPort->GetValidSize();
        imgHead.dwHashLen = m_pMasterHashMethod->getHashLength();
        imgHead.dwHashType = m_MasterPort->GetHashMethod();
        memcpy(imgHead.byImageDigest,m_pMasterHashMethod->digest(),m_pMasterHashMethod->getHashLength());
        imgHead.byEnd = END_FLAG;

        dwLen = SIZEOF_IMAGE_HEADER;

        if (!WriteFileAsyn(hFile,0,dwLen,(LPBYTE)&imgHead,port->GetOverlapped(FALSE),&dwErrorCode))
        {
            bResult = FALSE;

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Write image head error,filename=%s,Speed=%.2f,system errorcode=%ld,%s")
//                ,port->GetFileName(),port->GetRealSpeed(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
        }
    }

    CloseHandle(hFile);

    port->SetResult(bResult);
//    port->SetEndTime(CTime::GetCurrentTime());

    if (bResult)
    {
        port->SetPortState(PortState_Pass);
    }
    else
    {
        port->SetPortState(PortState_Fail);
        port->SetErrorCode(errType,dwErrorCode);
    }

    return bResult;
}

BOOL Disk::WriteRemoteImage(CPort *port, CDataQueue *pDataQueue)
{
    return FALSE;
}

BOOL Disk::SearchUserLog(CPort *port)
{
    return FALSE;
}

void Disk::SearchUserLog(CString strPath, CString strType, CStringArray *pArray)
{

}

void Disk::CleanMapPortStringArray()
{

}

BOOL Disk::UploadUserLog()
{
    return FALSE;
}

BOOL Disk::ReadRemoteFiles()
{
    return FALSE;
}

BOOL Disk::DownloadChangeList()
{
    return FALSE;
}

BOOL Disk::QueryChangeList()
{
    return FALSE;
}

BOOL Disk::DeleteChange(CPort *port)
{
    return FALSE;
}

int Disk::EnumFile(CString strPath, CString strVolume, CMapStringToULL *pMap, CMapStringToString *pArray)
{
    return FALSE;
}

BOOL Disk::EnumFileSize(CPort *port, CMapStringToULL *pMap, CMapStringToString *pArray)
{
    return FALSE;
}

BOOL Disk::ComputeHashValue(CPort *port, CMapStringToString *pMap)
{
    return FALSE;
}

void Disk::CompareFileSize()
{

}

void Disk::CompareHashValue()
{

}

CHAR *Disk::ConvertSENDCMDOUTPARAMSBufferToString(const DWORD *dwDiskData, DWORD nFirstIndex, DWORD nLastIndex)
{
    return 0;
}
