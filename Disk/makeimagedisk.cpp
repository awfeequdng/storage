#include "makeimagedisk.h"

#include <unistd.h>
#include <stdio.h>
#include<fcntl.h>

#include <thread>

//#define HIDE_SECTORS  0x800   // 1M
//#define COMPRESS_THREAD_COUNT 3
//extern BOOL g_bFilecopyType;



MakeImageDisk::MakeImageDisk()
{

    m_hDisk = -1;
    m_hImage = -1;

    m_nBlockSectors = 256;
    m_dwBytesPerSector = 512;
    m_bDataCompress = FALSE;
    m_bServerFirst = FALSE;

    m_bMustSameCapacity = FALSE;
    m_bAllowCapGap = TRUE;
    m_bCompressComplete = FALSE;

    m_bReadDiskOver = FALSE;

    m_strDiskPath = "/dev/nvme0n1";
    m_strImagePath = "/home/caipengxiang/image";

    m_hDisk = open(m_strDiskPath.c_str(),O_RDONLY);
    m_hImage = open(m_strImagePath.c_str(),O_RDWR);

    if(m_hDisk <0)
    {
        std::cerr<<"open disk "<<m_strDiskPath<<" error!"<<std::endl;
    }
    if(m_hImage <0)
    {
        std::cerr<<"open image file "<<m_strImagePath<<" error!"<<std::endl;
    }
}

MakeImageDisk::MakeImageDisk(CString filePath, CString imagePath)
{
     m_strDiskPath = filePath;
     m_strImagePath = imagePath;

     m_hDisk = open(m_strDiskPath.c_str(),O_RDONLY);
     if(m_hDisk <0)
     {
         std::cerr<<"open disk "<<m_strDiskPath<<" error!"<<std::endl;
     }
     m_hImage = open(m_strImagePath.c_str(),O_RDWR);
     if(m_hImage <0)
     {
         std::cerr<<"open image file "<<m_strImagePath<<" error!"<<std::endl;
     }
}

BOOL MakeImageDisk::make()
{
//    std::thread tdReadDisk()
    std::cout<<"Make Disk Image !"<<endl;

    BYTE pByte[512] ;
    DWORD readLen;
    ReadFile(m_hDisk,pByte,512,readLen,0);


    if(BriefAnalyze())
    {
        std::cout<<"brief analyze over!"<<std::endl;
    }
    else
    {
        std::cout<<"analyze file system failed!"<<std::endl;
    }
//    Sleep(5000);

//    std::thread readDiskThd(ReadDiskThreadProc,this);
//    std::thread writeImageThd(WriteImageThreadProc,this);

//    readDiskThd.join();
//    std::cout<<"read disk thread finished!"<<std::endl;
//    writeImageThd.join();
//    std::cout<<"write image thread finished!"<<std::endl;

    return FALSE;
}
CString MakeImageDisk::strImagePath() const
{

    return m_strImagePath;
}

void MakeImageDisk::setStrImagePath(const CString &strImagePath)
{
    m_strImagePath = strImagePath;
    m_hImage = open(m_strImagePath.c_str(),O_RDWR);
}
HANDLE MakeImageDisk::hImage() const
{
    return m_hImage;
}

void MakeImageDisk::setHImage(const HANDLE &hImage)
{
    m_hImage = hImage;
}
CString MakeImageDisk::strDiskPath() const
{
    return m_strDiskPath;
}

void MakeImageDisk::setStrDiskPath(const CString &strDiskPath)
{
    m_strDiskPath = strDiskPath;

    m_hDisk = open(m_strDiskPath.c_str(),O_RDONLY);
}
HANDLE MakeImageDisk::hDisk() const
{
    return m_hDisk;
}

void MakeImageDisk::setHDisk(const HANDLE &hDisk)
{
    m_hDisk = hDisk;
}

BOOL MakeImageDisk::IsReachLimitQty(int limit)
{
    bool bReached = false;

    if(m_DataQueue.GetCount() > limit)
    {
        bReached = TRUE;
    }

    return (bReached || m_CompressQueue.GetCount() > limit);
}

BOOL MakeImageDisk::ReadSectors(HANDLE hDevice, ULONGLONG ullStartSector, DWORD dwSectors, DWORD dwBytesPerSector, LPBYTE lpSectBuff, DWORD *pdwErrorCode, DWORD dwTimeOut)
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
#include <errno.h>
BOOL MakeImageDisk::ReadFile(HANDLE hDevice, LPBYTE lpSectBuff, DWORD dwLen, DWORD &dwReadLen, ULONGLONG offset)
{
    //从一个描述符读取n个字节

//    int fd = hDevice;
//    void *vptr = lpSectBuff;
//    size_t n= dwLen;

//     size_t  nleft = n;  //记录还剩下多少字节数没读取
//     ssize_t nread;      //记录已经读取的字节数
//     char*  ptr = (char *)vptr;  //指向要读取数据的指针
//     while(nleft > 0)    //还有数据要读取
//     {
//      if(nread = pread(fd,ptr,nleft,offset+n-nleft) < 0)
//       if(errno == EINTR)//系统被一个捕获的信号中断
//        nread = 0;       //再次读取
//       else
//        return FALSE;       //返回
//      else if(nread == 0)//没有出错但是也没有读取到数据
//       break;            //再次读取
//      nleft -= nread;    //计算剩下未读取的字节数
//      ptr  += nread;     //移动指针到以读取数据的下一个位置
//     }
//     dwReadLen = n-nleft;
//     return (n-nleft);   //返回读取的字节数

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

BOOL MakeImageDisk::WriteSectors(HANDLE hDevice, ULONGLONG ullStartSector, DWORD dwSectors, DWORD dwBytesPerSector, LPBYTE lpSectBuff, DWORD *pdwErrorCode, DWORD dwTimeOut)
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

BOOL MakeImageDisk::WriteFile(HANDLE hDevice, LPBYTE lpSectBuff, DWORD dwLen, DWORD &dwWriteLen, ULONGLONG offset)
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

BOOL MakeImageDisk::ReadFileAsyn(HANDLE hFile, ULONGLONG ullOffset, DWORD &dwSize, LPBYTE lpBuffer, PDWORD pdwErrorCode, DWORD dwTimeOut)
{
    int n;
    if((n = pread(hFile,lpBuffer,dwSize,ullOffset))>0)
    {
        dwSize = n;
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
            dwSize = n;
            return TRUE;
       }
    }
}

BOOL MakeImageDisk::WriteFileAsyn(HANDLE hFile, ULONGLONG ullOffset, DWORD &dwSize, LPBYTE lpBuffer, PDWORD pdwErrorCode, DWORD dwTimeOut)
{
    long n;
    if((n = pwrite(hFile,lpBuffer,dwSize,ullOffset))>0)
    {
        dwSize = n;
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
            dwSize = n;
            return TRUE;
       }
    }
}


BOOL MakeImageDisk::ReadDisk()
{
    BOOL bResult = TRUE;
    DWORD dwErrorCode = 0;
    ErrorType errType = ErrorType_System;
    ULONGLONG ullReadSectors = 0;
    ULONGLONG ullRemainSectors = 0;
    ULONGLONG ullStartSectors = 0;
    DWORD dwSectors = m_nBlockSectors;
    DWORD dwLen = m_nBlockSectors * m_dwBytesPerSector;


    EFF_LIST::const_iterator itEff = m_EffList.begin();
    EFF_LIST::const_iterator itEff_end = m_EffList.end();

    PBYTE pByte = new BYTE[dwLen];
    ZeroMemory(pByte,dwLen);

    m_bReadDiskOver = FALSE;

    while (itEff != itEff_end )
    {
        EFF_DATA effData = (*itEff);
        itEff++;

        ullReadSectors = 0;
        ullRemainSectors = 0;
        ullStartSectors = effData.ullStartSector;
        dwSectors = m_nBlockSectors;
        dwLen = m_nBlockSectors * m_dwBytesPerSector;

        while (bResult && ullReadSectors < effData.ullSectors && ullStartSectors < m_ullSectorNums)
        {
            // 判断队列是否达到限制值
            while (IsReachLimitQty(MAX_LENGTH_OF_DATA_QUEUE))
            {
                Sleep(5);
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

//            QueryPerformanceCounter(&t1);
            if (!ReadSectors(m_hDisk,ullStartSectors,dwSectors,effData.wBytesPerSector,pByte,&dwErrorCode))
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

                if (m_bDataCompress)
                {
                    m_CompressQueue.AddTail(dataInfo);
                }
                else
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

                    m_DataQueue.AddTail(compressData);

                    delete []dataInfo->pData;
                    delete dataInfo;
                }

                ullStartSectors += dwSectors;
                ullReadSectors += dwSectors;

//                QueryPerformanceCounter(&t3);

//                dbTimeNoWait = (double)(t2.QuadPart - t1.QuadPart) / (double)freq.QuadPart; // 秒
//                dbTimeWait = (double)(t3.QuadPart - t0.QuadPart) / (double)freq.QuadPart; // 秒

//                m_MasterPort->AppendUsedWaitTimeS(dbTimeWait);
//                m_MasterPort->AppendUsedNoWaitTimeS(dbTimeNoWait);
//                m_MasterPort->AppendCompleteSize(dwLen);
            }
        }
    }

    delete []pByte;


    // 先设置为停止状态
    // 先设置为停止状态
    if (bResult)
    {
//        m_MasterPort->SetPortState(PortState_Stop);
        std::cerr<<"read over!"<<endl;
    }
    else
    {
        std::cerr<<"read failed!"<<endl;
    }

    // 所有数据都拷贝完
//    while (!m_bCompressComplete)
//    {
//        Sleep(100);
//    }
    m_bReadDiskOver = TRUE;
    return bResult;
}

BOOL MakeImageDisk::ReadImage()
{
    if (m_bServerFirst)
    {
        return ReadRemoteImage();
    }
    else
    {
        return ReadLocalImage();
    }
}

void MakeImageDisk::ReadDiskThreadProc(LPVOID *param)
{
    MakeImageDisk *pMakeImage = (MakeImageDisk *)param;
    Sleep(1000);
    pMakeImage->ReadDisk();
}

void MakeImageDisk::ReadImageThreadProc(LPVOID *param)
{

}

void MakeImageDisk::WriteDiskThreadProc(LPVOID *param)
{

}

void MakeImageDisk::WriteImageThreadProc(LPVOID *param)
{
    MakeImageDisk *pMakeImage = (MakeImageDisk *)param;
    Sleep(1000);
    pMakeImage->WriteImage();
}


BOOL MakeImageDisk::ReadLocalImage()
{
    BOOL bResult = TRUE;
    DWORD dwErrorCode = 0;
    ErrorType errType = ErrorType_System;

    ULONGLONG ullReadSize = SIZEOF_IMAGE_HEADER;
    ULONGLONG ullOffset = SIZEOF_IMAGE_HEADER;
    DWORD dwLen = 0;

    // 计算精确速度
//    LARGE_INTEGER freq,t0,t1,t2,t3;
    double dbTimeNoWait = 0.0,dbTimeWait = 0.0;

//    QueryPerformanceFrequency(&freq);

    while (bResult && ullReadSize < m_ullImageSize)
    {
//        QueryPerformanceCounter(&t0);

        // 判断队列是否达到限制值
        while (IsReachLimitQty(MAX_LENGTH_OF_DATA_QUEUE))
        {
            Sleep(5);
        }

//        QueryPerformanceCounter(&t1);

        BYTE pkgHead[PKG_HEADER_SIZE] = {NULL};
        dwLen = PKG_HEADER_SIZE;
        if (!ReadFileAsyn(m_hImage,ullOffset,dwLen,pkgHead,&dwErrorCode))
        {
            bResult = FALSE;

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Read image file error,filename=%s,Speed=%.2f,system errorcode=%ld,%s")
//                ,m_MasterPort->GetFileName(),m_MasterPort->GetRealSpeed(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
            break;
        }

        dwLen = *(PDWORD)&pkgHead[8];

        PBYTE pByte = new BYTE[dwLen];
        ZeroMemory(pByte,dwLen);

        if (!ReadFileAsyn(m_hImage,ullOffset,dwLen,pByte,&dwErrorCode))
        {
            bResult = FALSE;
            delete []pByte;

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Read image file error,filename=%s,Speed=%.2f,system errorcode=%ld,%s")
//                ,m_MasterPort->GetFileName(),m_MasterPort->GetRealSpeed(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
            break;
        }

        if (pByte[dwLen-1] != END_FLAG)
        {
            errType = ErrorType_Custom;
            dwErrorCode = CustomError_Image_Format_Error;
            bResult = FALSE;
            delete []pByte;
            break;
        }

//        QueryPerformanceCounter(&t2);

        PDATA_INFO dataInfo = new DATA_INFO;
        ZeroMemory(dataInfo,sizeof(DATA_INFO));
        dataInfo->ullOffset = *(PULONGLONG)pByte;
        dataInfo->dwDataSize = dwLen - PKG_HEADER_SIZE - 1;
        dataInfo->pData = new BYTE[dataInfo->dwDataSize];
        memcpy(dataInfo->pData,&pByte[PKG_HEADER_SIZE],dataInfo->dwDataSize);

        if (m_bDataCompress)
        {
            m_CompressQueue.AddTail(dataInfo);
        }
        else
        {
            m_DataQueue.AddTail(dataInfo);

            EFF_DATA effData;
            effData.ullStartSector = dataInfo->ullOffset/BYTES_PER_SECTOR;
            effData.ullSectors = dataInfo->dwDataSize/BYTES_PER_SECTOR;
            effData.wBytesPerSector = BYTES_PER_SECTOR;
            m_EffList.push_back(effData);

//            m_pMasterHashMethod->update(dataInfo->pData,dataInfo->dwDataSize);
        }

        delete []pByte;

        ullOffset += dwLen;
        ullReadSize += dwLen;

//        QueryPerformanceCounter(&t3);
//        dbTimeNoWait = (double)(t2.QuadPart - t1.QuadPart) / (double)freq.QuadPart; // 秒
//        dbTimeWait = (double)(t3.QuadPart - t0.QuadPart) / (double)freq.QuadPart; // 秒
//        m_MasterPort->AppendUsedWaitTimeS(dbTimeWait);
//        m_MasterPort->AppendUsedNoWaitTimeS(dbTimeNoWait);

        // 因为是压缩数据，长度比实际长度短，所以要根据速度计算
//        m_MasterPort->SetCompleteSize(m_MasterPort->GetValidSize() * ullReadSize / m_ullImageSize);

    }


    // 先设置为停止状态
    if (bResult)
    {
//        m_MasterPort->SetPortState(PortState_Stop);
        std::cerr<<"read image successful !"<<endl;
    }
    else
    {
        std::cerr<<"read image failed !"<<endl;
    }

    return bResult;
}

BOOL MakeImageDisk::ReadRemoteImage()
{
//	BOOL bResult = TRUE;
//	DWORD dwErrorCode = 0;
//	ErrorType errType = ErrorType_System;

//	ULONGLONG ullReadSize = SIZEOF_IMAGE_HEADER;
//	DWORD dwLen = 0;

//	// 计算精确速度
//	LARGE_INTEGER freq,t0,t1,t2,t3;
//	double dbTimeNoWait = 0.0,dbTimeWait = 0.0;

//	WSAOVERLAPPED ol = {0};
//	ol.hEvent = WSACreateEvent();

//	QueryPerformanceFrequency(&freq);

//	m_MasterPort->Active();

//	// 等待其他线程创建好,最多等5次
//	Sleep(100);

//	int nTimes = 5;
//	while (!IsTargetsReady() && nTimes > 0)
//	{
//		Sleep(100);
//		nTimes--;
//	}

//	// 发送COPY IMAGE命令
//	CString strImageName = CUtils::GetFileName(m_MasterPort->GetFileName());

//	USES_CONVERSION;
//	char *fileName = W2A(strImageName);

//	DWORD dwSendLen = sizeof(CMD_IN) + strlen(fileName) + 2;
//	BYTE *sendBuf = new BYTE[dwSendLen];
//	ZeroMemory(sendBuf,dwSendLen);
//	sendBuf[dwSendLen - 1] = END_FLAG;

//	CMD_IN copyImageIn = {0};
//	copyImageIn.dwCmdIn = CMD_COPY_IMAGE_IN;
//	copyImageIn.dwSizeSend = dwSendLen;

//	memcpy(sendBuf,&copyImageIn,sizeof(CMD_IN));
//	memcpy(sendBuf + sizeof(CMD_IN),fileName,strlen(fileName));

//	while (bResult && !*m_lpCancel && ullReadSize < m_ullImageSize && m_MasterPort->GetPortState() == PortState_Active)
//	{
//		QueryPerformanceCounter(&t0);

//		// 判断队列是否达到限制值
//		while (IsReachLimitQty(MAX_LENGTH_OF_DATA_QUEUE)
//			&& !*m_lpCancel && !IsAllFailed(errType,&dwErrorCode))
//		{
//			//SwitchToThread();
//			Sleep(5);
//		}

//		if (*m_lpCancel)
//		{
//			dwErrorCode = CustomError_Cancel;
//			errType = ErrorType_Custom;
//			bResult = FALSE;
//			break;
//		}

//		if (IsAllFailed(errType,&dwErrorCode))
//		{
//			bResult = FALSE;
//			break;
//		}

//		QueryPerformanceCounter(&t1);

//		if (!Send(m_ClientSocket,(char *)sendBuf,dwSendLen,NULL,&dwErrorCode))
//		{
//			bResult = FALSE;

//			CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Send copy image command error,filename=%s,Speed=%.2f,system errorcode=%ld,%s")
//				,m_MasterPort->GetFileName(),m_MasterPort->GetRealSpeed(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

//			break;
//		}

//		CMD_OUT copyImageOut = {0};
//		dwLen = sizeof(CMD_OUT);
//		if (!Recv(m_ClientSocket,(char *)&copyImageOut,dwLen,&ol,&dwErrorCode))
//		{
//			bResult = FALSE;

//			CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Recv copy image error,filename=%s,Speed=%.2f,system errorcode=%ld,%s")
//				,strImageName,m_MasterPort->GetRealSpeed(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
//			break;
//		}

//		dwLen = copyImageOut.dwSizeSend - sizeof(CMD_OUT);

//		BYTE *pByte = new BYTE[dwLen];
//		ZeroMemory(pByte,dwLen);

//		DWORD dwRead = 0;

//		while(dwRead < dwLen)
//		{
//			DWORD dwByteRead = dwLen - dwRead;
//			if (!Recv(m_ClientSocket,(char *)(pByte+dwRead),dwByteRead,&ol,&dwErrorCode))
//			{
//				bResult = FALSE;

//				CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Recv copy image error,filename=%s,Speed=%.2f,system errorcode=%ld,%s")
//					,strImageName,m_MasterPort->GetRealSpeed(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
//				break;
//			}

//			dwRead += dwByteRead;
//		}

//		if (dwRead < dwLen)
//		{
//			bResult = FALSE;

//			delete []pByte;

//			break;
//		}

//		if (copyImageOut.dwErrorCode != 0)
//		{
//			bResult = FALSE;
//			errType = copyImageOut.errType;
//			dwErrorCode = copyImageOut.dwErrorCode;

//			delete []pByte;

//			CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Recv copy image error,filename=%s,Speed=%.2f,system errorcode=%d,%s")
//				,strImageName,m_MasterPort->GetRealSpeed(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
//			break;
//		}

//		if (copyImageOut.dwCmdOut != CMD_COPY_IMAGE_OUT || copyImageOut.dwSizeSend != dwLen + sizeof(CMD_OUT))
//		{
//			bResult = FALSE;
//			errType = ErrorType_Custom;
//			dwErrorCode = CustomError_Get_Data_From_Server_Error;

//			delete []pByte;

//			CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Recv copy image error,filename=%s,Speed=%.2f,custom errorcode=0x%X,get data from server error")
//				,strImageName,m_MasterPort->GetRealSpeed(),dwErrorCode);
//			break;
//		}


//		// 去除尾部标志
//		dwLen -= 1;

//		QueryPerformanceCounter(&t2);

//		PDATA_INFO dataInfo = new DATA_INFO;
//		ZeroMemory(dataInfo,sizeof(DATA_INFO));

//		dataInfo->ullOffset = *(PULONGLONG)pByte;
//		dataInfo->dwDataSize = dwLen - PKG_HEADER_SIZE - 1;
//		dataInfo->pData = new BYTE[dataInfo->dwDataSize];
//		memcpy(dataInfo->pData,&pByte[PKG_HEADER_SIZE],dataInfo->dwDataSize);

//		if (m_bDataCompress)
//		{
//			m_CompressQueue.AddTail(dataInfo);
//		}
//		else
//		{
//			AddDataQueueList(dataInfo);

//			m_pMasterHashMethod->update(dataInfo->pData,dataInfo->dwDataSize);

//			delete []dataInfo->pData;
//			delete dataInfo;
//		}


//		// 写文件
//		if (m_hDisk != INVALID_HANDLE_VALUE)
//		{
//			WriteFileAsyn(m_hDisk,ullReadSize,dwLen,pByte,m_MasterPort->GetOverlapped(FALSE),&dwErrorCode);
//		}


//		dwErrorCode = 0;

//		delete []pByte;

//		ullReadSize += dwLen;

//		QueryPerformanceCounter(&t3);

//		dbTimeNoWait = (double)(t2.QuadPart - t1.QuadPart) / (double)freq.QuadPart; // 秒
//		dbTimeWait = (double)(t3.QuadPart - t0.QuadPart) / (double)freq.QuadPart; // 秒
//		m_MasterPort->AppendUsedWaitTimeS(dbTimeWait);
//		m_MasterPort->AppendUsedNoWaitTimeS(dbTimeNoWait);

//		// 因为是压缩数据，长度比实际长度短，所以要根据速度计算
//		m_MasterPort->SetCompleteSize(m_MasterPort->GetValidSize() * ullReadSize / m_ullImageSize);

//		/*
//		if (IsAllFailed(errType,&dwErrorCode))
//		{
//			bResult = FALSE;
//			break;
//		}
//		*/

//	}

//	WSACloseEvent(ol.hEvent);

//	if (!bResult)
//	{
//		// 发送停止命令
//		copyImageIn.byStop = TRUE;
//		memcpy(sendBuf,&copyImageIn,sizeof(CMD_IN));
//		memcpy(sendBuf + sizeof(CMD_IN),fileName,strlen(fileName));

//		DWORD dwError = 0;

//		Send(m_ClientSocket,(char *)sendBuf,dwSendLen,NULL,&dwError);
//	}

//	delete []sendBuf;

//	if (*m_lpCancel)
//	{
//		bResult = FALSE;
//		dwErrorCode = CustomError_Cancel;
//		errType = ErrorType_Custom;

//		CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Read image file error,filename=%s,Speed=%.2f,custom errorcode=0x%X,user cancelled.")
//			,m_MasterPort->GetFileName(),m_MasterPort->GetRealSpeed(),dwErrorCode);
//	}

//	// 先设置为停止状态
//	// 先设置为停止状态
//	if (bResult)
//	{
//		m_MasterPort->SetPortState(PortState_Stop);
//	}
//	else
//	{
//		m_MasterPort->SetResult(FALSE);
//		m_MasterPort->SetPortState(PortState_Fail);
//		m_MasterPort->SetErrorCode(errType,dwErrorCode);
//	}


//	// 所有数据都拷贝完
//	while (!m_bCompressComplete)
//	{
//		//SwitchToThread();
//		Sleep(100);
//	}

//	if (!m_MasterPort->GetResult())
//	{
//		bResult = FALSE;
//		errType = m_MasterPort->GetErrorCode(&dwErrorCode);
//	}

//	m_MasterPort->SetEndTime(CTime::GetCurrentTime());

//	if (bResult)
//	{
//		if (m_bComputeHash)
//		{
//			m_MasterPort->SetHash(m_pMasterHashMethod->digest(),m_pMasterHashMethod->getHashLength());

//			CString strImageHash;
//			for (int i = 0; i < m_pMasterHashMethod->getHashLength();i++)
//			{
//				CString strHash;
//				strHash.Format(_T("%02X"),m_pMasterHashMethod->digest()[i]);
//				m_strMasterHash += strHash;

//				strHash.Format(_T("%02X"),m_ImageHash[i]);
//				strImageHash += strHash;
//			}

//			CString strHashMethod(m_pMasterHashMethod->getHashMetod());

//			// 此处加入判断IMAGE解压过程中是否出错
//			if (strImageHash.CompareNoCase(m_strMasterHash) != 0)
//			{
//				CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Image,%s - %s,Image hash value was changed,Compute=%s,Record=%s")
//					,m_MasterPort->GetFileName(),strHashMethod,m_strMasterHash,strImageHash);

//				bResult = FALSE;
//				m_MasterPort->SetResult(FALSE);
//				m_MasterPort->SetPortState(PortState_Fail);
//				m_MasterPort->SetErrorCode(ErrorType_Custom,CustomError_Image_Hash_Value_Changed);
//			}
//			else
//			{
//				m_MasterPort->SetResult(TRUE);
//				m_MasterPort->SetPortState(PortState_Pass);
//				m_MasterPort->SetErrorCode(errType,dwErrorCode);

//				CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Image,%s - %s,HashValue=%s")
//					,m_MasterPort->GetFileName(),strHashMethod,m_strMasterHash);
//			}

//		}
//		else
//		{
//			m_MasterPort->SetResult(TRUE);
//			m_MasterPort->SetPortState(PortState_Pass);
//			m_MasterPort->SetErrorCode(errType,dwErrorCode);
//		}
//	}
//	else
//	{
//		m_MasterPort->SetResult(FALSE);
//		m_MasterPort->SetPortState(PortState_Fail);
//		m_MasterPort->SetErrorCode(errType,dwErrorCode);
//	}

//	return bResult;
}


ULONGLONG MakeImageDisk::ullSectorNums() const
{
    return m_ullSectorNums;
}

void MakeImageDisk::setUllSectorNums(const ULONGLONG &ullSectorNums)
{
    m_ullSectorNums = ullSectorNums;
}

DWORD MakeImageDisk::dwBytesPerSector() const
{
    return m_dwBytesPerSector;
}

void MakeImageDisk::setDwBytesPerSector(const DWORD &dwBytesPerSector)
{
    m_dwBytesPerSector = dwBytesPerSector;
}

DWORD MakeImageDisk::nBlockSectors() const
{
    return m_nBlockSectors;
}

void MakeImageDisk::setNBlockSectors(const DWORD &nBlockSectors)
{
    m_nBlockSectors = nBlockSectors;
}



BOOL MakeImageDisk::WriteImage()
{
    if (m_bServerFirst)
    {
        return WriteRemoteImage();
    }
    else
    {
        return WriteLocalImage();
    }
}

BOOL MakeImageDisk::WriteLocalImage()
{
    BOOL bResult = TRUE;
    DWORD dwErrorCode = 0;
    ErrorType errType = ErrorType_System;
    ULONGLONG ullPkgIndex = 0;
    ULONGLONG ullOffset = SIZEOF_IMAGE_HEADER;
    DWORD dwLen = 0;

    CDataQueue *pDataQueue = &m_DataQueue;

    if (m_hImage)
    {
        std::cerr<<"image handle is failed! "<<endl;
//        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Create image file error,filename=%s,system errorcode=%ld,%s")
//            ,port->GetFileName(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

        return FALSE;
    }

    // 计算精确速度
    double dbTimeNoWait = 0.0,dbTimeWait = 0.0;


    while (!m_bReadDiskOver)
    {
//        QueryPerformanceCounter(&t0);
        while(pDataQueue->GetCount() <=0 )
        {
            Sleep(5);
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

        if (!WriteFileAsyn(m_hImage,ullOffset,dwLen,dataInfo->pData,&dwErrorCode))
        {
            bResult = FALSE;

            delete []dataInfo->pData;
            delete dataInfo;

            break;
        }

        ullOffset += dwLen;

        delete []dataInfo->pData;
        delete dataInfo;
    }

    // 写IMAGE头
    if (bResult)
    {
        ULONGLONG ullSize = 0;
        GetFileSize(m_hImage,ullSize);


        // 随机生成一个数作为IMAGEID

        IMAGE_HEADER imgHead;
        ZeroMemory(&imgHead,sizeof(IMAGE_HEADER));
        memcpy(imgHead.szImageFlag,IMAGE_FLAG,strlen(IMAGE_FLAG));
        imgHead.ullImageSize = ullSize;
        memcpy(imgHead.szAppVersion,APP_VERSION,strlen(APP_VERSION));


        imgHead.dwImageType = QUICK_IMAGE;


        imgHead.dwMaxSizeOfPackage = MAX_COMPRESS_BUF;
        imgHead.ullCapacitySize = 0xffffffffffffffff;
        imgHead.dwBytesPerSector = m_dwBytesPerSector;
        memcpy(imgHead.szZipVer,ZIP_VERSION,strlen(ZIP_VERSION));
        imgHead.byUnCompress = m_bDataCompress ? 0 : 1;
        imgHead.ullPkgCount = ullPkgIndex;
        imgHead.ullValidSize = m_ullValidSize;
//        imgHead.dwHashLen = m_pMasterHashMethod->getHashLength();
//        imgHead.dwHashType = m_MasterPort->GetHashMethod();
//        memcpy(imgHead.byImageDigest,m_pMasterHashMethod->digest(),m_pMasterHashMethod->getHashLength());
        imgHead.byEnd = END_FLAG;

        dwLen = SIZEOF_IMAGE_HEADER;

        if (!WriteFileAsyn(m_hImage,0,dwLen,(LPBYTE)&imgHead,&dwErrorCode))
        {
            bResult = FALSE;

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Write image head error,filename=%s,Speed=%.2f,system errorcode=%ld,%s")
//                ,port->GetFileName(),port->GetRealSpeed(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
        }
    }

    return bResult;
}

BOOL MakeImageDisk::WriteRemoteImage()
{
//	BOOL bResult = TRUE;
//	DWORD dwErrorCode = 0;
//	ErrorType errType = ErrorType_System;
//	ULONGLONG ullPkgIndex = 0;
//	ULONGLONG ullOffset = SIZEOF_IMAGE_HEADER;
//	DWORD dwLen = 0;

//	port->Active();

//	CString strImageName = CUtils::GetFileName(port->GetFileName());

//	USES_CONVERSION;
//	char *fileName = W2A(strImageName);

//	CMD_IN makeImageIn = {0};
//	makeImageIn.dwCmdIn = CMD_MAKE_IMAGE_IN;
//	makeImageIn.byStop = FALSE;

//	WSAOVERLAPPED olSend = {0};
//	olSend.hEvent = WSACreateEvent();

//	WSAOVERLAPPED olRecv = {0};
//	olRecv.hEvent = WSACreateEvent();

//	// 计算精确速度
//	LARGE_INTEGER freq,t0,t1,t2;
//	double dbTimeNoWait = 0.0,dbTimeWait = 0.0;

//	QueryPerformanceFrequency(&freq);

//	while (!*m_lpCancel && m_MasterPort->GetResult() && port->GetResult())
//	{
//		//dwTimeWait = timeGetTime();
//		QueryPerformanceCounter(&t0);
//		while(pDataQueue->GetCount() <=0 && !*m_lpCancel && m_MasterPort->GetResult()
//			&& (m_MasterPort->GetPortState() == PortState_Active || !m_bCompressComplete)
//			&& port->GetResult())
//		{
//			//SwitchToThread();
//			Sleep(5);
//		}

//		if (!m_MasterPort->GetResult())
//		{
//			errType = m_MasterPort->GetErrorCode(&dwErrorCode);
//			bResult = FALSE;
//			break;
//		}

//		if (!port->GetResult())
//		{
//			errType = port->GetErrorCode(&dwErrorCode);
//			bResult = FALSE;
//			break;
//		}

//		if (*m_lpCancel)
//		{
//			dwErrorCode = CustomError_Cancel;
//			errType = ErrorType_Custom;
//			bResult = FALSE;
//			break;
//		}

//		if (pDataQueue->GetCount() <= 0 && m_MasterPort->GetPortState() != PortState_Active && m_bCompressComplete)
//		{
//			dwErrorCode = 0;
//			bResult = TRUE;
//			break;
//		}

//		PDATA_INFO dataInfo = pDataQueue->GetHeadRemove();

//		if (dataInfo == NULL)
//		{
//			continue;
//		}

//		if (m_bDataCompress)
//		{
//			*(PULONGLONG)dataInfo->pData = ullPkgIndex;
//		}

//		ullPkgIndex++;

//		QueryPerformanceCounter(&t1);

//		dwLen = sizeof(CMD_IN) + strlen(fileName) + 2 + dataInfo->dwDataSize;

//		makeImageIn.dwSizeSend = dwLen;

//		BYTE *pByte = new BYTE[dwLen];
//		ZeroMemory(pByte,dwLen);
//		memcpy(pByte,&makeImageIn,sizeof(CMD_IN));
//		memcpy(pByte+sizeof(CMD_IN),fileName,strlen(fileName));
//		memcpy(pByte+sizeof(CMD_IN)+strlen(fileName)+1,dataInfo->pData,dataInfo->dwDataSize);

//		if (!Send(m_ClientSocket,(char *)pByte,dwLen,&olSend,&dwErrorCode))
//		{
//			delete []pByte;
//			bResult = FALSE;

//			delete []dataInfo->pData;
//			delete dataInfo;

//			CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Send write image file error,filename=%s,Speed=%.2f,system errorcode=%ld,%s")
//				,strImageName,port->GetRealSpeed(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

//			break;
//		}

//		delete []pByte;

//		// 读返回值
//		CMD_OUT makeImageOut = {0};
//		dwLen = sizeof(CMD_OUT);
//		if (!Recv(m_ClientSocket,(char *)&makeImageOut,dwLen,&olRecv,&dwErrorCode))
//		{
//			bResult = FALSE;

//			delete []dataInfo->pData;
//			delete dataInfo;

//			CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Recv write image file error,filename=%s,Speed=%.2f,system errorcode=%ld,%s")
//				,strImageName,port->GetRealSpeed(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

//			break;
//		}

//		if (makeImageOut.dwErrorCode != 0)
//		{
//			dwErrorCode = makeImageOut.dwErrorCode;
//			bResult = FALSE;

//			delete []dataInfo->pData;
//			delete dataInfo;

//			CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Recv write image file error,filename=%s,Speed=%.2f,system errorcode=%ld,%s")
//				,strImageName,port->GetRealSpeed(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

//			break;
//		}

//		if (makeImageOut.dwCmdOut != CMD_MAKE_IMAGE_OUT || makeImageOut.dwSizeSend != dwLen)
//		{
//			dwErrorCode = CustomError_Get_Data_From_Server_Error;
//			errType = ErrorType_Custom;
//			bResult = FALSE;

//			delete []dataInfo->pData;
//			delete dataInfo;

//			CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Recv write image file error,filename=%s,Speed=%.2f,custom errorcode=0x%X,get data from server error")
//				,strImageName,port->GetRealSpeed(),dwErrorCode);

//			break;
//		}

//		//DWORD dwTime = timeGetTime();
//		QueryPerformanceCounter(&t2);

//		dbTimeWait = (double)(t2.QuadPart - t0.QuadPart) / (double)freq.QuadPart;

//		dbTimeNoWait = (double)(t2.QuadPart - t1.QuadPart) / (double)freq.QuadPart;

//		ullOffset += dataInfo->dwDataSize;

//		port->AppendUsedNoWaitTimeS(dbTimeNoWait);
//		port->AppendUsedWaitTimeS(dbTimeNoWait);

//		// 压缩的数据比实际数据短，不能取压缩后的长度，要不压缩之前的长度
//		port->AppendCompleteSize(dataInfo->dwOldSize);

//		delete []dataInfo->pData;
//		delete dataInfo;

//	}

//	if (*m_lpCancel)
//	{
//		bResult = FALSE;
//		dwErrorCode = CustomError_Cancel;
//		errType = ErrorType_Custom;

//		CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Write image file error,filename=%s,Speed=%.2f,custom errorcode=0x%X,user cancelled.")
//			,port->GetFileName(),port->GetRealSpeed(),dwErrorCode);
//	}

//	if (!m_MasterPort->GetResult())
//	{
//		bResult = FALSE;
//		errType = m_MasterPort->GetErrorCode(&dwErrorCode);
//	}

//	if (!port->GetResult())
//	{
//		errType = port->GetErrorCode(&dwErrorCode);
//		bResult = FALSE;
//	}

//	// 写IMAGE头
//	if (bResult)
//	{

//		IMAGE_HEADER imgHead;
//		ZeroMemory(&imgHead,sizeof(IMAGE_HEADER));
//		memcpy(imgHead.szImageFlag,IMAGE_FLAG,strlen(IMAGE_FLAG));
//		imgHead.ullImageSize = ullOffset;
//		memcpy(imgHead.szAppVersion,APP_VERSION,strlen(APP_VERSION));

//		if (m_nImageType)
//		{
//			imgHead.dwImageType = QUICK_IMAGE;
//		}
//		else
//		{
//			imgHead.dwImageType = FULL_IMAGE;
//		}

//		imgHead.dwMaxSizeOfPackage = MAX_COMPRESS_BUF;
//		imgHead.ullCapacitySize = m_MasterPort->GetTotalSize();
//		imgHead.dwBytesPerSector = m_MasterPort->GetBytesPerSector();
//		memcpy(imgHead.szZipVer,ZIP_VERSION,strlen(ZIP_VERSION));
//		imgHead.byUnCompress = m_bDataCompress ? 0 : 1;
//		imgHead.ullPkgCount = ullPkgIndex;
//		imgHead.ullValidSize = m_MasterPort->GetValidSize();
//		imgHead.dwHashLen = m_pMasterHashMethod->getHashLength();
//		imgHead.dwHashType = m_MasterPort->GetHashMethod();
//		memcpy(imgHead.byImageDigest,m_pMasterHashMethod->digest(),m_pMasterHashMethod->getHashLength());
//		imgHead.byEnd = END_FLAG;

//		dwLen = sizeof(CMD_IN) + strlen(fileName) + 2 + SIZEOF_IMAGE_HEADER;

//		makeImageIn.dwSizeSend = dwLen;

//		BYTE *pByte = new BYTE[dwLen];
//		ZeroMemory(pByte,dwLen);
//		pByte[dwLen - 1] = END_FLAG;

//		memcpy(pByte,&makeImageIn,sizeof(CMD_IN));
//		memcpy(pByte+sizeof(CMD_IN),fileName,strlen(fileName));
//		memcpy(pByte+sizeof(CMD_IN)+strlen(fileName)+1,&imgHead,SIZEOF_IMAGE_HEADER);

//		if (!Send(m_ClientSocket,(char *)pByte,dwLen,&olSend,&dwErrorCode))
//		{
//			delete []pByte;
//			bResult = FALSE;
//			CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Send write image file error,filename=%s,Speed=%.2f,system errorcode=%ld,%s")
//				,strImageName,port->GetRealSpeed(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
//			goto END;
//		}

//		delete []pByte;

//		// 读返回值
//		CMD_OUT makeImageOut = {0};
//		dwLen = sizeof(CMD_OUT);
//		if (!Recv(m_ClientSocket,(char *)&makeImageOut,dwLen,&olRecv,&dwErrorCode))
//		{
//			bResult = FALSE;
//			CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Recv write image file error,filename=%s,Speed=%.2f,system errorcode=%ld,%s")
//				,strImageName,port->GetRealSpeed(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

//			goto END;
//		}

//		if (makeImageOut.dwErrorCode != 0)
//		{
//			dwErrorCode = makeImageOut.dwErrorCode;
//			bResult = FALSE;
//			CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Recv write image file error,filename=%s,Speed=%.2f,system errorcode=%ld,%s")
//				,strImageName,port->GetRealSpeed(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

//			goto END;
//		}

//		if (makeImageOut.dwCmdOut != CMD_MAKE_IMAGE_OUT || makeImageOut.dwSizeSend != dwLen)
//		{
//			dwErrorCode = CustomError_Get_Data_From_Server_Error;
//			errType = ErrorType_Custom;
//			bResult = FALSE;
//			CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Recv write image file error,filename=%s,Speed=%.2f,custom errorcode=0x%X,get data from server error")
//				,strImageName,port->GetRealSpeed(),dwErrorCode);

//			goto END;
//		}
//	}

//END:
//	WSACloseEvent(olSend.hEvent);
//	WSACloseEvent(olRecv.hEvent);

//	port->SetResult(bResult);
//	port->SetEndTime(CTime::GetCurrentTime());

//	if (bResult)
//	{
//		port->SetPortState(PortState_Pass);
//	}
//	else
//	{
//		port->SetPortState(PortState_Fail);
//		port->SetErrorCode(errType,dwErrorCode);
//	}

    //	return bResult;
}
#include <sys/stat.h>
BOOL MakeImageDisk::GetFileSize(int fd, ULONGLONG &ullSize)
{
    struct stat statbuff;
    if(fstat(fd, &statbuff) < 0){

        return FALSE;
    }else{
        ullSize = statbuff.st_size;
    }
    return TRUE;
}


BOOL MakeImageDisk::BriefAnalyze()
{
    DWORD dwErrorCode = 0;

    BYTE *pXBR = new BYTE[m_dwBytesPerSector];
    ZeroMemory(pXBR,m_dwBytesPerSector);
    if (!ReadSectors(m_hDisk,0,1,m_dwBytesPerSector,pXBR,&dwErrorCode))
    {
//        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Read sector 0 error,system errorcode=%ld,%s")
//            ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

        delete []pXBR;
        pXBR = NULL;

        return FALSE;
    }

    // 第一步，判断0号扇区是MBR，还是DBR？
    BootSector bootSector = GetBootSectorType(pXBR);

    // 第二步，判断分区体系
    PartitionStyle partitionStyle = GetPartitionStyle(pXBR,bootSector);

    // 第三步，判断文件系统
    FileSystem fileSystem = FileSystem_UNKNOWN;
    ULONGLONG ullStartSectors = 0;

    switch (partitionStyle)
    {
    case PartitionStyle_UNKNOWN:
        fileSystem = GetFileSystem(pXBR,ullStartSectors);

        if (!AppendEffDataList(pXBR,fileSystem,ullStartSectors,ullStartSectors,m_ullSectorNums,FALSE))
        {
            delete []pXBR;
            pXBR = NULL;

            return FALSE;
        }
        break;

    case PartitionStyle_DOS:
        {
            fileSystem = FileSystem_EXTEND;
            if (!AppendEffDataList(pXBR,fileSystem,ullStartSectors,ullStartSectors,m_ullSectorNums,TRUE))
            {
                delete []pXBR;
                pXBR = NULL;

                return FALSE;
            }
        }
        break;

    case PartitionStyle_GPT:
        {
            BYTE *pEFI = new BYTE[m_dwBytesPerSector];
            ZeroMemory(pEFI,m_dwBytesPerSector);

            if (!ReadSectors(m_hDisk,1,1,m_dwBytesPerSector,pEFI,&dwErrorCode))
            {
//                m_MasterPort->SetEndTime(CTime::GetCurrentTime());
//                m_MasterPort->SetPortState(PortState_Fail);
//                m_MasterPort->SetResult(FALSE);
//                m_MasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Read EFI info error,system errorcode=%ld,%s")
//                    ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

                delete []pEFI;
                pEFI = NULL;

                delete []pXBR;
                pXBR = NULL;

                return FALSE;
            }

            EFI_HEAD efiHead;
            ZeroMemory(&efiHead,sizeof(EFI_HEAD));
            memcpy(&efiHead,pEFI,sizeof(EFI_HEAD));

            delete []pEFI;
            pEFI = NULL;

            // 添加保护MBR、EFI信息（GPT头）、分区表，34个分区
            EFF_DATA effData;
            effData.ullStartSector = 0;
            effData.ullSectors = efiHead.FirstUsableLBA;
            effData.wBytesPerSector = (WORD)m_dwBytesPerSector;
            m_EffList.push_back(effData);

            // 读分区表项
            DWORD dwPatitionEntrySectors = efiHead.MaxNumberOfPartitionEntries * efiHead.SizeOfPartitionEntry / m_dwBytesPerSector;
            ULONGLONG ullStartSectors = efiHead.PartitionEntryLBA;
            DWORD dwEntryIndex = 1;
            for (DWORD i = 0;i < dwPatitionEntrySectors;i++)
            {
                BYTE *pByte = new BYTE[m_dwBytesPerSector];
                ZeroMemory(pByte,m_dwBytesPerSector);
                if (!ReadSectors(m_hDisk,ullStartSectors+i,1,m_dwBytesPerSector,pByte,&dwErrorCode))
                {
//                    m_MasterPort->SetEndTime(CTime::GetCurrentTime());
//                    m_MasterPort->SetPortState(PortState_Fail);
//                    m_MasterPort->SetResult(FALSE);
//                    m_MasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//                    CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - StartSector:%ld - Read GUID Partition Table Entry #%d erorr,system errorcode=%ld,%s")
//                        ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum(),ullStartSectors,dwEntryIndex,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

                    delete []pByte;
                    pByte = NULL;

                    delete []pXBR;
                    pXBR = NULL;

                    return FALSE;
                }

                DWORD dwEntryNumsPerSector = m_dwBytesPerSector/efiHead.SizeOfPartitionEntry;
                GPT_PARTITION_ENTRY *gptPartitionEntry = new GPT_PARTITION_ENTRY[dwEntryNumsPerSector];
                ZeroMemory(gptPartitionEntry,sizeof(GPT_PARTITION_ENTRY) * dwEntryNumsPerSector);
                memcpy(gptPartitionEntry,pByte,sizeof(GPT_PARTITION_ENTRY) * dwEntryNumsPerSector);

                delete []pByte;
                pByte = NULL;

                for (DWORD entry = 0; entry < dwEntryNumsPerSector;entry++)
                {
                    ULONGLONG ullTempStartSector = gptPartitionEntry[entry].StartingLBA;
                    ULONGLONG ullSectors = gptPartitionEntry[entry].EndingLBA - gptPartitionEntry[entry].StartingLBA;

                    if (ullSectors == 0)
                    {
                        continue;
                    }

                    /* Windows保留分区可以拷贝可以不拷贝
                    if (ullTempStartSector == 0)
                    {
                    dwEntryIndex++;
                    continue;
                    }
                    */

                    BYTE *pTempDBR = new BYTE[m_dwBytesPerSector];
                    ZeroMemory(pTempDBR,m_dwBytesPerSector);
                    if (!ReadSectors(m_hDisk,ullTempStartSector,1,m_dwBytesPerSector,pTempDBR,&dwErrorCode))
                    {
//                        m_MasterPort->SetEndTime(CTime::GetCurrentTime());
//                        m_MasterPort->SetPortState(PortState_Fail);
//                        m_MasterPort->SetResult(FALSE);
//                        m_MasterPort->SetErrorCode(ErrorType_System,dwErrorCode);


//                        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - StartSector:%ld - Read GUID Partition Table Entry #%d erorr,system errorcode=%ld,%s")
//                            ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum(),ullTempStartSector,dwEntryIndex,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

                        delete []pTempDBR;
                        pTempDBR = NULL;

                        delete []pXBR;
                        pXBR = NULL;

                        delete []gptPartitionEntry;
                        gptPartitionEntry = NULL;

                        return FALSE;
                    }

                    fileSystem = GetFileSystem(pTempDBR,ullTempStartSector);

                    if (!AppendEffDataList(pTempDBR,fileSystem,ullTempStartSector,ullTempStartSector,ullSectors,FALSE))
                    {
                        delete []pTempDBR;
                        pTempDBR = NULL;

                        delete []pXBR;
                        pXBR = NULL;

                        delete []gptPartitionEntry;
                        gptPartitionEntry = NULL;

                        return FALSE;
                    }

                    delete []pTempDBR;
                    pTempDBR = NULL;

                    dwEntryIndex++;
                }

                delete []gptPartitionEntry;
                gptPartitionEntry = NULL;
            }


            // 添加分区表备份分区表
            effData.ullStartSector = efiHead.LastUsableLBA;
            effData.ullSectors = efiHead.MaxNumberOfPartitionEntries * efiHead.SizeOfPartitionEntry / m_dwBytesPerSector;
            effData.wBytesPerSector = (WORD)m_dwBytesPerSector;
            m_EffList.push_back(effData);

            // 添加备份EFI信息
            effData.ullStartSector = efiHead.BackupLBA;
            effData.ullSectors = 1;
            effData.wBytesPerSector = (WORD)m_dwBytesPerSector;
            m_EffList.push_back(effData);
        }
        break;
    }

    delete []pXBR;
    pXBR = NULL;

    return TRUE;
}



BootSector MakeImageDisk::GetBootSectorType( const PBYTE pXBR )
{
    /* 用此法判断是否DBR有待确定
    // EB 3C 90 -> FAT12/16
    // EB 58 90 -> FAT32
    // EB 52 90 -> NTFS
    // EB 76 90 -> EXFAT
    */
    MASTER_BOOT_RECORD mbr;
    int mbrsize = sizeof(MASTER_BOOT_RECORD);
    ZeroMemory(&mbr,mbrsize);
    memcpy(&mbr,pXBR,mbrsize);

    if (mbr.Signature != 0xAA55)
    {
//		CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetBootSectorType=BootSector_UNKNOWN"));
        return BootSector_UNKNOWN;
    }

    if (ReadOffset(pXBR,0,3) == 0x000000)
    {
//        CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetBootSectorType=BootSector_MBR"));
        return BootSector_MBR;
    }

    ULONGLONG ullStartSector = 0;
    DWORD dwErrorCode = 0;
    for (int i = 0;i < 4;i++)
    {
        ullStartSector = mbr.Partition[i].StartLBA;
        if (ullStartSector == 0)
        {
            continue;
        }

//        if (ullStartSector > m_ullSectorNums - 1)
//        {
//            CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetBootSectorType=BootSector_DBR"));
//            return BootSector_DBR;
//        }

        if (mbr.Partition[i].StartLBA == 0x01 && mbr.Partition[i].TotalSector == 0xFFFFFFFF
            && mbr.Partition[i].PartitionType == 0xEE)
        {
//            CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetBootSectorType=BootSector_MBR"));
            return BootSector_MBR;
        }

        BYTE *pDBRTemp = new BYTE[m_dwBytesPerSector];
        ZeroMemory(pDBRTemp,m_dwBytesPerSector);

        if (!ReadSectors(m_hDisk,ullStartSector,1,m_dwBytesPerSector,pDBRTemp,&dwErrorCode))
        {
            delete []pDBRTemp;
            pDBRTemp = NULL;

//            CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetBootSectorType=BootSector_DBR"));
            return BootSector_DBR;
        }

        if (GetFileSystem(pDBRTemp,ullStartSector) != FileSystem_UNKNOWN)
        {
            delete []pDBRTemp;
            pDBRTemp = NULL;

//            CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetBootSectorType=BootSector_MBR"));
            return BootSector_MBR;
        }
        else
        {
            delete []pDBRTemp;
            pDBRTemp = NULL;

//            CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetBootSectorType=BootSector_DBR"));
            return BootSector_DBR;
        }
    }

//    CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetBootSectorType=BootSector_DBR"));
    return BootSector_DBR;
}

#include <assert.h>
ULONGLONG MakeImageDisk::ReadOffset( const PBYTE pByte,DWORD offset,BYTE bytes )
{
    assert(bytes <= 8 && bytes > 0);
    ULONGLONG ullMask = 0xFFFFFFFFFFFFFFFF;
    ullMask = (ullMask >> (sizeof(ULONGLONG) - sizeof(BYTE) * bytes) * 8);

    ULONGLONG ullValue = *(PULONGLONG)(pByte+offset) & ullMask;
    return ullValue;
}

FileSystem MakeImageDisk::GetFileSystem( const PBYTE pDBR,ULONGLONG ullStartSector )
{
    FileSystem fileSystem = FileSystem_UNKNOWN;
    if (ReadOffset(pDBR,3,4) == 0x5346544E) //'NTFS'
    {
        fileSystem = FileSystem_NTFS;
//		CUtils::WriteLogFile(m_hLogFile,FALSE,_T("Port %s,Disk %d - GetFileSystem=FileSystem_NTFS")
//			,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum());
    }
    else if (ReadOffset(pDBR,3,5)== 0x5441465845) //'EXFAT'
    {
        fileSystem = FileSystem_EXFAT;
//        CUtils::WriteLogFile(m_hLogFile,FALSE,_T("Port %s,Disk %d - GetFileSystem=FileSystem_EXFAT")
//            ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum());
    }
    else if (ReadOffset(pDBR,0x36,5) == 0x3231544146) //'FAT12'
    {
        fileSystem = FileSystem_FAT12;
//        CUtils::WriteLogFile(m_hLogFile,FALSE,_T("Port %s,Disk %d - GetFileSystem=FileSystem_FAT12")
//            ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum());
    }
    else if (ReadOffset(pDBR,0x36,5) == 0x3631544146) //'FAT16'
    {
        fileSystem = FileSystem_FAT16;
//        CUtils::WriteLogFile(m_hLogFile,FALSE,_T("Port %s,Disk %d - GetFileSystem=FileSystem_FAT16")
//            ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum());
    }
    else if (ReadOffset(pDBR,0x52,5) == 0x3233544146) //'FAT32'
    {
        fileSystem = FileSystem_FAT32;
//        CUtils::WriteLogFile(m_hLogFile,FALSE,_T("Port %s,Disk %d - GetFileSystem=FileSystem_FAT32")
//            ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum());
    }
    else
    {
        // 偏移2个扇区，确定是否是EXT_X文件系统
        BYTE *pByte = new BYTE[BYTES_PER_SECTOR];
        ZeroMemory(pByte,BYTES_PER_SECTOR);

        DWORD dwErrorCode = 0;
        if (ReadSectors(m_hDisk,ullStartSector + 2,1,BYTES_PER_SECTOR,pByte,&dwErrorCode))
        {
            if (ReadOffset(pByte,0x38,2) == 0xEF53)
            {
                fileSystem = FileSystem_EXT_X;
//                CUtils::WriteLogFile(m_hLogFile,FALSE,_T("Port %s,Disk %d - GetFileSystem=FileSystem_EXT_X")
//                    ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum());
            }
            else
            {
                fileSystem = FileSystem_UNKNOWN;
//                CUtils::WriteLogFile(m_hLogFile,FALSE,_T("Port %s,Disk %d - GetFileSystem=FileSystem_UNKNOWN")
//                    ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum());
            }
        }
        else
        {
            fileSystem = FileSystem_UNKNOWN;
//            CUtils::WriteLogFile(m_hLogFile,FALSE,_T("Port %s,Disk %d - GetFileSystem=FileSystem_UNKNOWN")
//                ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum());
        }
        delete []pByte;
        pByte = NULL;
    }

    return fileSystem;
}


PartitionStyle MakeImageDisk::GetPartitionStyle( const PBYTE pByte,BootSector bootSector )
{
    PartitionStyle partitionStyle = PartitionStyle_UNKNOWN;
    if (bootSector == BootSector_MBR)
    {
        MASTER_BOOT_RECORD mbr;
        ZeroMemory(&mbr,sizeof(MASTER_BOOT_RECORD));
        memcpy(&mbr,pByte,sizeof(MASTER_BOOT_RECORD));

        for (int i = 0;i < 4;i++)
        {
            if (mbr.Partition[i].StartLBA == 0)
            {
                continue;
            }

            if (mbr.Partition[i].StartLBA == 0x01 && mbr.Partition[i].TotalSector == 0xFFFFFFFF
                && mbr.Partition[i].PartitionType == 0xEE)
            {
                partitionStyle = PartitionStyle_GPT;
//                CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetPartitionStyle=PartitionStyle_GPT"));
                break;
            }

            if (mbr.Partition[i].PartitionType == 0xEE)
            {
                partitionStyle = PartitionStyle_GPT;
//                CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetPartitionStyle=PartitionStyle_GPT"));
                break;
            }

            partitionStyle = PartitionStyle_DOS;
//            CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetPartitionStyle=PartitionStyle_DOS"));
            break;

        }
    }
    else
    {
        if (ReadOffset(pByte,0,2) == 0x5245)
        {
            partitionStyle = PartitionStyle_APPLE;
//            CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetPartitionStyle=PartitionStyle_APPLE"));
        }
        else if (ReadOffset(pByte,0,4) == 0x82564557)
        {
            partitionStyle = PartitionStyle_BSD;
//            CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetPartitionStyle=PartitionStyle_BSD"));
        }
        else
        {
            partitionStyle = PartitionStyle_UNKNOWN;
//            CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetPartitionStyle=PartitionStyle_UNKNOWN"));
        }
    }

    return partitionStyle;
}



BOOL MakeImageDisk::AppendEffDataList( const PBYTE pDBR,FileSystem fileSystem,ULONGLONG ullStartSector,ULONGLONG ullMasterSectorOffset,ULONGLONG ullSectors ,BOOL bMBR)
{
    DWORD dwErrorCode = 0;
    switch (fileSystem)
    {
    case FileSystem_FAT12:
    case FileSystem_FAT16:
    case FileSystem_FAT32:
        {
            FAT_INFO fatInfo;
            ZeroMemory(&fatInfo,sizeof(FAT_INFO));

            fatInfo.wBytesPerSector = (WORD)ReadOffset(pDBR,0x0B,2);
            fatInfo.bySectorsPerCluster = (BYTE)ReadOffset(pDBR,0x0D,1);

            fatInfo.wReserverSectors = (WORD)ReadOffset(pDBR,0x0E,2);

            fatInfo.byNumsOfFAT = (BYTE)ReadOffset(pDBR,0x10,1);
            fatInfo.ullStartSector = ullStartSector;
            fatInfo.dwVolumeLength = (DWORD)ReadOffset(pDBR,0x20,4);

            if (fileSystem == FileSystem_FAT32)
            {
                fatInfo.dwFATLength = (DWORD)ReadOffset(pDBR,0x24,4);
                fatInfo.byFATBit = 32;
            }
            else
            {
                fatInfo.dwFATLength = (DWORD)ReadOffset(pDBR,0x16,2);
                fatInfo.wSectorsOfRoot = (WORD)ReadOffset(pDBR,0x11,2) * 32 / fatInfo.wBytesPerSector;

                if (fileSystem == FileSystem_FAT16)
                {
                    fatInfo.byFATBit = 16;
                }
                else
                {
                    fatInfo.byFATBit = 12;
                }
            }

            // 分析有效簇
            DWORD dwFATLen = fatInfo.dwFATLength * fatInfo.wBytesPerSector;
            ULONGLONG ullFatStartSectors = fatInfo.ullStartSector + fatInfo.wReserverSectors;
            BYTE *pFATByte = new BYTE[dwFATLen];
            ZeroMemory(pFATByte,dwFATLen);
            if (!ReadSectors(m_hDisk,ullFatStartSectors,fatInfo.dwFATLength,fatInfo.wBytesPerSector,pFATByte,&dwErrorCode))
            {
//                m_MasterPort->SetEndTime(CTime::GetCurrentTime());
//                m_MasterPort->SetPortState(PortState_Fail);
//                m_MasterPort->SetResult(FALSE);
//                m_MasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Read FAT Table error,system errorcode=%ld,%s")
//                    ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

                delete []pFATByte;
                pFATByte = NULL;
                return FALSE;
            }

            EFF_DATA effData;
            ZeroMemory(&effData,sizeof(EFF_DATA));
            effData.ullStartSector = fatInfo.ullStartSector;
            effData.ullSectors = fatInfo.wReserverSectors + fatInfo.byNumsOfFAT * fatInfo.dwFATLength + fatInfo.wSectorsOfRoot;
            effData.wBytesPerSector = fatInfo.wBytesPerSector;

            // 把起始扇区数据单独分开，为了最后读写这一部分数据，防止文件系统产生被系统识别而拒绝写磁盘
            m_EffList.push_back(effData);
            effData.ullStartSector = fatInfo.ullStartSector + effData.ullSectors;
            effData.ullSectors = 0;
            effData.wBytesPerSector = fatInfo.wBytesPerSector;


            DWORD dwNumsOfCluster = dwFATLen * 8 / fatInfo.byFATBit;
            for (DWORD dwStartCluser = 2;dwStartCluser < dwNumsOfCluster;dwStartCluser++)
            {
                if (effData.ullStartSector > m_ullSectorNums)
                {
                    break;
                }

                if (IsFATValidCluster(pFATByte,dwStartCluser,fatInfo.byFATBit))
                {
                    effData.ullSectors += fatInfo.bySectorsPerCluster;

                    // 是否超过总扇区数
                    if (effData.ullStartSector + effData.ullSectors > m_ullSectorNums)
                    {
                        effData.ullSectors = m_ullSectorNums - effData.ullStartSector;
                    }

                }
                else
                {
                    if (effData.ullSectors != 0)
                    {
                        m_EffList.push_back(effData);
                    }

                    effData.ullStartSector += (effData.ullSectors + fatInfo.bySectorsPerCluster);
                    effData.ullSectors = 0;
                }
            }

            if (effData.ullSectors != 0)
            {
                m_EffList.push_back(effData);
            }

            delete []pFATByte;
            pFATByte = NULL;

        }
        break;
    case FileSystem_EXFAT:
        {
            EXFAT_INFO exfatInfo;
            ZeroMemory(&exfatInfo,sizeof(EXFAT_INFO));

            exfatInfo.ullPartitionOffset = ReadOffset(pDBR,0x40,8);
            exfatInfo.ullVolumeLength = ReadOffset(pDBR,0x48,8);
            exfatInfo.dwFatOffset = (DWORD)ReadOffset(pDBR,0x50,4);
            exfatInfo.dwFatLength = (DWORD)ReadOffset(pDBR,0x54,4);
            exfatInfo.dwClusterHeapOffset = (DWORD)ReadOffset(pDBR,0x58,4);
            exfatInfo.dwClusterCount = (DWORD)ReadOffset(pDBR,0x5C,4);
            exfatInfo.dwRootDirectoryCluster = (DWORD)ReadOffset(pDBR,0x60,4);
            exfatInfo.wBytesPerSector = (WORD)pow(2,(double)ReadOffset(pDBR,0x6C,1));
            exfatInfo.dwSectorsPerCluster = (DWORD)pow(2,(double)ReadOffset(pDBR,0x6D,1));
            exfatInfo.byNumsOfFat = (BYTE)ReadOffset(pDBR,0x6E,1);

            // 第一步，找到根目录
            // 通过ClusterHeapOffset可以获得2号簇的起始扇区，然后通过RootDirectoryCluster，即可获得根目录的起始扇区
            DWORD dwRootDirectoryStartSector = exfatInfo.dwClusterHeapOffset
                + (exfatInfo.dwRootDirectoryCluster - 2) * exfatInfo.dwSectorsPerCluster;
            BYTE *pRoot = new BYTE[exfatInfo.wBytesPerSector];
            ZeroMemory(pRoot,exfatInfo.wBytesPerSector);

            // 20140-12-10 取Boot Sector中的偏移扇区不保险，换成MBR中记录的起始地址
            //ULONGLONG ullTempStartSector = exfatInfo.ullPartitionOffset + dwRootDirectoryStartSector;
            ULONGLONG ullTempStartSector = ullStartSector + dwRootDirectoryStartSector;
            if (!ReadSectors(m_hDisk,ullTempStartSector,1,exfatInfo.wBytesPerSector,pRoot,&dwErrorCode))
            {
//                m_MasterPort->SetEndTime(CTime::GetCurrentTime());
//                m_MasterPort->SetPortState(PortState_Fail);
//                m_MasterPort->SetResult(FALSE);
//                m_MasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %d,Disk %d - Read EXFAT Root Directory error,system errorcode=%ld,%s")
//                    ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

                delete []pRoot;
                pRoot = NULL;
                return FALSE;
            }

            // 第二步，获得BitMap起始扇区
            DWORD dwDirectoryItems =  exfatInfo.wBytesPerSector / 0x20;
            DWORD dwBitMapStartSectors = 0;
            DWORD dwBitMapLength = 0;
            for (DWORD item = 0,offset = 0;item < dwDirectoryItems;item++,offset += 0x20)
            {
                BYTE byEntryType = (BYTE)ReadOffset(pRoot,offset,1);
                if (byEntryType == 0x81)
                {
                    DWORD dwFirstCluster = (DWORD)ReadOffset(pRoot,offset+0x14,4);
                    dwBitMapLength = (DWORD)ReadOffset(pRoot,offset+0x18,4);
                    dwBitMapStartSectors = (dwFirstCluster - 2) * exfatInfo.dwSectorsPerCluster + exfatInfo.dwClusterHeapOffset;
                    break;
                }
            }

            delete []pRoot;
            pRoot = NULL;

            BYTE *pBitMap = new BYTE[dwBitMapLength];
            ZeroMemory(pBitMap,dwBitMapLength);

            ullTempStartSector = exfatInfo.ullPartitionOffset + dwBitMapStartSectors;
            if (!ReadSectors(m_hDisk,ullTempStartSector,dwBitMapLength/exfatInfo.wBytesPerSector,exfatInfo.wBytesPerSector,pBitMap,&dwErrorCode))
            {
//                m_MasterPort->SetEndTime(CTime::GetCurrentTime());
//                m_MasterPort->SetPortState(PortState_Fail);
//                m_MasterPort->SetResult(FALSE);
//                m_MasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Read EXFAT Bitmap error,system errorcode=%ld,%s")
//                    ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

                delete []pBitMap;
                pBitMap = NULL;
                return FALSE;
            }

            // 第三步，分析哪些是有效簇,注意从2号簇开始
            // 把2号簇之前的数据添加进有效数据列表
            EFF_DATA effData;
            ZeroMemory(&effData,sizeof(EFF_DATA));
            effData.ullStartSector = exfatInfo.ullPartitionOffset;
            effData.ullSectors = exfatInfo.dwClusterHeapOffset;
            effData.wBytesPerSector = exfatInfo.wBytesPerSector;

            m_EffList.push_back(effData);

            effData.ullStartSector += effData.ullSectors;
            effData.ullSectors = 0;

            for (DWORD cluster = 0;cluster < dwBitMapLength * 8;cluster++)
            {
                if (effData.ullStartSector > m_ullSectorNums)
                {
                    break;
                }

                // EXFAT 和NTFS共用一个判断有效簇的方法
                if (IsNTFSValidCluster(pBitMap,cluster))
                {
                    effData.ullSectors += exfatInfo.dwSectorsPerCluster;

                    if (effData.ullStartSector + effData.ullSectors > m_ullSectorNums)
                    {
                        effData.ullSectors = m_ullSectorNums - effData.ullStartSector;
                    }
                }
                else
                {
                    if (effData.ullSectors != 0)
                    {
                        m_EffList.push_back(effData);
                    }

                    effData.ullStartSector += (effData.ullSectors + exfatInfo.dwSectorsPerCluster);
                    effData.ullSectors = 0;

                }
            }


            if (effData.ullSectors != 0)
            {
                m_EffList.push_back(effData);
            }

            delete []pBitMap;
            pBitMap = NULL;

        }
        break;
    case FileSystem_NTFS:
        {
            NTFS_INFO ntfsInfo;
            ZeroMemory(&ntfsInfo,sizeof(NTFS_INFO));

            ntfsInfo.wBytesPerSectors = (WORD)ReadOffset(pDBR,0x0B,2);
            ntfsInfo.bySectorsPerCluster = (BYTE)ReadOffset(pDBR,0x0D,1);
            ntfsInfo.ullVolumeLength = ReadOffset(pDBR,0x28,8);
            ntfsInfo.ullStartCluserOfMFT = ReadOffset(pDBR,0x30,8);
            ntfsInfo.byMFTSize = (BYTE)ReadOffset(pDBR,0x40,1);
            ntfsInfo.ullStartSector = ullStartSector;

            // 分析有效cu
            // Step1.找到6号MFT项，即BITMAP项，每个MFT项占1024个字节
            ULONGLONG ullBitMapMFTStartSector = ntfsInfo.ullStartSector + ntfsInfo.ullStartCluserOfMFT * ntfsInfo.bySectorsPerCluster
                + 1024 / ntfsInfo.wBytesPerSectors * 6;

            // Step2 找BITMAP的DATA属性，即属性类型为0x80
            BYTE *pMFT = new BYTE[1024];
            ZeroMemory(pMFT,1024);
            if (!ReadSectors(m_hDisk,ullBitMapMFTStartSector,1024/ntfsInfo.wBytesPerSectors,ntfsInfo.wBytesPerSectors,pMFT,&dwErrorCode))
            {
//                m_MasterPort->SetEndTime(CTime::GetCurrentTime());
//                m_MasterPort->SetPortState(PortState_Fail);
//                m_MasterPort->SetResult(FALSE);
//                m_MasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Read Bitmmap MFT error,system errorcode=%ld,%s")
//                    ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

                delete []pMFT;
                pMFT = NULL;
                return FALSE;
            }

            ULONGLONG ullBitMapDataStartCluster = 0;
            DWORD dwBitMapDataClusters = 0;
            WORD wFirstAttributeOffset = (WORD)ReadOffset(pMFT,0x14,2);
            DWORD dwAttributeType = 0;;
            DWORD dwAttributeLength = 0;
            BYTE  byNonResident = 0;
            DWORD dwAttributeOffset = wFirstAttributeOffset;
            for (int i = 0; i < 128;i++)
            {
                dwAttributeType = (DWORD)ReadOffset(pMFT,dwAttributeOffset,4);
                dwAttributeLength = (DWORD)ReadOffset(pMFT,dwAttributeOffset + 0x04,4);
                byNonResident = (BYTE)ReadOffset(pMFT,dwAttributeOffset + 0x08,1);
                if (dwAttributeType == 0x80 && byNonResident == 1)
                {
                    WORD wDataRunOffset = (WORD)ReadOffset(pMFT,dwAttributeOffset + 0x20,2);

                    // 第一部分：第一个字节，高四位表示簇流起始位置的字节数（起始簇号），低四位表示簇流长度的字节数（簇数）
                    // 第二部分：簇流包含的簇数
                    // 第三部分：簇流起始簇号
                    BYTE byPartOne = (BYTE)ReadOffset(pMFT,dwAttributeOffset + wDataRunOffset,1);
                    BYTE low=0,high=0;
                    low = byPartOne & 0x0F;
                    high = (byPartOne >> 4) & 0x0F;

                    DWORD dwPartTwo = (DWORD)ReadOffset(pMFT,dwAttributeOffset + wDataRunOffset + 1,low);
                    ULONGLONG ullPartThree = ReadOffset(pMFT,dwAttributeOffset + wDataRunOffset + 1 + low,high);

                    dwBitMapDataClusters= dwPartTwo;
                    ullBitMapDataStartCluster = ullPartThree;

                    break;
                }

                dwAttributeOffset += dwAttributeLength;
            }
            delete []pMFT;
            pMFT = NULL;

            // Step3 找到Bitmap位置，确定哪些是有效簇
            ULONGLONG ullBitMapDataStartSector = ntfsInfo.ullStartSector + ullBitMapDataStartCluster * ntfsInfo.bySectorsPerCluster;
            DWORD dwLengthPerCluster = ntfsInfo.bySectorsPerCluster * ntfsInfo.wBytesPerSectors;

            EFF_DATA effData;
            ZeroMemory(&effData,sizeof(EFF_DATA));
            effData.ullStartSector = ntfsInfo.ullStartSector;
            effData.ullSectors = ntfsInfo.bySectorsPerCluster;
            effData.wBytesPerSector = ntfsInfo.wBytesPerSectors;

            // 0号簇单独添加
            m_EffList.push_back(effData);

            effData.ullStartSector = ntfsInfo.ullStartSector + ntfsInfo.bySectorsPerCluster;
            effData.ullSectors = 0;

            BYTE *pBitmap = new BYTE[dwLengthPerCluster];

            ULONGLONG ullClusterIndex = 0;
            for (DWORD i = 0;i < dwBitMapDataClusters;i++)
            {
                ZeroMemory(pBitmap,dwLengthPerCluster);
                ULONGLONG ullTempStart = ullBitMapDataStartSector + i * ntfsInfo.bySectorsPerCluster;
                if (!ReadSectors(m_hDisk,ullTempStart,ntfsInfo.bySectorsPerCluster,ntfsInfo.wBytesPerSectors,pBitmap,&dwErrorCode))
                {
//                    m_MasterPort->SetEndTime(CTime::GetCurrentTime());
//                    m_MasterPort->SetPortState(PortState_Fail);
//                    m_MasterPort->SetResult(FALSE);
//                    m_MasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//                    CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Read Bitmap data error,system errorcode=%ld,%s")
//                        ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

                    delete []pBitmap;
                    pBitmap = NULL;
                    return FALSE;
                }

                for (DWORD cluster = 0;cluster < dwLengthPerCluster * 8;cluster++)
                {
                    // 0号簇已经添加
                    if (ullClusterIndex == 0)
                    {
                        ullClusterIndex++;
                        continue;
                    }

                    if (effData.ullStartSector > m_ullSectorNums)
                    {
                        break;
                    }

                    if (IsNTFSValidCluster(pBitmap,cluster))
                    {
                        effData.ullSectors += ntfsInfo.bySectorsPerCluster;
                        if (effData.ullStartSector + effData.ullSectors > m_ullSectorNums)
                        {
                            effData.ullSectors = m_ullSectorNums - effData.ullStartSector;
                        }
                    }
                    else
                    {
                        if (effData.ullSectors != 0)
                        {
                            m_EffList.push_back(effData);
                        }

                        effData.ullStartSector += (effData.ullSectors + ntfsInfo.bySectorsPerCluster);
                        effData.ullSectors = 0;
                    }

                    ullClusterIndex++;
                }

            }

            if (effData.ullSectors != 0)
            {
                m_EffList.push_back(effData);
            }

            delete []pBitmap;
            pBitmap = NULL;

        }
        break;
    case FileSystem_EXT_X:
        {
            ULONGLONG ullTempStartSector = ullStartSector + 1024/m_dwBytesPerSector;
            BYTE *pSuperBlock = new BYTE[m_dwBytesPerSector];
            ZeroMemory(pSuperBlock,m_dwBytesPerSector);

            if (!ReadSectors(m_hDisk,ullTempStartSector,1,m_dwBytesPerSector,pSuperBlock,&dwErrorCode))
            {
//                m_MasterPort->SetEndTime(CTime::GetCurrentTime());
//                m_MasterPort->SetPortState(PortState_Fail);
//                m_MasterPort->SetResult(FALSE);
//                m_MasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Read Super Block error,system errorcode=%ld,%s")
//                    ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));


                delete []pSuperBlock;
                pSuperBlock = NULL;

                return FALSE;
            }

            SUPER_BLOCK_INFO superBlockInfo;
            ZeroMemory(&superBlockInfo,sizeof(SUPER_BLOCK_INFO));
            superBlockInfo.dwBlocksOfVolume = (DWORD)ReadOffset(pSuperBlock,0x04,4);
            superBlockInfo.dwStartBlockOfGroup0 = (DWORD)ReadOffset(pSuperBlock,0x14,4);
            superBlockInfo.dwBytesPerBlock = (1024 << (DWORD)ReadOffset(pSuperBlock,0x18,4));
            superBlockInfo.dwBlocksPerBlockGroup = (DWORD)ReadOffset(pSuperBlock,0x20,4);
            superBlockInfo.dwNodesPerBlockGroup = (DWORD)ReadOffset(pSuperBlock,0x28,4);
            superBlockInfo.dwSectorsPerBlock = superBlockInfo.dwBytesPerBlock / m_dwBytesPerSector;

            delete []pSuperBlock;
            pSuperBlock = NULL;

            // 添加超级块之前的块到有效数据列表
            EFF_DATA effData;
            ZeroMemory(&effData,sizeof(EFF_DATA));
            effData.ullStartSector = ullStartSector;
            effData.ullSectors = superBlockInfo.dwStartBlockOfGroup0 * superBlockInfo.dwSectorsPerBlock;
            effData.wBytesPerSector = (WORD)m_dwBytesPerSector;

            if (effData.ullSectors != 0)
            {
                m_EffList.push_back(effData);
            }

            // 组块的总数
            DWORD dwTotalGroupBlocks = superBlockInfo.dwBlocksOfVolume / superBlockInfo.dwBlocksPerBlockGroup;
            if (superBlockInfo.dwBlocksOfVolume % superBlockInfo.dwBlocksPerBlockGroup)
            {
                dwTotalGroupBlocks++;
            }

            // 定位组描述表的扇区

            // 超级块的块号
            DWORD dwSuperBlockNum = 1;
            if (superBlockInfo.dwBytesPerBlock / 1024 > 1)
            {
                dwSuperBlockNum = 0;
            }

            // 组描述表的块号，组描述表起始于超级块所在块的下一块
            DWORD dwGroupDescriptionBlockNum = dwSuperBlockNum + 1;
            DWORD dwGroupDescriptionStartSector = dwGroupDescriptionBlockNum * superBlockInfo.dwSectorsPerBlock;

            // 组描述符表占N的块，而非一个块，根据块组大小决定 2014-06-27 Binggoo
            DWORD dwGroupDescriptionBlocks = dwTotalGroupBlocks * 0x20 / superBlockInfo.dwBytesPerBlock;
            if (dwTotalGroupBlocks * 0x20 % superBlockInfo.dwBytesPerBlock != 0)
            {
                dwGroupDescriptionBlocks++;
            }

            DWORD dwGroupDescriptionLen = dwGroupDescriptionBlocks * superBlockInfo.dwBytesPerBlock;
            BYTE *pGroupDescrption = new BYTE[dwGroupDescriptionLen];
            ZeroMemory(pGroupDescrption,dwGroupDescriptionLen);

            ullTempStartSector = ullStartSector + dwGroupDescriptionStartSector;
            if (!ReadSectors(m_hDisk,ullTempStartSector,dwGroupDescriptionBlocks * superBlockInfo.dwSectorsPerBlock,m_dwBytesPerSector,pGroupDescrption,&dwErrorCode))
            {
//                m_MasterPort->SetEndTime(CTime::GetCurrentTime());
//                m_MasterPort->SetPortState(PortState_Fail);
//                m_MasterPort->SetResult(FALSE);
//                m_MasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Read Group Block Description error,system errorcode=%ld,%s")
//                    ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

                delete []pGroupDescrption;
                pGroupDescrption = NULL;

                return FALSE;
            }

            DWORD dwBlocksIndex = superBlockInfo.dwStartBlockOfGroup0;

            for (DWORD group=0,offset=0;group < dwTotalGroupBlocks;group++,offset += 0x20)
            {
                // 块位图的起始位置和大小
                DWORD dwBlockBitMapStartBlock = (DWORD)ReadOffset(pGroupDescrption,offset,4);
                DWORD dwBlockBitMapStartSector = dwBlockBitMapStartBlock * superBlockInfo.dwSectorsPerBlock;
                DWORD dwBlockBitMapLength = superBlockInfo.dwBlocksPerBlockGroup / 8;

                //块位图的大小必须是块大小的整数倍
                dwBlockBitMapLength = (dwBlockBitMapLength + superBlockInfo.dwBytesPerBlock - 1) / superBlockInfo.dwBytesPerBlock
                    * superBlockInfo.dwBytesPerBlock;

                BYTE *pBitMap = new BYTE[dwBlockBitMapLength];
                ZeroMemory(pBitMap,dwBlockBitMapLength);

                ullTempStartSector = ullStartSector + dwBlockBitMapStartSector;
                if (!ReadSectors(m_hDisk,ullTempStartSector,dwBlockBitMapLength/m_dwBytesPerSector,m_dwBytesPerSector,pBitMap,&dwErrorCode))
                {
//                    m_MasterPort->SetEndTime(CTime::GetCurrentTime());
//                    m_MasterPort->SetPortState(PortState_Fail);
//                    m_MasterPort->SetResult(FALSE);
//                    m_MasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//                    CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Read Block Bitmap error,system errorcode=%ld,%s")
//                        ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

                    delete []pBitMap;
                    pBitMap = NULL;

                    delete []pGroupDescrption;
                    pGroupDescrption = NULL;

                    return FALSE;

                }

                // 块位图只描述本块组内的块的分配情况
                effData.ullStartSector = ullStartSector
                    + superBlockInfo.dwStartBlockOfGroup0 * superBlockInfo.dwSectorsPerBlock
                    + group * superBlockInfo.dwBlocksPerBlockGroup * superBlockInfo.dwSectorsPerBlock;
                effData.ullSectors = 0;

                // 判断有效块
                for (DWORD block = 0;block < superBlockInfo.dwBlocksPerBlockGroup;block++)
                {
                    // 最后一个块组的块数有可能小于实际每个组块的块数
                    if (dwBlocksIndex > superBlockInfo.dwBlocksOfVolume - 1)
                    {
                        break;
                    }

                    if (effData.ullStartSector > m_ullSectorNums)
                    {
                        break;
                    }

                    if (IsExtXValidBlock(pBitMap,block))
                    {
                        effData.ullSectors += superBlockInfo.dwSectorsPerBlock;

                        if (effData.ullStartSector + effData.ullSectors > m_ullSectorNums)
                        {
                            effData.ullSectors = m_ullSectorNums - effData.ullStartSector;
                        }
                    }
                    else
                    {
                        if (effData.ullSectors != 0)
                        {
                            m_EffList.push_back(effData);
                        }

                        effData.ullStartSector += (effData.ullSectors + superBlockInfo.dwSectorsPerBlock);
                        effData.ullSectors = 0;
                    }

                    dwBlocksIndex++;
                }

                delete []pBitMap;
                pBitMap = NULL;

                if (effData.ullSectors != 0)
                {
                    m_EffList.push_back(effData);
                }

            }

            delete []pGroupDescrption;
            pGroupDescrption = NULL;

        }
        break;

    case FileSystem_EXTEND:
        {
            MASTER_BOOT_RECORD ebr;
            ZeroMemory(&ebr,sizeof(MASTER_BOOT_RECORD));
            memcpy(&ebr,pDBR,sizeof(MASTER_BOOT_RECORD));

            CString strPartions,strByte;
            DWORD dwMinSector = -1;
            for (int row = 0;row < 4;row++)
            {
                strPartions.Empty();
                for (int col =0;col < 16;col++)
                {
                    int offset = 0x1BE + row * 16 + col;
                    strByte.Format(_T("%02X "),pDBR[offset]);
                    strPartions += strByte;
                }
//                CUtils::WriteLogFile(m_hLogFile,FALSE,strPartions);

                if (ebr.Partition[row].StartLBA != 0 && bMBR)
                {
                    if (ebr.Partition[row].StartLBA < dwMinSector)
                    {
                        dwMinSector = ebr.Partition[row].StartLBA;
                    }
                }
            }

            // 把MBR到第一个分区之前的数据添加进列表
            EFF_DATA headData;
            headData.ullStartSector = ullStartSector;
            headData.ullSectors = 63;
            headData.wBytesPerSector = (WORD)m_dwBytesPerSector;

            if (bMBR)
            {
                headData.ullSectors = dwMinSector;
            }

            m_EffList.push_back(headData);

            for (int i = 0; i < 4;i++)
            {
                if (ebr.Partition[i].StartLBA == 0)
                {
                    continue;
                }
                BYTE *pDBRTemp = new BYTE[m_dwBytesPerSector];
                ZeroMemory(pDBRTemp,m_dwBytesPerSector);

                ULONGLONG ullTempStartSector = ebr.Partition[i].StartLBA;

                if (ebr.Partition[i].PartitionType == 0x0F) // 主扩展分区
                {
                    ullMasterSectorOffset = ullTempStartSector;
                }
                else if (ebr.Partition[i].PartitionType == 0x05) // 二级扩展分区
                {
                    ullTempStartSector += ullMasterSectorOffset;
                }
                else
                {
                    ullTempStartSector += ullStartSector;
                }

                if (!ReadSectors(m_hDisk,ullTempStartSector,1,m_dwBytesPerSector,pDBRTemp,&dwErrorCode))
                {
//                    m_MasterPort->SetEndTime(CTime::GetCurrentTime());
//                    m_MasterPort->SetPortState(PortState_Fail);
//                    m_MasterPort->SetResult(FALSE);
//                    m_MasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//                    CString strErrMsg;
//                    strErrMsg.Format(_T("Partition=%d,FileSystem=%02X,StartLBA=0x%X,Sectors=0x%X - Read DBR Error"),
//                        i+1,ebr.Partition[i].PartitionType,ebr.Partition[i].StartLBA,ebr.Partition[i].TotalSector);

//                    CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - %s,system errorcode=%ld,%s")
//                        ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum(),strErrMsg,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

                    delete []pDBRTemp;
                    pDBRTemp = NULL;

                    return FALSE;
                }

                if (ebr.Partition[i].PartitionType == 0x05 || ebr.Partition[i].PartitionType == 0x0F) // 扩展分区
                {
                    fileSystem = FileSystem_EXTEND;
                }
                else
                {
                    fileSystem = GetFileSystem(pDBRTemp,ullTempStartSector);
                }

                if (!AppendEffDataList(pDBRTemp,fileSystem,ullTempStartSector,ullMasterSectorOffset,
                    ebr.Partition[i].TotalSector,FALSE))
                {
                    delete []pDBRTemp;
                    pDBRTemp = NULL;

                    return FALSE;
                }

                delete []pDBRTemp;
                pDBRTemp = NULL;
            }
        }
        break;

    case FileSystem_UNKNOWN:
    default:
        {
            // 不识别的文件系统，拷贝所有资料
            EFF_DATA effData;
            ZeroMemory(&effData,sizeof(EFF_DATA));
            effData.ullStartSector = ullStartSector;
            effData.ullSectors = ullSectors;
            effData.wBytesPerSector = (WORD)m_dwBytesPerSector;

            m_EffList.push_back(effData);
        }
        break;
    }

    return TRUE;
}


BOOL MakeImageDisk::IsExtXValidBlock( const PBYTE pByte,DWORD block )
{
    DWORD dwByteIndex = block/8;
    BYTE  bitIndex = block % 8;
    BYTE byValue = (BYTE)ReadOffset(pByte,dwByteIndex,1);

    return ((byValue >> bitIndex) & 0x1);
}



BOOL MakeImageDisk::IsFATValidCluster( const PBYTE pByte,DWORD cluster,BYTE byFATBit )
{
    // byFAT = 12/16/32;
    // cluster begin 2
    // 坏簇标志 FAT12 - 0xFF7, FAT16 - 0xFFF7, FAT32 - 0x0FFFFFF7
    if (byFATBit == 32)
    {
        DWORD dwValue = (DWORD)ReadOffset(pByte,cluster * 4,4);

        return (dwValue != 0 && dwValue != 0x0FFFFFF7);
    }
    else if (byFATBit == 16)
    {
        WORD wValue = (WORD)ReadOffset(pByte,cluster * 2,2);

        return (wValue != 0 && wValue != 0xFFF7);
    }
    else
    {
        DWORD offset = cluster * 3 / 2;
        WORD wValue = 0;
        if (cluster * 3 % 2 == 0)
        {
            wValue = (WORD)ReadOffset(pByte,offset,2);
        }
        else
        {
            wValue = (WORD)ReadOffset(pByte,offset,2);
        }

        return (wValue != 0 && wValue != 0xFF7);
    }
}



BOOL MakeImageDisk::IsNTFSValidCluster( const PBYTE pByte,DWORD cluster )
{
    DWORD dwByteIndex = cluster/8;
    BYTE  bitIndex = cluster % 8;
    BYTE byValue = (BYTE)ReadOffset(pByte,dwByteIndex,1);

    return ((byValue >> bitIndex) & 0x1);
}
