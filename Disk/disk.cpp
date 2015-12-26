#include "disk.h"
#include <string.h>
#include <zlib.h>
#include <assert.h>

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/hdreg.h>
#include <linux/fs.h>

#include <sys/socket.h>
#include <pthread.h>
//#include <wait.h>

#include "common/PrintMacros.h"

Disk::Disk()
{

}

HANDLE Disk::GetHandleOnPhysicalDrive(CString strDiskPath, DWORD dwFlagsAndAttributes, PDWORD pdwErrorCode)
{
//    HANDLE hDisk = open(strDiskPath.c_str(),dwFlagsAndAttributes);
    HANDLE hDisk ;
    if(dwFlagsAndAttributes & O_CREAT)
    {
        mode_t mode = S_IRUSR|S_IWUSR|S_IROTH|S_IWOTH;
        hDisk = open(strDiskPath.c_str(),dwFlagsAndAttributes,mode);
    }
    else
    {
        hDisk = open(strDiskPath.c_str(),dwFlagsAndAttributes);
    }
    if(hDisk<0)
    {
        *pdwErrorCode=errno;
    }
    return hDisk;
}

//HANDLE Disk::GetHandleOnPhysicalDrive(int iDiskNumber, DWORD dwFlagsAndAttributes, PDWORD pdwErrorCode)
//{

//}

HANDLE Disk::GetHandleOnVolume(CString letter, DWORD dwFlagsAndAttributes, PDWORD pdwErrorCode)
{
//    HANDLE hDisk = open(letter.c_str(),dwFlagsAndAttributes);
    HANDLE hDisk ;
    if(dwFlagsAndAttributes & O_CREAT)
    {
        mode_t mode = S_IRUSR|S_IWUSR|S_IROTH|S_IWOTH;
        hDisk = open(letter.c_str(),dwFlagsAndAttributes,mode);
    }
    else
    {
        hDisk = open(letter.c_str(),dwFlagsAndAttributes);
    }

    if(hDisk)
    {
        *pdwErrorCode=errno;
    }
    return hDisk;
}

HANDLE Disk::GetHandleOnFile(CString strFileName,  DWORD dwFlagsAndAttributes, PDWORD pdwErrorCode)
{
    HANDLE hFile ;
    if(dwFlagsAndAttributes & O_CREAT)
    {
        mode_t mode = S_IRUSR|S_IWUSR|S_IROTH|S_IWOTH|S_IRGRP|S_IWGRP;
        hFile = open(strFileName.c_str(),dwFlagsAndAttributes,mode);
    }
    else
    {
        hFile = open(strFileName.c_str(),dwFlagsAndAttributes);
    }

    if(hFile)
    {
        *pdwErrorCode=errno;
    }
    return hFile;
}

ULONGLONG Disk::GetNumberOfSectors(HANDLE hDevice, PDWORD pdwBytesPerSector, MEDIA_TYPE *type)
{
    type = type;

    if(hDevice<0)
    {
        std::cout<<__FILE__<<__LINE__<<"file handle invalid!"<<endl;
        return (ULONGLONG)-1;
    }
//    off_t size = lseek(hDevice,0,SEEK_END);
    int   sector_size = 0;

    if(ioctl(hDevice,BLKSSZGET,&sector_size)<0)
    {
        std::cout<<__FILE__<<__LINE__<<"ioctl failed !"<<endl;
        return (ULONGLONG)-1;
    }
    *pdwBytesPerSector = sector_size;

    long sector_count = 0;
    if(ioctl(hDevice,BLKGETSIZE,&sector_count)<0)
    {
        cout<<"get sector count failed "<<endl;
    }

    return sector_count;
}

BOOL Disk::SetDiskAtrribute(HANDLE hDisk, BOOL bReadOnly, BOOL bOffline, PDWORD pdwErrorCode)
{
    USE_PARAM(hDisk);
    USE_PARAM(bReadOnly);
    USE_PARAM(bOffline);
    USE_PARAM(pdwErrorCode);

    return FALSE;
}


//BOOL Disk::GetDiskModelNameAndSerialNumber(HANDLE hDevice, LPTSTR lpszModulName, LPTSTR lpszSerialNum, DWORD *pdwErrorCode)
//{

//}

void Disk::Init(HWND hWnd, LPBOOL lpCancel, HANDLE hLogFile, PortCommand *pCommand, UINT nBlockSectors)
{
    m_hWnd = hWnd;
    m_lpCancel = lpCancel;
    m_hLogFile = hLogFile;
    m_pCommand = pCommand;
    m_nBlockSectors = nBlockSectors;
}

#include <sys/stat.h>
BOOL Disk::GetFileSize(int fd, ULONGLONG &ullSize)
{
    struct stat statbuff;
    if(fstat(fd, &statbuff) < 0){

        return FALSE;
    }else{
        ullSize = statbuff.st_size;
    }
    return TRUE;
}


void Disk::SetMasterPort(CPort *port)
{
    m_pMasterPort = port;
    m_pMasterPort->Reset();

    DWORD dwErrorCode = 0;
    switch (m_pMasterPort->GetPortType())
    {
    case PortType_MASTER_DISK:
        {
            m_hMaster = GetHandleOnPhysicalDrive(m_pMasterPort->GetDevicePath() ,
                O_RDWR,&dwErrorCode);
            if (m_hMaster < 0)
            {
                m_pMasterPort->SetResult(FALSE);
                m_pMasterPort->SetErrorCode(ErrorType_System,dwErrorCode);
                m_pMasterPort->SetPortState(PortState_Fail);
//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - OpenDisk error,system errorcode=%ld,%s")
//                    ,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
            }
            else
            {
                m_pMasterPort->SetPortState(PortState_Active);
                m_ullSectorNums = GetNumberOfSectors(m_hMaster,&m_dwBytesPerSector,NULL);
                m_ullCapacity = m_ullSectorNums * m_dwBytesPerSector;
            }
        }

        break;

    case PortType_MASTER_FILE:
        {
            m_hMaster = GetHandleOnFile(m_pMasterPort->GetFileName(),
                O_RDWR,&dwErrorCode);

            if (m_hMaster < 0)
            {
                m_pMasterPort->SetResult(FALSE);
                m_pMasterPort->SetErrorCode(ErrorType_System,dwErrorCode);
                m_pMasterPort->SetPortState(PortState_Fail);
//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Open image file error,filename=%s,system errorcode=%ld,%s")
//                    ,m_pMasterPort->GetFileName(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
            }
            else
            {
                m_pMasterPort->SetPortState(PortState_Active);
                ULONGLONG ullSize = 0;
                if (GetFileSize(m_hMaster,ullSize))
                {
                    m_ullImageSize = ullSize;
                }

            }
        }


        break;

    case PortType_SERVER:
        {
            // 本地创建IMAGE
            CString strTempName,strImageFile;
            strImageFile = m_pMasterPort->GetFileName();

//            strTempName.Format(_T("%s.$$$"),strImageFile.Left(strImageFile.GetLength() - 4));

            m_hMaster = GetHandleOnFile(strTempName,
                O_RDWR|O_CREAT|O_TRUNC,&dwErrorCode);

            if (m_hMaster < 0)
            {
//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Open image file error,filename=%s,system errorcode=%ld,%s")
//                    ,strTempName,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
            }
        }


        break;

    default:

        break;
    }
}

void Disk::SetSingleTargetPort(CPort *port)
{
    m_pSinglePort = port;
}

void Disk::SetTargetPorts(PortList *pList)
{
    m_TargetPorts = pList;
    PortList::const_iterator it = m_TargetPorts->begin();
    PortList::const_iterator it_end = m_TargetPorts->end();

    while (it != it_end)
    {
        CPort *pPort = (*it);
        it++;
        if (pPort->IsConnected())
        {
            pPort->Reset();
        }
    }
}

void Disk::SetHashMethod(BOOL bComputeHash, HashMethod hashMethod)
{
    m_bComputeHash = bComputeHash;
    m_HashMethod = hashMethod;

    if (m_pMasterHashMethod != NULL)
    {
        delete m_pMasterHashMethod;
        m_pMasterHashMethod = NULL;
    }

    if (bComputeHash)
    {
        switch (hashMethod)
        {
        case HashMethod_CHECKSUM32:
            m_pMasterHashMethod = new CChecksum32();
            break;

        case HashMethod_CRC32:
            m_pMasterHashMethod = new CCRC32();
            break;

        case HashMethod_MD5:
            m_pMasterHashMethod = new MD5();
            break;
        }
    }
}


void Disk::SetWorkMode(WorkMode workMode)
{
    m_WorkMode = workMode;
}

void Disk::SetGlobalParm(BOOL bMustSameCapacity)
{
    m_bMustSameCapacity = bMustSameCapacity;
}

void Disk::SetCleanMode(CleanMode cleanMode, int nFillValue, BOOL bCompareClean, CompareCleanSeq seq)
{
    m_CleanMode = cleanMode;
    m_nFillValue = nFillValue;
    m_bCompareClean = bCompareClean;
    m_CompareCleanSeq = seq;
}

void Disk::SetCompareMode(CompareMode compareMode)
{
    m_CompareMode = compareMode;
}

void Disk::SetFileAndFolder(BOOL bCleanupTarget, const CStringArray &fileArray, const CStringArray &folderArray)
{
    m_bCleanupTargets = bCleanupTarget;
    CStringArray::const_iterator itFileArr= fileArray.begin();
    CStringArray::const_iterator itFileArr_end = fileArray.end();

    CStringArray::const_iterator itFolderArr= folderArray.begin();
    CStringArray::const_iterator itFolderArr_end = folderArray.end();

    m_FileArray.clear();
    m_FolderArray.clear();
    while(itFileArr != itFileArr_end)
    {
        m_FileArray.push_back((*itFileArr));
        itFileArr ++ ;
    }

    while(itFolderArr != itFolderArr_end)
    {
        m_FolderArray.push_back((*itFolderArr));
        itFolderArr ++ ;
    }

}

void Disk::SetFormatParm(CString strVolumeLabel, FileSystem fileSystem, DWORD dwClusterSize, BOOL bQuickFormat)
{
    m_strVolumnLabel = strVolumeLabel;
    m_FileSystem = fileSystem;
    m_dwClusterSize = dwClusterSize;
    m_bQuickFormat = bQuickFormat;

    if (m_dwClusterSize == 0)
    {
        m_dwClusterSize = 8 * 512;
    }
}

void Disk::SetMakeImageParm(int nImageType, BOOL bCompress, int compressLevel)
{
    m_iCompressLevel = compressLevel;
    m_nImageType = nImageType;
    m_bDataCompress = bCompress;

}

void Disk::SetFullCopyParm(BOOL bAllowCapGap, UINT nPercent)
{
    m_bAllowCapGap = bAllowCapGap;
    m_nCapGapPercent = nPercent;
}

void Disk::SetQuickCopyParm(RANGE_FROM_TO *ranges, int count)
{
    m_ListRangeFromTo.clear();

    for (int i = 0; i < count;i++)
    {
        m_ListRangeFromTo.push_back(ranges[i]);
    }
}

void Disk::SetCleanDiskFirst(BOOL bCleanDiskFist, BOOL bCompareClean, CompareCleanSeq seq, int times, int *values)
{
    m_bCleanDiskFirst = bCleanDiskFist;
    m_bCompareClean = bCompareClean;
    m_CompareCleanSeq = seq;

    if (m_bCleanDiskFirst)
    {
        m_nCleanTimes = times;

        m_pCleanValues = new int[times];

        for (int i = 0; i < times;i++)
        {
            m_pCleanValues[i] = values[i];
        }
    }
}

void Disk::SetCompareParm(BOOL bCompare, CompareMethod method)
{
    m_bCompare = bCompare;
    m_CompareMethod = method;
}

void Disk::SetFullRWTestParm(BOOL bReadOnlyTest, BOOL bRetainOriginData, BOOL bFormatFinished, BOOL bStopBadBlock)
{
    m_bReadOnlyTest = bReadOnlyTest;
    m_bRetainOrginData = bRetainOriginData;
    m_bFormatFinished = bFormatFinished;
    m_bStopBadBlock = bStopBadBlock;
}

void Disk::SetFadePickerParm(BOOL bRetainOriginData, BOOL bFormatFinished)
{
    m_bRetainOrginData = bRetainOriginData;
    m_bFormatFinished = bFormatFinished;
}

void Disk::SetSpeedCheckParm(BOOL bCheckSpeed, double dbReaddSpeed, double dbWriteSpeed)
{
    m_bCheckSpeed = bCheckSpeed;
    m_dbMinReadSpeed = dbReaddSpeed;
    m_dbMinWriteSpeed = dbWriteSpeed;
}

BOOL Disk::Start()
{
    // 初始化
    m_ullValidSize = 0;
    m_EffList.clear();
    ZeroMemory(m_ImageHash,LEN_DIGEST);
    m_CompressQueue.Clear();
    m_MapCopyFiles.clear();


    BOOL bRet = FALSE;
    switch (m_WorkMode)
    {
    case WorkMode_FullCopy:
    case WorkMode_QuickCopy:
        bRet = OnCopyDisk();
        break;

    case WorkMode_ImageCopy:
        bRet = OnCopyImage();
        break;

    case WorkMode_ImageMake:
        bRet = OnMakeImage();
        break;

    case WorkMode_DiskCompare:
        bRet = OnCompareDisk();
        break;

    case WorkMode_DiskClean:
        bRet = OnCleanDisk();
        break;

    case WorkMode_FileCopy:
        bRet = OnCopyFiles();
        break;

    case WorkMode_DiskFormat:
        bRet = OnFormatDisk();
        break;

    case WorkMode_DifferenceCopy:
        bRet = OnDiffCopy();
        break;

    case WorkMode_Full_RW_Test:
        bRet = OnFullRWTest();
        break;

    case WorkMode_Fade_Picker:
        bRet = OnFadePickerTest();
        break;

    case WorkMode_Speed_Check:
        bRet = OnSpeedCheck();
        break;
    default:
        bRet = FALSE;
        break;
    }

    return bRet;
}

void Disk::SetSocket(SOCKET sClient, BOOL bServerFirst)
{
    m_ClientSocket = sClient;
    m_bServerFirst = bServerFirst;
}

void Disk::SetDiffCopyParm(UINT nSourceType, BOOL bServer, UINT nCompareRule, BOOL bUpload, const CStringArray &logArray, BOOL bIncludeSunDir)
{
    m_nSourceType = nSourceType;
    m_bServerFirst = bServer;
    m_nCompareRule = nCompareRule;
    m_bIncludeSubDir = bIncludeSunDir;
    m_bUploadUserLog = bUpload;
//    m_ArrayLogPath.Copy(logArray);
    CStringArray::const_iterator it = logArray.begin();
    CStringArray::const_iterator it_end = logArray.end();
    while(it != it_end)
    {
        m_ArrayLogPath.push_back((*it));
        it++;
    }
}

//BOOL Disk::ReadSectors(HANDLE hDevice, ULONGLONG ullStartSector, DWORD dwSectors, DWORD dwBytesPerSector, LPBYTE lpSectBuff, LPOVERLAPPED lpOverlap, DWORD *pdwErrorCode, DWORD dwTimeOut)
//{

//}

//BOOL Disk::WriteSectors(HANDLE hDevice, ULONGLONG ullStartSector, DWORD dwSectors, DWORD dwBytesPerSector, LPBYTE lpSectBuff, LPOVERLAPPED lpOverlap, DWORD *pdwErrorCode, DWORD dwTimeOut)
//{
//    ULONGLONG ullOffset = ullStartSector * dwBytesPerSector;
//    DWORD dwLen = dwSectors * dwBytesPerSector;
//    DWORD dwWriteLen = 0;
//    DWORD dwErrorCode = 0;

//    if (lpOverlap)
//    {
//        lpOverlap->Offset = LODWORD(ullOffset);
//        lpOverlap->OffsetHigh = HIDWORD(ullOffset);
//    }


//    if (!WriteFile(hDevice,lpSectBuff,dwLen,dwWriteLen,lpOverlap))
//    {
//        dwErrorCode = ::GetLastError();

//        return FALSE;
//    }
//    else
//    {
//        return TRUE;
//    }
//}

//BOOL Disk::WriteFile(HANDLE hDevice, LPBYTE lpSectBuff, DWORD dwLen, DWORD &dwWriteLen, LPOVERLAPPED lpOverlap)
//{
//    ULONGLONG offset;
//    offset=((lpOverlap->OffsetHigh)<<32)|(lpOverlap->Offset);
//    int n;
//    if((n = pwrite(hDevice,lpSectBuff,dwLen,offset))>0)
//    {
//        dwWriteLen = n;
//        return TRUE;
//    }
//    else
//    {
//       if(n<0)
//       {
//            return FALSE;
//       }
//       else
//       {
//            dwWriteLen = n;
//            return TRUE;
//       }
//    }
//}

//BOOL Disk::ReadFileAsyn(HANDLE hFile, ULONGLONG ullOffset, DWORD &dwSize, LPBYTE lpBuffer, LPOVERLAPPED lpOverlap, PDWORD pdwErrorCode, DWORD dwTimeOut)
//{

//}

//BOOL Disk::WriteFileAsyn(HANDLE hFile, ULONGLONG ullOffset, DWORD &dwSize, LPBYTE lpBuffer, LPOVERLAPPED lpOverlap, PDWORD pdwErrorCode, DWORD dwTimeOut)
//{

//}

BOOL Disk::ReadSectors(HANDLE hDevice, ULONGLONG ullStartSector, DWORD dwSectors, DWORD dwBytesPerSector, LPBYTE lpSectBuff, DWORD *pdwErrorCode, DWORD dwTimeOut)
{
    ULONGLONG ullOffset = ullStartSector * dwBytesPerSector;
    DWORD dwLen = dwSectors * dwBytesPerSector;
    DWORD dwReadLen = 0;
//    DWORD dwErrorCode = 0;
    dwTimeOut = dwTimeOut;
    if (!ReadFile(hDevice,lpSectBuff,dwLen,dwReadLen,ullOffset))
    {
        if(pdwErrorCode != NULL)
            *pdwErrorCode = errno;
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
//    DWORD dwErrorCode = 0;
    dwTimeOut = dwTimeOut;
    if (!WriteFile(hDevice,lpSectBuff,dwLen,dwWriteLen,ullOffset))
    {
        if(pdwErrorCode != NULL)
            *pdwErrorCode = errno;
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
    USE_PARAM(dwTimeOut);

    DWORD dwReadSize = 0;
    if(!ReadFile(hFile,lpBuffer,dwSize,dwReadSize,ullOffset))
    {
        return FALSE;
    }
    else
    {
        if(dwReadSize != dwSize)
        {
            dwSize = dwReadSize;
            if(pdwErrorCode != NULL)
            {
                *pdwErrorCode = errno;
            }
            return FALSE;
        }
        else
        {
            dwSize = dwReadSize;
            return TRUE;
        }
    }
}

BOOL Disk::WriteFileAsyn(HANDLE hFile, ULONGLONG ullOffset, DWORD &dwSize, LPBYTE lpBuffer, PDWORD pdwErrorCode, DWORD dwTimeOut)
{
    USE_PARAM(dwTimeOut);

    DWORD dwWriteSize = 0;
    if(!WriteFile(hFile,lpBuffer,dwSize,dwWriteSize,ullOffset))
    {
        return FALSE;
    }
    else
    {
        if(dwWriteSize != dwSize)
        {
            dwSize = dwWriteSize;
            if(pdwErrorCode != NULL)
            {
                *pdwErrorCode = errno;
            }
            return FALSE;
        }
        else
        {
            dwSize = dwWriteSize;
            return TRUE;
        }
    }
}

BootSector Disk::GetBootSectorType(const PBYTE pXBR)
{
    /* 用此法判断是否DBR有待确定
    // EB 3C 90 -> FAT12/16
    // EB 58 90 -> FAT32
    // EB 52 90 -> NTFS
    // EB 76 90 -> EXFAT
    */
    MASTER_BOOT_RECORD mbr;
    ZeroMemory(&mbr,sizeof(MASTER_BOOT_RECORD));
    memcpy(&mbr,pXBR,sizeof(MASTER_BOOT_RECORD));

    if (mbr.Signature != 0xAA55)
    {
//		CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetBootSectorType=BootSector_UNKNOWN"));
        return BootSector_UNKNOWN;
    }

    if (ReadOffset(pXBR,0,3) == 0x000000)
    {
//		CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetBootSectorType=BootSector_MBR"));
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

        if (ullStartSector > m_ullSectorNums - 1)
        {
//			CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetBootSectorType=BootSector_DBR"));
            return BootSector_DBR;
        }

        if (mbr.Partition[i].StartLBA == 0x01 && mbr.Partition[i].TotalSector == 0xFFFFFFFF
            && mbr.Partition[i].PartitionType == 0xEE)
        {
//			CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetBootSectorType=BootSector_MBR"));
            return BootSector_MBR;
        }

        BYTE *pDBRTemp = new BYTE[m_dwBytesPerSector];
        ZeroMemory(pDBRTemp,m_dwBytesPerSector);

        if (!ReadSectors(m_hMaster,ullStartSector,1,m_dwBytesPerSector,pDBRTemp,&dwErrorCode))
        {
            delete []pDBRTemp;
            pDBRTemp = NULL;

//			CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetBootSectorType=BootSector_DBR"));
            return BootSector_DBR;
        }

        if (GetFileSystem(pDBRTemp,ullStartSector) != FileSystem_UNKNOWN)
        {
            delete []pDBRTemp;
            pDBRTemp = NULL;

//			CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetBootSectorType=BootSector_MBR"));
            return BootSector_MBR;
        }
        else
        {
            delete []pDBRTemp;
            pDBRTemp = NULL;

//			CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetBootSectorType=BootSector_DBR"));
            return BootSector_DBR;
        }
    }

//	CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetBootSectorType=BootSector_DBR"));
    return BootSector_DBR;
}

PartitionStyle Disk::GetPartitionStyle(const PBYTE pByte, BootSector bootSector)
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
//				CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetPartitionStyle=PartitionStyle_GPT"));
                break;
            }

            if (mbr.Partition[i].PartitionType == 0xEE)
            {
                partitionStyle = PartitionStyle_GPT;
//				CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetPartitionStyle=PartitionStyle_GPT"));
                break;
            }

            partitionStyle = PartitionStyle_DOS;
//			CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetPartitionStyle=PartitionStyle_DOS"));
            break;

        }
    }
    else
    {
        if (ReadOffset(pByte,0,2) == 0x5245)
        {
            partitionStyle = PartitionStyle_APPLE;
//			CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetPartitionStyle=PartitionStyle_APPLE"));
        }
        else if (ReadOffset(pByte,0,4) == 0x82564557)
        {
            partitionStyle = PartitionStyle_BSD;
//			CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetPartitionStyle=PartitionStyle_BSD"));
        }
        else
        {
            partitionStyle = PartitionStyle_UNKNOWN;
//			CUtils::WriteLogFile(m_hLogFile,FALSE,_T("GetPartitionStyle=PartitionStyle_UNKNOWN"));
        }
    }

    return partitionStyle;
}

FileSystem Disk::GetFileSystem(const PBYTE pDBR, ULONGLONG ullStartSector)
{
    FileSystem fileSystem = FileSystem_UNKNOWN;
    if (ReadOffset(pDBR,3,4) == 0x5346544E) //'NTFS'
    {
        fileSystem = FileSystem_NTFS;
//		CUtils::WriteLogFile(m_hLogFile,FALSE,_T("Port %s,Disk %d - GetFileSystem=FileSystem_NTFS")
//			,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum());
    }
    else if (ReadOffset(pDBR,3,5)== 0x5441465845) //'EXFAT'
    {
        fileSystem = FileSystem_EXFAT;
//		CUtils::WriteLogFile(m_hLogFile,FALSE,_T("Port %s,Disk %d - GetFileSystem=FileSystem_EXFAT")
//			,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum());
    }
    else if (ReadOffset(pDBR,0x36,5) == 0x3231544146) //'FAT12'
    {
        fileSystem = FileSystem_FAT12;
//		CUtils::WriteLogFile(m_hLogFile,FALSE,_T("Port %s,Disk %d - GetFileSystem=FileSystem_FAT12")
//			,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum());
    }
    else if (ReadOffset(pDBR,0x36,5) == 0x3631544146) //'FAT16'
    {
        fileSystem = FileSystem_FAT16;
//		CUtils::WriteLogFile(m_hLogFile,FALSE,_T("Port %s,Disk %d - GetFileSystem=FileSystem_FAT16")
//			,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum());
    }
    else if (ReadOffset(pDBR,0x52,5) == 0x3233544146) //'FAT32'
    {
        fileSystem = FileSystem_FAT32;
//		CUtils::WriteLogFile(m_hLogFile,FALSE,_T("Port %s,Disk %d - GetFileSystem=FileSystem_FAT32")
//			,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum());
    }
    else
    {
        // 偏移2个扇区，确定是否是EXT_X文件系统
        BYTE *pByte = new BYTE[BYTES_PER_SECTOR];
        ZeroMemory(pByte,BYTES_PER_SECTOR);

        DWORD dwErrorCode = 0;
        if (ReadSectors(m_hMaster,ullStartSector + 2,1,BYTES_PER_SECTOR,pByte,&dwErrorCode))
        {
            if (ReadOffset(pByte,0x38,2) == 0xEF53)
            {
                fileSystem = FileSystem_EXT_X;
//				CUtils::WriteLogFile(m_hLogFile,FALSE,_T("Port %s,Disk %d - GetFileSystem=FileSystem_EXT_X")
//					,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum());
            }
            else
            {
                fileSystem = FileSystem_UNKNOWN;
//				CUtils::WriteLogFile(m_hLogFile,FALSE,_T("Port %s,Disk %d - GetFileSystem=FileSystem_UNKNOWN")
//					,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum());
            }
        }
        else
        {
            fileSystem = FileSystem_UNKNOWN;
//			CUtils::WriteLogFile(m_hLogFile,FALSE,_T("Port %s,Disk %d - GetFileSystem=FileSystem_UNKNOWN")
//				,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum());
        }
        delete []pByte;
        pByte = NULL;
    }

    return fileSystem;
}

BOOL Disk::BriefAnalyze()
{
    DWORD dwErrorCode = 0;

    BYTE *pXBR = new BYTE[m_dwBytesPerSector];
    ZeroMemory(pXBR,m_dwBytesPerSector);
    if (!ReadSectors(m_hMaster,0,1,m_dwBytesPerSector,pXBR,&dwErrorCode))
    {
//        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Read sector 0 error,system errorcode=%ld,%s")
//            ,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

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

            if (!ReadSectors(m_hMaster,1,1,m_dwBytesPerSector,pEFI,&dwErrorCode))
            {
//                m_pMasterPort->SetEndTime(CTime::GetCurrentTime());
//                m_pMasterPort->SetPortState(PortState_Fail);
//                m_pMasterPort->SetResult(FALSE);
//                m_pMasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Read EFI info error,system errorcode=%ld,%s")
//                    ,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

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
                if (!ReadSectors(m_hMaster,ullStartSectors+i,1,m_dwBytesPerSector,pByte,&dwErrorCode))
                {
//                    m_pMasterPort->SetEndTime(CTime::GetCurrentTime());
//                    m_pMasterPort->SetPortState(PortState_Fail);
//                    m_pMasterPort->SetResult(FALSE);
//                    m_pMasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//                    CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - StartSector:%ld - Read GUID Partition Table Entry #%d erorr,system errorcode=%ld,%s")
//                        ,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum(),ullStartSectors,dwEntryIndex,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

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
                    if (!ReadSectors(m_hMaster,ullTempStartSector,1,m_dwBytesPerSector,pTempDBR,&dwErrorCode))
                    {
//                        m_pMasterPort->SetEndTime(CTime::GetCurrentTime());
//                        m_pMasterPort->SetPortState(PortState_Fail);
//                        m_pMasterPort->SetResult(FALSE);
//                        m_pMasterPort->SetErrorCode(ErrorType_System,dwErrorCode);


//                        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - StartSector:%ld - Read GUID Partition Table Entry #%d erorr,system errorcode=%ld,%s")
//                            ,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum(),ullTempStartSector,dwEntryIndex,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

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
    default:
        return FALSE;
//        break;
    }

    delete []pXBR;
    pXBR = NULL;

    return TRUE;
}

BOOL Disk::AppendEffDataList(const PBYTE pDBR, FileSystem fileSystem, ULONGLONG ullStartSector, ULONGLONG ullMasterSectorOffset, ULONGLONG ullSectors, BOOL bMBR)
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
            if (!ReadSectors(m_hMaster,ullFatStartSectors,fatInfo.dwFATLength,fatInfo.wBytesPerSector,pFATByte,&dwErrorCode))
            {
                struct timeval tval;
                gettimeofday(&tval,NULL);
                m_pMasterPort->SetEndTime(tval);
                m_pMasterPort->SetPortState(PortState_Fail);
                m_pMasterPort->SetResult(FALSE);
                m_pMasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Read FAT Table error,system errorcode=%ld,%s")
//                    ,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

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
            if (!ReadSectors(m_hMaster,ullTempStartSector,1,exfatInfo.wBytesPerSector,pRoot,&dwErrorCode))
            {
                struct timeval tval;
                gettimeofday(&tval,NULL);
                m_pMasterPort->SetEndTime(tval);
                m_pMasterPort->SetPortState(PortState_Fail);
                m_pMasterPort->SetResult(FALSE);
                m_pMasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %d,Disk %d - Read EXFAT Root Directory error,system errorcode=%ld,%s")
//                    ,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

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
            if (!ReadSectors(m_hMaster,ullTempStartSector,dwBitMapLength/exfatInfo.wBytesPerSector,exfatInfo.wBytesPerSector,pBitMap,&dwErrorCode))
            {
                struct timeval tval;
                gettimeofday(&tval,NULL);
                m_pMasterPort->SetEndTime(tval);
                m_pMasterPort->SetPortState(PortState_Fail);
                m_pMasterPort->SetResult(FALSE);
                m_pMasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Read EXFAT Bitmap error,system errorcode=%ld,%s")
//                    ,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

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
            if (!ReadSectors(m_hMaster,ullBitMapMFTStartSector,1024/ntfsInfo.wBytesPerSectors,ntfsInfo.wBytesPerSectors,pMFT,&dwErrorCode))
            {
                struct timeval tval;
                gettimeofday(&tval,NULL);
                m_pMasterPort->SetEndTime(tval);
                m_pMasterPort->SetPortState(PortState_Fail);
                m_pMasterPort->SetResult(FALSE);
                m_pMasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Read Bitmmap MFT error,system errorcode=%ld,%s")
//                    ,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

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
                if (!ReadSectors(m_hMaster,ullTempStart,ntfsInfo.bySectorsPerCluster,ntfsInfo.wBytesPerSectors,pBitmap,&dwErrorCode))
                {
                    struct timeval tval;
                    gettimeofday(&tval,NULL);
                    m_pMasterPort->SetEndTime(tval);
                    m_pMasterPort->SetPortState(PortState_Fail);
                    m_pMasterPort->SetResult(FALSE);
                    m_pMasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//                    CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Read Bitmap data error,system errorcode=%ld,%s")
//                        ,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

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

            if (!ReadSectors(m_hMaster,ullTempStartSector,1,m_dwBytesPerSector,pSuperBlock,&dwErrorCode))
            {
                struct timeval tval;
                gettimeofday(&tval,NULL);
                m_pMasterPort->SetEndTime(tval);
                m_pMasterPort->SetPortState(PortState_Fail);
                m_pMasterPort->SetResult(FALSE);
                m_pMasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Read Super Block error,system errorcode=%ld,%s")
//                    ,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));


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
            if (!ReadSectors(m_hMaster,ullTempStartSector,dwGroupDescriptionBlocks * superBlockInfo.dwSectorsPerBlock,m_dwBytesPerSector,pGroupDescrption,&dwErrorCode))
            {
                struct timeval tval;
                gettimeofday(&tval,NULL);
                m_pMasterPort->SetEndTime(tval);
                m_pMasterPort->SetPortState(PortState_Fail);
                m_pMasterPort->SetResult(FALSE);
                m_pMasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Read Group Block Description error,system errorcode=%ld,%s")
//                    ,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

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
                if (!ReadSectors(m_hMaster,ullTempStartSector,dwBlockBitMapLength/m_dwBytesPerSector,m_dwBytesPerSector,pBitMap,&dwErrorCode))
                {
                    struct timeval tval;
                    gettimeofday(&tval,NULL);
                    m_pMasterPort->SetEndTime(tval);
                    m_pMasterPort->SetPortState(PortState_Fail);
                    m_pMasterPort->SetResult(FALSE);
                    m_pMasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//                    CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Read Block Bitmap error,system errorcode=%ld,%s")
//                        ,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

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
                CUtils::WriteLogFile(m_hLogFile,FALSE,strPartions);

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

                if (!ReadSectors(m_hMaster,ullTempStartSector,1,m_dwBytesPerSector,pDBRTemp,&dwErrorCode))
                {
                    struct timeval tval;
                    gettimeofday(&tval,NULL);
                    m_pMasterPort->SetEndTime(tval);
                    m_pMasterPort->SetPortState(PortState_Fail);
                    m_pMasterPort->SetResult(FALSE);
                    m_pMasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

                    CString strErrMsg;
                    strErrMsg.Format(_T("Partition=%d,FileSystem=%02X,StartLBA=0x%X,Sectors=0x%X - Read DBR Error"),
                        i+1,ebr.Partition[i].PartitionType,ebr.Partition[i].StartLBA,ebr.Partition[i].TotalSector);

//                    CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - %s,system errorcode=%ld,%s")
//                        ,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum(),strErrMsg,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

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
    default:
        return FALSE;
//        break;
    }

    return TRUE;
}

ULONGLONG Disk::ReadOffset(const PBYTE pByte, DWORD offset, BYTE bytes)
{
    assert(bytes <= 8 && bytes > 0);
    ULONGLONG ullMask = 0xFFFFFFFFFFFFFFFF;
    ullMask = (ullMask >> (sizeof(ULONGLONG) - sizeof(BYTE) * bytes) * 8);

    ULONGLONG ullValue = *(PULONGLONG)(pByte+offset) & ullMask;
    return ullValue;
}

BOOL Disk::IsFATValidCluster(const PBYTE pByte, DWORD cluster, BYTE byFATBit)
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

BOOL Disk::IsNTFSValidCluster(const PBYTE pByte, DWORD cluster)
{
    DWORD dwByteIndex = cluster/8;
    BYTE  bitIndex = cluster % 8;
    BYTE byValue = (BYTE)ReadOffset(pByte,dwByteIndex,1);

    return ((byValue >> bitIndex) & 0x1);
}

BOOL Disk::IsExtXValidBlock(const PBYTE pByte, DWORD block)
{
    DWORD dwByteIndex = block/8;
    BYTE  bitIndex = block % 8;
    BYTE byValue = (BYTE)ReadOffset(pByte,dwByteIndex,1);

    return ((byValue >> bitIndex) & 0x1);
}

void Disk::SetValidSize(ULONGLONG ullSize)
{
    m_pMasterPort->SetValidSize(ullSize);

    PortList::const_iterator it = m_TargetPorts->begin();
    PortList::const_iterator it_end = m_TargetPorts->end();
    while (it != it_end)
    {
        CPort *port = (*it);
        it++;
        if (port->IsConnected())
        {
            port->SetValidSize(ullSize);
        }
    }
}

ULONGLONG Disk::GetValidSize()
{
    ULONGLONG ullValidSize = 0;

    EFF_LIST::const_iterator it = m_EffList.begin();
    EFF_LIST::const_iterator it_end = m_EffList.end();

    while (it != it_end)
    {
        EFF_DATA effData = (*it);
        it++;
        ullValidSize += effData.ullSectors * effData.wBytesPerSector;
    }

    return ullValidSize;
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
                data->szFileName = new CHAR[strlen(dataInfo->szFileName) + 1];
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



UINT Disk::GetCurrentTargetCount()
{
    UINT nCount = 0;
    PortList::const_iterator it = m_TargetPorts->begin();
    PortList::const_iterator it_end = m_TargetPorts->end();

    while (it != it_end)
    {
        CPort *port = (*it);
        it++;
        if (port)
        {
            if (port->IsConnected() && port->GetResult())
            {
                nCount++;
            }
        }
    }

    return nCount;
}


BOOL Disk::QuickClean(CPort *port, PDWORD pdwErrorCode)
{
    BOOL bResult = TRUE;
    HANDLE hDisk = GetHandleOnPhysicalDrive(port->GetDevicePath(),O_RDWR,pdwErrorCode);

    if(hDisk <0)
    {
        DEBUG("get disk handle error!");
        return FALSE;
    }
    BYTE *pByte = new BYTE[CLEAN_LENGTH];
    ZeroMemory(pByte,CLEAN_LENGTH);

    if (SetDiskAtrribute(hDisk,FALSE,TRUE,pdwErrorCode))
    {
        DWORD dwBytesPerSector = 0;
        ULONGLONG ullSectorNums = GetNumberOfSectors(hDisk,&dwBytesPerSector,NULL);
        DWORD dwSectors = CLEAN_LENGTH/dwBytesPerSector;
//        DWORD dwSize = 0;

        if (!WriteSectors(hDisk,0,dwSectors,dwBytesPerSector,pByte,pdwErrorCode))
        {
            bResult = FALSE;
        }

        if (!WriteSectors(hDisk,ullSectorNums-dwSectors,dwSectors,dwBytesPerSector,pByte,pdwErrorCode))
        {
            bResult = FALSE;
        }

        if (bResult)
        {
//            //fresh the partition table
//            bResult = DeviceIoControl(
//                hDisk,
//                IOCTL_DISK_UPDATE_PROPERTIES,
//                NULL,
//                0,
//                NULL,
//                0,
//                &dwSize,
//                NULL
//                );
//            if (!bResult)
//            {
//                *pdwErrorCode = GetLastError();
//            }
        }

    }
    else
    {
        bResult = FALSE;
    }

    close(hDisk);

    delete []pByte;

    return bResult;
}

BOOL Disk::QuickClean(HANDLE hDisk, CPort *port, PDWORD pdwErrorCode)
{
    USE_PARAM(port);

    BOOL bResult = TRUE;
    BYTE *pByte = new BYTE[CLEAN_LENGTH];
    ZeroMemory(pByte,CLEAN_LENGTH);

    DWORD dwBytesPerSector = 0;
    ULONGLONG ullSectorNums = GetNumberOfSectors(hDisk,&dwBytesPerSector,NULL);
    DWORD dwSectors = CLEAN_LENGTH/dwBytesPerSector;
//    DWORD dwSize = 0;

    if (!WriteSectors(hDisk,0,dwSectors,dwBytesPerSector,pByte,pdwErrorCode))
    {
        bResult = FALSE;
    }

    if (!WriteSectors(hDisk,ullSectorNums-dwSectors,dwSectors,dwBytesPerSector,pByte,pdwErrorCode))
    {
        bResult = FALSE;
    }

    delete []pByte;

    return bResult;
}

void *Disk::ReadDiskThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void *Disk::ReadImageThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void *Disk::WriteDiskThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void *Disk::WriteDiskWithDataThreadProc(LPVOID lpParam)
{
    USE_PARAM(lpParam);
    return NULL;
}

void *Disk::WriteImageThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void *Disk::VerifyThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void *Disk::CompressThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void *Disk::UnCompressThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void* Disk::CleanDiskThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void *Disk::VerifySectorThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void *Disk::ReadFilesThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void *Disk::WriteFilesThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void *Disk::VerifyFilesThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void *Disk::VerifyFilesByteThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void *Disk::FormatDiskThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void *Disk::SearchUserLogThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void *Disk::DeleteChangeThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void *Disk::EnumFileThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void *Disk::ComputeHashThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void *Disk::FullRWTestThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void *Disk::FadePickerThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void *Disk::SpeedCheckThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void *Disk::CompareCleanThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

void *Disk::CopyFilesAsyncThreadProc(LPVOID lpParm)
{
    USE_PARAM(lpParm);
    return NULL;
}

BOOL Disk::OnCopyFilesAsync()
{
    return FALSE;
}

BOOL Disk::OnCopyDisk()
{
    BOOL bResult = FALSE;
    DWORD /*dwReadId,dwWriteId,dwVerifyId,*/dwErrorCode;

    m_bEnd = TRUE;

    switch (m_WorkMode)
    {
    case WorkMode_FullCopy:
        {
            EFF_DATA effData;
            effData.ullStartSector = 0;
            effData.ullSectors = m_ullSectorNums;
            effData.wBytesPerSector = (WORD)m_dwBytesPerSector;

            m_EffList.push_back(effData);

            m_ullValidSize = GetValidSize();
            SetValidSize(m_ullValidSize);
        }

        break;

    case WorkMode_QuickCopy:
        {
//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Analyze start......"));
            if (!BriefAnalyze())
            {
                return FALSE;
            }

            // 是否有用户指定区域,如果存在直接加载列表后面
            CListRangeFromTo::const_iterator it = m_ListRangeFromTo.begin();
            CListRangeFromTo::const_iterator it_end = m_ListRangeFromTo.end();

            while (it != it_end)
            {
                RANGE_FROM_TO range = (*it);
                it++;

                CString strList;
                strList.Format(_T("User custom area --- StartSector:%I64d  EndSector:%I64d")
                    ,range.ullStartLBA,range.ullEndingLBA);

//                CUtils::WriteLogFile(m_hLogFile,FALSE,strList);

//                if (range.ullStartLBA < m_ullSectorNums && range.ullEndingLBA <= m_ullCapacity)
                if (range.ullStartLBA < m_ullSectorNums && range.ullEndingLBA <= m_ullSectorNums)
                {
                    EFF_DATA effData ;
                    ZeroMemory(&effData,sizeof(effData));
                    effData.ullStartSector = range.ullStartLBA;
                    effData.ullSectors = range.ullEndingLBA - range.ullStartLBA;
//                    effData.wBytesPerSector = BYTES_PER_SECTOR;
                     effData.wBytesPerSector = m_dwBytesPerSector;

                    if (effData.ullSectors != 0)
                    {
                        m_EffList.push_back(effData);
                    }
                }
            }

            m_ullValidSize = GetValidSize();
            SetValidSize(m_ullValidSize);

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Analyze end, valid data size=%I64d"),m_ullValidSize);

        }
        break;
    default:
        ERROR("WorkMode is invalid!");
        break;
    }

//    CUtils::WriteLogFile(m_hLogFile,FALSE,_T("Valid Data List:"));
    CString strList;
    EFF_LIST::const_iterator it = m_EffList.begin();
    EFF_LIST::const_iterator it_end = m_EffList.end();
    while (it != it_end)
    {
        EFF_DATA effData = (*it);
        it++;
        strList.Format(_T("StartSector:%I64d  Sectors:%I64d  BytesPerSector:%d"),effData.ullStartSector,effData.ullSectors,effData.wBytesPerSector);
//        CUtils::WriteLogFile(m_hLogFile,FALSE,strList);
    }

    // 是否要执行全盘擦除
    if (m_bCleanDiskFirst)
    {
//        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Clean disk first......"));

        m_CleanMode = CleanMode_Full;

        char function[255] = {0};

        m_bEnd = FALSE;

        //子盘写线程

        for (int times = 0; times < m_nCleanTimes;times++)
        {
            snprintf(function,255,"DISK CLEAN %d/%d",times+1,m_nCleanTimes);
//            ::SendMessage(m_hWnd,WM_UPDATE_FUNCTION,(WPARAM)function,0);

            m_nFillValue = m_pCleanValues[times];

            DWORD dwLen = m_dwBytesPerSector * m_nBlockSectors;

            if (m_pFillBytes != NULL)
            {
                delete []m_pFillBytes;
            }

            m_pFillBytes = new BYTE[dwLen];

            if (m_nFillValue != RANDOM_VALUE)
            {
                memset(m_pFillBytes,m_nFillValue,dwLen);
            }
            else
            {
                srand((unsigned int)time(NULL));

                for (DWORD i = 0;i < dwLen;i++)
                {
                    m_pFillBytes[i] = (BYTE)rand();
                }
            }

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Disk Copy - DISK CLEAN %d/%d,fill value %d"),times+1,m_nCleanTimes,m_nFillValue);

            UINT nCurrentTargetCount = GetCurrentTargetCount();

            pthread_t *pthWriteThreads = new pthread_t[nCurrentTargetCount];

            UINT nCount = 0;
            PortList::const_iterator it = m_TargetPorts->begin();
            PortList::const_iterator it_end =  m_TargetPorts->end();

            while (it != it_end)
            {
                CPort *port = (*it);
                it++;
                if (port->IsConnected() && port->GetResult())
                {
                    LPVOID_PARM lpVoid = new VOID_PARM;
                    lpVoid->lpVoid1 = this;
                    lpVoid->lpVoid2 = port;

                    pthread_create(&pthWriteThreads[nCount],NULL,CleanDiskThreadProc,lpVoid);

//                    CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - CreateCleanDiskThread,ThreadId=0x%04X,HANDLE=0x%04X")
//                        ,port->GetPortName(),port->GetDiskNum(),dwWriteId,hWriteThreads[nCount]);

                    nCount++;
                }
            }

            bResult = FALSE;
            for (UINT i = 0; i < nCount;i++)
            {
                int *val;
                pthread_join(pthWriteThreads[i],(void **)&val);
                dwErrorCode = *val;
                bResult |= dwErrorCode;
            }
            delete []pthWriteThreads;

            if (!bResult)
            {
                m_pMasterPort->SetResult(FALSE);
                struct timeval tval;
                gettimeofday(&tval,NULL);
                m_pMasterPort->SetEndTime(tval);
                m_pMasterPort->SetPortState(PortState_Fail);

                if (*m_lpCancel)
                {
                    m_pMasterPort->SetErrorCode(ErrorType_Custom,CustomError_Cancel);
                }
                else
                {
                    // 任意取一个错误
                    ErrorType errType = ErrorType_System;
                    DWORD dwErrorCode = 0;

//                    pos = m_TargetPorts->GetHeadPosition();
                    PortList::const_iterator itPort = m_TargetPorts->begin();
                    PortList::const_iterator itPort_end = m_TargetPorts->end();

                    while (itPort != itPort_end)
                    {
                        CPort *port = (*itPort);
                        itPort++;
                        if (port->IsConnected() && !port->GetResult())
                        {
                            errType = port->GetErrorCode(&dwErrorCode);
                            break;
                        }
                    }
                    m_pMasterPort->SetErrorCode(errType,dwErrorCode);
                }

                return FALSE;
            }

            //是写过程中比较还是写之后比较
            if (m_bCompareClean && m_CompareCleanSeq == CompareCleanSeq_After)
            {
                snprintf(function,255,"COMPARE CLEAN %d/%d",times+1,m_nCleanTimes);
//                ::SendMessage(m_hWnd,WM_UPDATE_FUNCTION,(WPARAM)function,0);

//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Disk Copy - COMPARE CLEAN %d/%d"),times+1,m_nCleanTimes);

                nCurrentTargetCount = GetCurrentTargetCount();
                pthread_t *pthCompareCleanThreads = new pthread_t[nCurrentTargetCount];

                nCount = 0;

                PortList::const_iterator itPort = m_TargetPorts->begin();
                PortList::const_iterator itPort_end = m_TargetPorts->end();

                while (itPort != itPort_end)
                {
                    CPort *port = (*itPort);
                    itPort++;
                    if (port->IsConnected() && port->GetResult())
                    {
                        LPVOID_PARM lpVoid = new VOID_PARM;
                        lpVoid->lpVoid1 = this;
                        lpVoid->lpVoid2 = port;

                        pthread_create(&pthCompareCleanThreads[nCount],NULL,CompareCleanThreadProc,lpVoid);

//                        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - CreateCompareCleanThread,ThreadId=0x%04X,HANDLE=0x%04X")
//                            ,port->GetPortName(),port->GetDiskNum(),dwWriteId,hWriteThreads[nCount]);

                        nCount++;
                    }
                }

                //等待线程结束

                bResult = FALSE;
                for (UINT i = 0; i < nCount;i++)
                {
                    int *val;
                    pthread_join(pthCompareCleanThreads[i],(void **)&val);
                    dwErrorCode = *val;
                    bResult |= dwErrorCode;

                }
                delete []pthCompareCleanThreads;

                if (!bResult)
                {
                    struct timeval tval;
                    gettimeofday(&tval,NULL);
                    m_pMasterPort->SetResult(FALSE);
                    m_pMasterPort->SetEndTime(tval);
                    m_pMasterPort->SetPortState(PortState_Fail);

                    if (*m_lpCancel)
                    {
                        m_pMasterPort->SetErrorCode(ErrorType_Custom,CustomError_Cancel);
                    }
                    else
                    {
                        // 任意取一个错误
                        ErrorType errType = ErrorType_System;
                        DWORD dwErrorCode = 0;

//                        pos = m_TargetPorts->GetHeadPosition();
                        itPort = m_TargetPorts->begin();
                        itPort_end = m_TargetPorts->end();
                        while (itPort != itPort_end)
                        {
                            CPort *port = (*itPort);
                            itPort++;
                            if (port->IsConnected() && !port->GetResult())
                            {
                                errType = port->GetErrorCode(&dwErrorCode);
                                break;
                            }
                        }
                        m_pMasterPort->SetErrorCode(errType,dwErrorCode);
                    }

                    return FALSE;
                }
            }

        }

        if (m_WorkMode == WorkMode_FullCopy)
        {
            strncpy(function,"FULL COPY",sizeof("FULL COPY"));
        }
        else
        {
            strncpy(function,"QUICK COPY",sizeof("QUICK COPY"));
        }
//        ::SendMessage(m_hWnd,WM_UPDATE_FUNCTION,(WPARAM)function,0);

//        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Clean disk completed"));
    }

    // 开始拷贝之前，清空DataQueue中的数据
//    ClearDataQueueList();
    m_DataQueue.Clear();

    SetValidSize(m_ullValidSize);

    pthread_t pthReadThread ;
    pthread_create(&pthReadThread,NULL,ReadDiskThreadProc,this);

//    CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Disk Copy(Master) - CreateReadDiskThread,ThreadId=0x%04X,HANDLE=0x%04X")
//        ,dwReadId,hReadThread);

    UINT nCount = 0;
    UINT nCurrentTargetCount = GetCurrentTargetCount();

    pthread_t *pthWriteThreads = new pthread_t[nCurrentTargetCount];
//    pthread_t pthWriteDiskWithDataThreads;

//    LPVOID_PARM pVoidData = new VOID_PARM;
//    pVoidData->lpVoid1 = this;
//    pVoidData->lpVoid2 = NULL;
//    pVoidData->lpVoid3 = &m_DataQueue;

//    pthread_create(&pthWriteDiskWithDataThreads,NULL,WriteDiskWithDataThreadProc,pVoidData);
    PortList::const_iterator itPort = m_TargetPorts->begin();
    PortList::const_iterator itPort_end = m_TargetPorts->end();
    while (itPort != itPort_end)
    {
        CPort *port = (*itPort);
        itPort ++;
        CDataQueue *dataQueue = &m_DataQueue;
        if (port->IsConnected() && port->GetResult())
        {
            LPVOID_PARM lpVoid = new VOID_PARM;
            lpVoid->lpVoid1 = this;
            lpVoid->lpVoid2 = port;
            lpVoid->lpVoid3 = dataQueue;

            pthread_create(&pthWriteThreads[nCount],NULL,WriteDiskThreadProc,lpVoid);
//            hWriteThreads[nCount] = CreateThread(NULL,0,WriteDiskThreadProc,lpVoid,0,&dwWriteId);

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - CreateWriteDiskThread,ThreadId=0x%04X,HANDLE=0x%04X")
//                ,port->GetPortName(),port->GetDiskNum(),dwWriteId,hWriteThreads[nCount]);

            nCount++;
        }
    }

//    等待写磁盘线程结束
    bResult = FALSE;
    int *val;
//    pthread_join(pthWriteDiskWithDataThreads,(void **)&val);
//    dwErrorCode = *val;
//    bResult |= dwErrorCode;

    for (UINT i = 0; i < nCount;i++)
    {
        pthread_join(pthWriteThreads[i],(void **)&val);
        dwErrorCode = *val;
        bResult |= dwErrorCode;
    }
    delete []pthWriteThreads;

    //等待读磁盘线程结束
    pthread_join(pthReadThread,(void **)&val);
    dwErrorCode = *val;
    bResult &= dwErrorCode;

    if (bResult && m_bCompare)
    {
        m_bVerify = TRUE;
        char function[255] = "VERIFY";
//        ::SendMessage(m_hWnd,WM_UPDATE_FUNCTION,(WPARAM)function,0);

        m_bEnd = TRUE;

        nCurrentTargetCount = GetCurrentTargetCount();


        pthread_t *pthVerifyThreads = new pthread_t[nCurrentTargetCount];

        // 如果允许容量误差，并且设置的误差不是0，则需要安装Sector by Sector的方式进行比较
        if (m_bAllowCapGap && m_nCapGapPercent != 0)
        {
            m_CompareMethod = CompareMethod_Byte;
        }

        if (m_CompareMethod == CompareMethod_Byte)
        {
            m_bComputeHash = FALSE;

            // 开始拷贝之前，清空DataQueue中的数据


            pthread_create(&pthReadThread,NULL,ReadDiskThreadProc,this);

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Read Disk(Master) - CreateReadDiskThread,ThreadId=0x%04X,HANDLE=0x%04X")
//                ,dwReadId,hReadThread);

            nCount = 0;
//            pos = m_TargetPorts->GetHeadPosition();
            itPort = m_TargetPorts->begin();
            itPort_end = m_TargetPorts->end();
//            posQueue = m_DataQueueList.GetHeadPosition();
            while (itPort != itPort_end)
            {
                CPort *port = (*itPort);
                itPort ++;
                CDataQueue *dataQueue = &m_DataQueue;
//				dataQueue->Clear();
                if (port->IsConnected() && port->GetResult())
                {
                    LPVOID_PARM lpVoid = new VOID_PARM;
                    lpVoid->lpVoid1 = this;
                    lpVoid->lpVoid2 = port;
                    lpVoid->lpVoid3 = dataQueue;
                    pthread_create(&pthVerifyThreads[nCount],NULL,VerifySectorThreadProc,lpVoid);

//                    hVerifyThreads[nCount] = CreateThread(NULL,0,VerifySectorThreadProc,lpVoid,0,&dwVerifyId);

//                    CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - CreateVerifyDiskSectorThread,ThreadId=0x%04X,HANDLE=0x%04X")
//                        ,port->GetPortName(),port->GetDiskNum(),dwVerifyId,hVerifyThreads[nCount]);

                    nCount++;
                }
            }

            //等待校验线程结束
//            WaitForMultipleObjects(nCount,hVerifyThreads,TRUE,INFINITE);
            bResult = FALSE;
            for (UINT i = 0; i < nCount;i++)
            {
                pthread_join(pthVerifyThreads[i],(void **)&val);
                dwErrorCode = *val;
                bResult |= dwErrorCode;
            }
            delete []pthVerifyThreads;


            //等待读磁盘线程结束
            pthread_join(pthReadThread,(void **)&val);
            dwErrorCode = *val;
            bResult &= dwErrorCode;

        }
        else
        {
            nCount = 0;
//            pos = m_TargetPorts->GetHeadPosition();
            itPort = m_TargetPorts->begin();
            itPort_end = m_TargetPorts->end();

            while (itPort != itPort_end)
            {
                CPort *port = (*itPort);
                itPort++;
                if (port->IsConnected() && port->GetResult())
                {
                    CHashMethod *pHashMethod;

                    switch (port->GetHashMethod())
                    {
                    case HashMethod_CHECKSUM32:
                        pHashMethod = new CChecksum32();
                        break;

                    case HashMethod_CRC32:
                        pHashMethod = new CCRC32();
                        break;

                    case HashMethod_MD5:
                        pHashMethod = new MD5();
                        break;
                    }

                    LPVOID_PARM lpVoid = new VOID_PARM;
                    lpVoid->lpVoid1 = this;
                    lpVoid->lpVoid2 = port;
                    lpVoid->lpVoid3 = pHashMethod;

                    pthread_create(&pthVerifyThreads[nCount],NULL,VerifyThreadProc,lpVoid);

//                    CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - CreateVerifyDiskThread,ThreadId=0x%04X,HANDLE=0x%04X")
//                        ,port->GetPortName(),port->GetDiskNum(),dwVerifyId,hVerifyThreads[nCount]);

                    nCount++;
                }
            }

            //等待校验线程结束
            bResult = FALSE;
            for (UINT i = 0; i < nCount;i++)
            {
                pthread_join(pthVerifyThreads[i],(void **)&val);
                dwErrorCode = *val;
                bResult |= dwErrorCode;
            }
            delete []pthVerifyThreads;
        }

        if (!bResult)
        {
            // 任意取一个错误
            if (*m_lpCancel)
            {
                m_pMasterPort->SetErrorCode(ErrorType_Custom,CustomError_Cancel);
            }
            else
            {
                ErrorType errType = ErrorType_System;
                DWORD dwErrorCode = 0;
                errType = m_pMasterPort->GetErrorCode(&dwErrorCode);

                if (dwErrorCode == 0)
                {
//                    pos = m_TargetPorts->GetHeadPosition();
                    itPort = m_TargetPorts->begin();
                    itPort_end = m_TargetPorts->end();
                    while (itPort != itPort_end)
                    {
                        CPort *port = (*itPort);
                        itPort++;
                        if (port->IsConnected() && !port->GetResult())
                        {
                            errType = port->GetErrorCode(&dwErrorCode);
                            break;
                        }
                    }

                    m_pMasterPort->SetErrorCode(errType,dwErrorCode);
                }
            }

        }
    }

    return bResult;
}

BOOL Disk::OnCopyImage()
{
    IMAGE_HEADER imgHead ;
    ZeroMemory(&imgHead,sizeof(IMAGE_HEADER));
    DWORD dwErrorCode = 0;
    DWORD dwLen = SIZEOF_IMAGE_HEADER;

    if (m_bServerFirst)
    {
        CString strImageName = m_pMasterPort->GetFileName();//CUtils::GetFileName(m_pMasterPort->GetFileName());
//        char *fileName = W2A(strImageName);
        const char * const fileName = strImageName.c_str();

        dwLen = sizeof(CMD_IN) + strlen(fileName) + 2;

        BYTE *bufSend = new BYTE[dwLen];
        ZeroMemory(bufSend,dwLen);
        bufSend[dwLen - 1] = END_FLAG;

        CMD_IN queryImageIn ;
        ZeroMemory(&queryImageIn,sizeof(CMD_IN));
        queryImageIn.dwCmdIn = CMD_QUERY_IMAGE_IN;
        queryImageIn.dwSizeSend = dwLen;

        memcpy(bufSend,&queryImageIn,sizeof(CMD_IN));
        memcpy(bufSend + sizeof(CMD_IN),fileName,strlen(fileName));


        if (send(m_ClientSocket,(char *)bufSend,dwLen,0)<0)
        {
            delete []bufSend;
            struct timeval tval;
            gettimeofday(&tval,NULL);
            m_pMasterPort->SetEndTime(tval);
            m_pMasterPort->SetPortState(PortState_Fail);
            m_pMasterPort->SetResult(FALSE);
            m_pMasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Query image from server error,filename=%s,system errorcode=%ld,%s")
//                ,strImageName,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));


            return FALSE;
        }

        delete []bufSend;

        QUERY_IMAGE_OUT queryImageOut ;
        ZeroMemory(&queryImageOut,sizeof(QUERY_IMAGE_OUT));
        dwLen = sizeof(QUERY_IMAGE_OUT);
        if (recv(m_ClientSocket,(char *)&queryImageOut,dwLen,0)<0)
        {
            struct timeval tval;
            m_pMasterPort->SetEndTime(tval);
            m_pMasterPort->SetPortState(PortState_Fail);
            m_pMasterPort->SetResult(FALSE);
            m_pMasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Query image from server error,filename=%s,system errorcode=%ld,%s")
//                ,strImageName,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

            return FALSE;
        }


        if (queryImageOut.dwErrorCode != 0 || queryImageOut.dwCmdOut != CMD_QUERY_IMAGE_OUT || queryImageOut.dwSizeSend != dwLen)
        {
            dwErrorCode = CustomError_Get_Data_From_Server_Error;
            struct timeval tval;
            m_pMasterPort->SetEndTime(tval);
            m_pMasterPort->SetPortState(PortState_Fail);
            m_pMasterPort->SetResult(FALSE);
            m_pMasterPort->SetErrorCode(ErrorType_Custom,dwErrorCode);

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Query image from server error,filename=%s,custom errorcode=0x%X,get data from server error")
//                ,strImageName,dwErrorCode);

            return FALSE;
        }

        memcpy(&imgHead,queryImageOut.byHead,SIZEOF_IMAGE_HEADER);

        if (m_hMaster >= 0)
        {
            dwLen = SIZEOF_IMAGE_HEADER;
            WriteFileAsyn(m_hMaster,0,dwLen,(LPBYTE)&imgHead,&dwErrorCode);
        }
    }
    else
    {
        if (!ReadFileAsyn(m_hMaster,0,dwLen,(LPBYTE)&imgHead,&dwErrorCode))
        {
            struct timeval tval;
            m_pMasterPort->SetEndTime(tval);
            m_pMasterPort->SetPortState(PortState_Fail);
            m_pMasterPort->SetResult(FALSE);
            m_pMasterPort->SetErrorCode(ErrorType_System,dwErrorCode);

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Read Image Head error,filename=%s,system errorcode=%ld,%s")
//                ,m_pMasterPort->GetFileName(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

            return FALSE;
        }
    }

    m_ullValidSize = imgHead.ullValidSize;
    m_ullSectorNums = imgHead.ullCapacitySize / imgHead.dwBytesPerSector;
    m_ullCapacity = imgHead.ullCapacitySize;
    m_ullImageSize = imgHead.ullImageSize;
    m_dwBytesPerSector = imgHead.dwBytesPerSector;
    m_bDataCompress = (imgHead.byUnCompress == 0) ? TRUE : FALSE;

    HashMethod hashMethod = (HashMethod)imgHead.dwHashType;

    memcpy(m_ImageHash,imgHead.byImageDigest,imgHead.dwHashLen);

    SetValidSize(m_ullValidSize);
    SetHashMethod(m_bComputeHash,hashMethod);

    m_pMasterPort->SetHashMethod(hashMethod);
//    POSITION pos = m_TargetPorts->GetHeadPosition();
    PortList::const_iterator itPort = m_TargetPorts->begin();
    PortList::const_iterator itPort_end = m_TargetPorts->end();
    while (itPort != itPort_end)
    {
        CPort *port = (*itPort);
        itPort ++;
        if (port->IsConnected())
        {
            port->SetHashMethod(hashMethod);
        }
    }


    BOOL bResult = FALSE;
//    DWORD dwReadId,dwWriteId,dwVerifyId,dwUncompressId;

    m_bEnd = TRUE;

    // 是否要执行全盘擦除
    if (m_bCleanDiskFirst)
    {
        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Clean disk first......"));

        m_CleanMode = CleanMode_Full;
        char function[255] = {0};

        m_bEnd = FALSE;

        //子盘写线程

        for (int times = 0; times < m_nCleanTimes;times++)
        {
            snprintf(function,255,"DISK CLEAN %d/%d",times+1,m_nCleanTimes);
//            ::SendMessage(m_hWnd,WM_UPDATE_FUNCTION,(WPARAM)function,0);

            m_nFillValue = m_pCleanValues[times];

            DWORD dwLen = BYTES_PER_SECTOR * m_nBlockSectors;

            if (m_pFillBytes != NULL)
            {
                delete []m_pFillBytes;
            }

            m_pFillBytes = new BYTE[dwLen];

            if (m_nFillValue != RANDOM_VALUE)
            {
                memset(m_pFillBytes,m_nFillValue,dwLen);
            }
            else
            {
                srand((unsigned int)time(NULL));

                for (DWORD i = 0;i < dwLen;i++)
                {
                    m_pFillBytes[i] = (BYTE)rand();
                }
            }

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Disk Copy - DISK CLEAN %d/%d,fill value %d"),times+1,m_nCleanTimes,m_nFillValue);

            UINT nCurrentTargetCount = GetCurrentTargetCount();
//            HANDLE *hWriteThreads = new HANDLE[nCurrentTargetCount];
            pthread_t *pthWriteThreads = new pthread_t[nCurrentTargetCount];

            UINT nCount = 0;
//            POSITION pos = m_TargetPorts->GetHeadPosition();
            itPort = m_TargetPorts->begin();
            itPort_end = m_TargetPorts->end();
            while (itPort != itPort_end)
            {
                CPort *port = (*itPort);
                itPort ++;
                if (port->IsConnected() && port->GetResult())
                {
                    LPVOID_PARM lpVoid = new VOID_PARM;
                    lpVoid->lpVoid1 = this;
                    lpVoid->lpVoid2 = port;
                    pthread_create(&pthWriteThreads[nCount],NULL,CleanDiskThreadProc,lpVoid);
//                    hWriteThreads[nCount] = CreateThread(NULL,0,CleanDiskThreadProc,lpVoid,0,&dwWriteId);

//                    CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - CreateCleanDiskThread,ThreadId=0x%04X,HANDLE=0x%04X")
//                        ,port->GetPortName(),port->GetDiskNum(),dwWriteId,hWriteThreads[nCount]);

                    nCount++;
                }
            }

            //等待线程结束
//            WaitForMultipleObjects(nCount,hWriteThreads,TRUE,INFINITE);
            int *val=0;
            for (UINT i = 0; i < nCount;i++)
            {
                pthread_join(pthWriteThreads[i],(void **)&val);
                dwErrorCode = *val;
                bResult |= dwErrorCode;
            }
            delete []pthWriteThreads;

            if (!bResult)
            {
                return FALSE;
            }

            //是写过程中比较还是写之后比较
            if (m_bCompareClean && m_CompareCleanSeq == CompareCleanSeq_After)
            {
                snprintf(function,255,"COMPARE CLEAN %d/%d",times+1,m_nCleanTimes);
//                ::SendMessage(m_hWnd,WM_UPDATE_FUNCTION,(WPARAM)function,0);

//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Disk Copy - COMPARE CLEAN %d/%d"),times+1,m_nCleanTimes);

                nCurrentTargetCount = GetCurrentTargetCount();
//                HANDLE *hCompareCleanThreads = new HANDLE[nCurrentTargetCount];
                pthread_t *pthCompareCleanThreads = new pthread_t[nCurrentTargetCount];
                nCount = 0;
//                pos = m_TargetPorts->GetHeadPosition();
                itPort = m_TargetPorts->begin();
                itPort_end = m_TargetPorts->end();
                while (itPort != itPort_end)
                {
                    CPort *port = (*itPort);
                    itPort ++;
                    if (port->IsConnected() && port->GetResult())
                    {
                        LPVOID_PARM lpVoid = new VOID_PARM;
                        lpVoid->lpVoid1 = this;
                        lpVoid->lpVoid2 = port;
                        pthread_create(&pthCompareCleanThreads[nCount],NULL,CompareCleanThreadProc,lpVoid);
//                        hCompareCleanThreads[nCount] = CreateThread(NULL,0,CompareCleanThreadProc,lpVoid,0,&dwWriteId);

//                        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - CreateCompareCleanThread,ThreadId=0x%04X,HANDLE=0x%04X")
//                            ,port->GetPortName(),port->GetDiskNum(),dwWriteId,hWriteThreads[nCount]);

                        nCount++;
                    }
                }

                //等待线程结束
//                WaitForMultipleObjects(nCount,hCompareCleanThreads,TRUE,INFINITE);
                bResult = FALSE;
                for (UINT i = 0; i < nCount;i++)
                {
                    pthread_join(pthCompareCleanThreads[i],(void **)&val);
                    dwErrorCode = *val;
                    bResult |= dwErrorCode;
                }
                delete []pthCompareCleanThreads;

                if (!bResult)
                {
                    return FALSE;
                }
            }
        }

        ZeroMemory(function,255);
        strncpy(function,"IMAGE COPY",sizeof("IMAGE COPY"));

//        ::SendMessage(m_hWnd,WM_UPDATE_FUNCTION,(WPARAM)function,0);

        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Clean disk completed"));

    }

    // 开始拷贝之前，清空DataQueue中的数据
    ClearDataQueueList();

    SetValidSize(m_ullValidSize);

//    HANDLE hReadThread = CreateThread(NULL,0,ReadImageThreadProc,this,0,&dwReadId);
    pthread_t pthReadThread;
    pthread_create(&pthReadThread,NULL,ReadImageThreadProc,this);

//    CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Copy Image(%s) - CreateReadImageThread,ThreadId=0x%04X,HANDLE=0x%04X")
//        ,m_pMasterPort->GetFileName(),dwReadId,hReadThread);

    // 创建多个解压缩线程
//    HANDLE hUncompressThread = NULL;
    pthread_t pthUncompressThread;

    if (m_bDataCompress)
    {
        pthread_create(&pthUncompressThread,NULL,UnCompressThreadProc,this);
//        hUncompressThread = CreateThread(NULL,0,UnCompressThreadProc,this,0,&dwUncompressId);
//        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Copy Image - CreateUncompressThread,ThreadId=0x%04X,HANDLE=0x%04X")
//            ,dwUncompressId,hUncompressThread);
    }

    UINT nCount = 0;
    UINT nCurrentTargetCount = GetCurrentTargetCount();
//    HANDLE *hWriteThreads = new HANDLE[nCurrentTargetCount];
    pthread_t *pthWriteThreads = new pthread_t[nCurrentTargetCount];
//    pos = m_TargetPorts->GetHeadPosition();
//    POSITION posQueue = m_DataQueueList.GetHeadPosition();
    m_DataQueue.Clear();
    itPort = m_TargetPorts->begin();
    itPort_end = m_TargetPorts->end();
    while (itPort != itPort_end)
    {
        CPort *port = (*itPort);
        itPort++;
        CDataQueue *dataQueue = &m_DataQueue;

        if (port->IsConnected() && port->GetResult())
        {
            LPVOID_PARM lpVoid = new VOID_PARM;
            lpVoid->lpVoid1 = this;
            lpVoid->lpVoid2 = port;
            lpVoid->lpVoid3 = dataQueue;
            pthread_create(&pthWriteThreads[nCount],NULL,WriteDiskThreadProc,lpVoid);
//            hWriteThreads[nCount] = CreateThread(NULL,0,WriteDiskThreadProc,lpVoid,0,&dwWriteId);

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - CreateWriteDiskThread,ThreadId=0x%04X,HANDLE=0x%04X")
//                ,port->GetPortName(),port->GetDiskNum(),dwWriteId,hWriteThreads[nCount]);

            nCount++;
        }
    }

    // 等待写磁盘线程结束
//    WaitForMultipleObjects(nCount,hWriteThreads,TRUE,INFINITE);
    int *val;
    for (UINT i = 0; i < nCount;i++)
    {

        pthread_join(pthWriteThreads[i],(void **)&val);
        dwErrorCode = *val;
        bResult |= dwErrorCode;

    }
    delete []pthWriteThreads;

    if (m_bDataCompress)
    {
        // 等待解压缩线程结束
//        WaitForSingleObject(hUncompressThread,INFINITE);
        pthread_join(pthUncompressThread,NULL);
//        CloseHandle(hUncompressThread);
    }


    //等待读磁盘线程结束

    pthread_join(pthReadThread,(void **)&val);
    dwErrorCode = *val;
    bResult &= dwErrorCode;
//    CloseHandle(hReadThread);

    if (bResult && m_bCompare)
    {
        m_bVerify = TRUE;
        char function[255] = "VERIFY";
//        ::SendMessage(m_hWnd,WM_UPDATE_FUNCTION,(WPARAM)function,0);

        nCurrentTargetCount = GetCurrentTargetCount();
//        HANDLE *hVerifyThreads = new HANDLE[nCurrentTargetCount];
        pthread_t *pthVerifyThreads = new pthread_t[nCurrentTargetCount];

        if (m_CompareMethod == CompareMethod_Byte)
        {
            m_bComputeHash = FALSE;
            m_bServerFirst = FALSE;

            // 开始拷贝之前，清空DataQueue中的数据
            ClearDataQueueList();
            pthread_create(&pthReadThread,NULL,ReadImageThreadProc,this);
//            hReadThread = CreateThread(NULL,0,ReadImageThreadProc,this,0,&dwReadId);

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Verfiy Image(%s) - CreateReadImageThread,ThreadId=0x%04X,HANDLE=0x%04X")
//                ,m_pMasterPort->GetFileName(),dwReadId,hReadThread);

            if (m_bDataCompress)
            {
                // 创建多个解压缩线程
                pthread_create(&pthUncompressThread,NULL,UnCompressThreadProc,this);
//                hUncompressThread = CreateThread(NULL,0,UnCompressThreadProc,this,0,&dwUncompressId);
//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Verify Image - CreateUncompressThread,ThreadId=0x%04X,HANDLE=0x%04X")
//                    ,dwUncompressId,hUncompressThread);
            }

            nCount = 0;
            itPort = m_TargetPorts->begin();
            itPort_end = m_TargetPorts->end();

            while (itPort != itPort_end)
            {
                CPort *port = (*itPort);
                itPort++;
                CDataQueue *dataQueue = &m_DataQueue;
//				dataQueue->Clear();
                if (port->IsConnected() && port->GetResult())
                {
                    LPVOID_PARM lpVoid = new VOID_PARM;
                    lpVoid->lpVoid1 = this;
                    lpVoid->lpVoid2 = port;
                    lpVoid->lpVoid3 = dataQueue;

                    pthread_create(&pthVerifyThreads[nCount],NULL,VerifySectorThreadProc,lpVoid);
//                    hVerifyThreads[nCount] = CreateThread(NULL,0,VerifySectorThreadProc,lpVoid,0,&dwVerifyId);

//                    CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - CreateVerifyDiskSectorThread,ThreadId=0x%04X,HANDLE=0x%04X")
//                        ,port->GetPortName(),port->GetDiskNum(),dwVerifyId,hVerifyThreads[nCount]);

                    nCount++;
                }
            }

            //等待校验线程结束
//            WaitForMultipleObjects(nCount,hVerifyThreads,TRUE,INFINITE);

            bResult = FALSE;
            for (UINT i = 0; i < nCount;i++)
            {
//                GetExitCodeThread(hVerifyThreads[i],&dwErrorCode);
                pthread_join(pthVerifyThreads[i],(void **)&val);
                dwErrorCode = *val;
                bResult |= dwErrorCode;
//                CloseHandle(hVerifyThreads[i]);
            }
            delete []pthVerifyThreads;

            if (m_bDataCompress)
            {
                // 等待解压缩线程结束
                pthread_join(pthUncompressThread,NULL);
//                WaitForSingleObject(hUncompressThread,INFINITE);
//                CloseHandle(hUncompressThread);
            }

            //等待读磁盘线程结束
//            WaitForSingleObject(hReadThread,INFINITE);
            pthread_join(pthReadThread,(void **)&val);
            dwErrorCode = *val;
//            GetExitCodeThread(hReadThread,&dwErrorCode);
            bResult &= dwErrorCode;
//            CloseHandle(hReadThread);

        }
        else
        {
            nCount = 0;
//            pos = m_TargetPorts->GetHeadPosition();
            itPort = m_TargetPorts->begin();
            itPort_end = m_TargetPorts->end();
            while (itPort != itPort_end)
            {
                CPort *port = (*itPort);
                itPort++;
                if (port->IsConnected() && port->GetResult())
                {
                    CHashMethod *pHashMethod;

                    port->SetHashMethod(hashMethod);

                    switch (port->GetHashMethod())
                    {
                    case HashMethod_CHECKSUM32:
                        pHashMethod = new CChecksum32();
                        break;

                    case HashMethod_CRC32:
                        pHashMethod = new CCRC32();
                        break;

                    case HashMethod_MD5:
                        pHashMethod = new MD5();
                        break;
                    }

                    LPVOID_PARM lpVoid = new VOID_PARM;
                    lpVoid->lpVoid1 = this;
                    lpVoid->lpVoid2 = port;
                    lpVoid->lpVoid3 = pHashMethod;

                    pthread_create(&pthVerifyThreads[nCount],NULL,VerifyThreadProc,lpVoid);
//                    hVerifyThreads[nCount] = CreateThread(NULL,0,VerifyThreadProc,lpVoid,0,&dwVerifyId);

//                    CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - CreateVerifyDiskThread,ThreadId=0x%04X,HANDLE=0x%04X")
//                        ,port->GetPortName(),port->GetDiskNum(),dwVerifyId,hVerifyThreads[nCount]);

                    nCount++;
                }
            }

            // 等待校验线程结束
//            WaitForMultipleObjects(nCount,hVerifyThreads,TRUE,INFINITE);

            bResult = FALSE;
            for (UINT i = 0; i < nCount;i++)
            {
//                GetExitCodeThread(hVerifyThreads[i],&dwErrorCode);
                pthread_join(pthVerifyThreads[i],(void **)&val);
                dwErrorCode = *val;
                bResult |= dwErrorCode;
//                CloseHandle(hVerifyThreads[i]);
            }
            delete []pthVerifyThreads;
        }

        if (!bResult)
        {
            // 任意取一个错误
            ErrorType errType = ErrorType_System;
            DWORD dwErrorCode = 0;

//            pos = m_TargetPorts->GetHeadPosition();
            itPort = m_TargetPorts->begin();
            itPort_end = m_TargetPorts->end();
            while (itPort != itPort_end)
            {
                CPort *port = (*itPort);
                itPort ++;
                if (port->IsConnected() && !port->GetResult())
                {
                    errType = port->GetErrorCode(&dwErrorCode);
                    break;
                }
            }

            m_pMasterPort->SetErrorCode(errType,dwErrorCode);
        }
    }

    return bResult;
}

BOOL Disk::OnMakeImage()
{
    // 暂时不考虑文件映像
    if (QUICK_IMAGE == m_nImageType)
    {
//        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Analyze start......"));
        if (!BriefAnalyze())
        {
            return FALSE;
        }

        // 是否有用户指定区域,如果存在直接加载列表后面
//        POSITION pos = m_ListRangeFromTo.GetHeadPosition();
        CListRangeFromTo::const_iterator itRangeFrom = m_ListRangeFromTo.begin();
        CListRangeFromTo::const_iterator itRangeFrom_end = m_ListRangeFromTo.end();
        while (itRangeFrom != itRangeFrom_end)
        {
            RANGE_FROM_TO range = (*itRangeFrom);
            itRangeFrom++;
            CString strList;
            strList.Format(_T("User custom area --- StartSector:%I64d  EndSector:%I64d")
                ,range.ullStartLBA,range.ullEndingLBA);

//            CUtils::WriteLogFile(m_hLogFile,FALSE,strList);

            if (range.ullStartLBA < m_ullSectorNums && range.ullEndingLBA <= m_ullCapacity)
            {
                EFF_DATA effData;
                ZeroMemory(&effData,sizeof(effData));
                effData.ullStartSector = range.ullStartLBA;
                effData.ullSectors = range.ullEndingLBA - range.ullStartLBA;
                effData.wBytesPerSector = BYTES_PER_SECTOR;

                if (effData.ullSectors != 0)
                {
                    m_EffList.push_back(effData);
                }
            }

        }

        m_ullValidSize = GetValidSize();
        SetValidSize(m_ullValidSize);

//        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Analyze end, valid data size=%I64d"),m_ullValidSize);
    }
    else
    {
        EFF_DATA effData;
        effData.ullStartSector = 0;
        effData.ullSectors = m_ullSectorNums;
        effData.wBytesPerSector = (WORD)m_dwBytesPerSector;

        m_EffList.push_back(effData);

        m_ullValidSize = GetValidSize();
        SetValidSize(m_ullValidSize);
    }

    CUtils::WriteLogFile(m_hLogFile,FALSE,_T("Valid Data List:"));
    CString strList;
//    POSITION pos = m_EffList.GetHeadPosition();
    EFF_LIST::const_iterator itEFF = m_EffList.begin();
    EFF_LIST::const_iterator itEFF_end = m_EffList.end();

    while (itEFF != itEFF_end)
    {
        EFF_DATA effData = (*itEFF);
        itEFF++;
        strList.Format(_T("StartSector:%I64d  Sectors:%I64d  BytesPerSector:%d"),effData.ullStartSector,effData.ullSectors,effData.wBytesPerSector);
//        CUtils::WriteLogFile(m_hLogFile,FALSE,strList);
    }

    // 开始拷贝之前，清空DataQueue中的数据
//    ClearDataQueueList();
    m_DataQueue.Clear();
    BOOL bResult = FALSE;
    DWORD /*dwReadId,dwWriteId,dwCompressId,*/dwErrorCode;

    pthread_t pthReadThread;
    pthread_create(&pthReadThread,NULL,ReadDiskThreadProc,this);
//    HANDLE hReadThread = CreateThread(NULL,0,ReadDiskThreadProc,this,0,&dwReadId);

//    CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Image Make(Master) - CreateReadDiskThread,ThreadId=0x%04X,HANDLE=0x%04X")
//        ,dwReadId,hReadThread);

//    HANDLE hCompressThread = NULL;
    pthread_t pthComressThread ;

    if (m_bDataCompress)
    {
        pthread_create(&pthComressThread,NULL,CompressThreadProc,this);
//        hCompressThread = CreateThread(NULL,0,CompressThreadProc,this,0,&dwCompressId);
//        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Compress Image - CreateCompressThread,ThreadId=0x%04X,HANDLE=0x%04X")
//            ,dwCompressId,hCompressThread);
    }

    UINT nCount = 0;
    UINT nCurrentTargetCount = GetCurrentTargetCount();
//    HANDLE *hWriteThreads = new HANDLE[nCurrentTargetCount];
    pthread_t *pthWriteThreads = new pthread_t[nCurrentTargetCount];

//    pos = m_TargetPorts->GetHeadPosition();
//    POSITION posQueue = m_DataQueueList.GetHeadPosition();
    PortList::const_iterator itPort = m_TargetPorts->begin();
    PortList::const_iterator itPort_end = m_TargetPorts->end();
    while(itPort != itPort_end)
    {
        CPort *port = (*itPort);
        itPort ++;
        CDataQueue *dataQueue = &m_DataQueue;
//		dataQueue->Clear();
        if (port->IsConnected() && port->GetResult())
        {
            LPVOID_PARM lpVoid = new VOID_PARM;
            lpVoid->lpVoid1 = this;
            lpVoid->lpVoid2 = port;
            lpVoid->lpVoid3 = dataQueue;

            pthread_create(&pthWriteThreads[nCount],NULL,WriteImageThreadProc,lpVoid);
//            hWriteThreads[nCount] = CreateThread(NULL,0,WriteImageThreadProc,lpVoid,0,&dwWriteId);

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Image Make - CreateWriteImageThread,ThreadId=0x%04X,HANDLE=0x%04X")
//                ,dwWriteId,hWriteThreads[nCount]);
            nCount++;
        }
    }

    //等待写映像线程结束
//    WaitForMultipleObjects(nCount,hWriteThreads,TRUE,INFINITE);

    int *val;
    for (UINT i = 0; i < nCount;i++)
    {
        pthread_join(pthWriteThreads[i],(void **)&val);
        dwErrorCode = *val;
        bResult |= dwErrorCode;
    }
    delete []pthWriteThreads;

    if (m_bDataCompress)
    {
        //等待压缩线程结束
        pthread_join(pthComressThread,NULL);
//        WaitForSingleObject(hCompressThread,INFINITE);
//        CloseHandle(hCompressThread);
    }

    //等待读磁盘线程结束
//    WaitForSingleObject(hReadThread,INFINITE);
    pthread_join(pthReadThread,(void **)&val);
    dwErrorCode = *val;
    bResult &= dwErrorCode;

    return bResult;
}

BOOL Disk::OnCompareDisk()
{
    switch (m_CompareMode)
    {
    case CompareMode_Full:
        {
            EFF_DATA effData;
            effData.ullStartSector = 0;
            effData.ullSectors = m_ullSectorNums;
            effData.wBytesPerSector = (WORD)m_dwBytesPerSector;

            m_EffList.push_back(effData);

            m_ullValidSize = GetValidSize();
            SetValidSize(m_ullValidSize);
        }

        break;

    case CompareMode_Quick:
        {
            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Analyze start......"));
            if (!BriefAnalyze())
            {
                return FALSE;
            }

            m_ullValidSize = GetValidSize();
            SetValidSize(m_ullValidSize);

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Analyze end, valid data size=%I64d"),m_ullValidSize);

        }
        break;
    }

    CUtils::WriteLogFile(m_hLogFile,FALSE,_T("Valid Data List:"));
    CString strList;
//    POSITION pos = m_EffList.GetHeadPosition();
    EFF_LIST::const_iterator itEFF = m_EffList.begin();
    EFF_LIST::const_iterator itEFF_end = m_EffList.end();

    while (itEFF != itEFF_end)
    {
        EFF_DATA effData = (*itEFF);
        itEFF ++;
        strList.Format(_T("StartSector:%I64d  Sectors:%I64d  BytesPerSector:%d"),effData.ullStartSector,effData.ullSectors,effData.wBytesPerSector);
//        CUtils::WriteLogFile(m_hLogFile,FALSE,strList);
    }

    BOOL bResult = FALSE;
    DWORD dwErrorCode;

//    HANDLE hMasterVerifyThread = NULL;
    pthread_t pthMasterVerifyThread;
    UINT nCurrentTargetCount = GetCurrentTargetCount();
//    HANDLE *hVerifyThreads = new HANDLE[nCurrentTargetCount];
    pthread_t *pthVerifyThreads = new pthread_t[nCurrentTargetCount];
    UINT nCount = 0;

    m_bVerify = TRUE;

    if (m_CompareMethod == CompareMethod_Byte)
    {
        m_bComputeHash = FALSE;

        // 开始拷贝之前，清空DataQueue中的数据
//        ClearDataQueueList();
        m_DataQueue.Clear();
        pthread_create(&pthMasterVerifyThread,NULL,ReadDiskThreadProc,this);
//        hMasterVerifyThread = CreateThread(NULL,0,ReadDiskThreadProc,this,0,&dwVerifyId);

//        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Disk Compare(Master) - CreateReadDiskThread,ThreadId=0x%04X,HANDLE=0x%04X")
//            ,dwVerifyId,hMasterVerifyThread);

//        pos = m_TargetPorts->GetHeadPosition();
//        POSITION posQueue = m_DataQueueList.GetHeadPosition();
        PortList::const_iterator itPort = m_TargetPorts->begin();
        PortList::const_iterator itPort_end = m_TargetPorts->end();

        while (itPort != itPort_end)
        {
            CPort *port = (*itPort);
            itPort++;
            CDataQueue *dataQueue = &m_DataQueue;
            if (port->IsConnected() && port->GetResult())
            {
                LPVOID_PARM lpVoid = new VOID_PARM;
                lpVoid->lpVoid1 = this;
                lpVoid->lpVoid2 = port;
                lpVoid->lpVoid3 = dataQueue;

                pthread_create(&pthVerifyThreads[nCount],NULL,VerifySectorThreadProc,lpVoid);
//                hVerifyThreads[nCount] = CreateThread(NULL,0,VerifySectorThreadProc,lpVoid,0,&dwVerifyId);

//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - CreateVerifyDiskSectorThread,ThreadId=0x%04X,HANDLE=0x%04X")
//                    ,port->GetPortName(),port->GetDiskNum(),dwVerifyId,hVerifyThreads[nCount]);

                nCount++;
            }
        }
    }
    else
    {
        //母盘读线程
        LPVOID_PARM lpMasterVoid = new VOID_PARM;
        lpMasterVoid->lpVoid1 = this;
        lpMasterVoid->lpVoid2 = m_pMasterPort;
        lpMasterVoid->lpVoid3 = m_pMasterHashMethod;

        pthread_create(&pthMasterVerifyThread,NULL,VerifyThreadProc,lpMasterVoid);
//        hMasterVerifyThread = CreateThread(NULL,0,VerifyThreadProc,lpMasterVoid,0,&dwVerifyId);

//        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - CreateVerifyDiskThread,ThreadId=0x%04X,HANDLE=0x%04X")
//            ,m_MasterPort->GetPortName(),m_MasterPort->GetDiskNum(),dwVerifyId,hMasterVerifyThread);

        //子盘读线程
//        pos = m_TargetPorts->GetHeadPosition();
        PortList::const_iterator itPort = m_TargetPorts->begin();
        PortList::const_iterator itPort_end = m_TargetPorts->end();

        while (itPort != itPort_end)
        {
            CPort *port = (*itPort);
            itPort++;
            if (port->IsConnected() && port->GetResult())
            {
                CHashMethod *pHashMethod;

                switch (port->GetHashMethod())
                {
                case HashMethod_CHECKSUM32:
                    pHashMethod = new CChecksum32();
                    break;

                case HashMethod_CRC32:
                    pHashMethod = new CCRC32();
                    break;

                case HashMethod_MD5:
                    pHashMethod = new MD5();
                    break;
                }

                LPVOID_PARM lpVoid = new VOID_PARM;
                lpVoid->lpVoid1 = this;
                lpVoid->lpVoid2 = port;
                lpVoid->lpVoid3 = pHashMethod;

                pthread_create(&pthVerifyThreads[nCount],NULL,VerifyThreadProc,lpVoid);
//                hVerifyThreads[nCount] = CreateThread(NULL,0,VerifyThreadProc,lpVoid,0,&dwVerifyId);

//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - CreateVerifyDiskThread,ThreadId=0x%04X,HANDLE=0x%04X")
//                    ,port->GetPortName(),port->GetDiskNum(),dwVerifyId,hVerifyThreads[nCount]);

                nCount++;
            }
        }
    }

    //等待校验线程结束
//    WaitForMultipleObjects(nCount,hVerifyThreads,TRUE,INFINITE);

    int *val;
    for (UINT i = 0; i < nCount;i++)
    {
//        GetExitCodeThread(hVerifyThreads[i],&dwErrorCode);
        pthread_join(pthVerifyThreads[i],(void **)&val);
        dwErrorCode = *val;
        bResult |= dwErrorCode;
//        CloseHandle(hVerifyThreads[i]);
    }
    delete []pthVerifyThreads;

    if (!bResult)
    {
        // 任意取一个错误
        ErrorType errType = ErrorType_System;
        DWORD dwErrorCode = 0;

//        pos = m_TargetPorts->GetHeadPosition();
        PortList::const_iterator itPort = m_TargetPorts->begin();
        PortList::const_iterator itPort_end = m_TargetPorts->end();
        while (itPort != itPort_end)
        {
            CPort *port = (*itPort);
            itPort++;
            if (port->IsConnected() && !port->GetResult())
            {
                errType = port->GetErrorCode(&dwErrorCode);
                break;
            }
        }

        m_pMasterPort->SetErrorCode(errType,dwErrorCode);
    }

    //等待母盘读线程结束
    pthread_join(pthMasterVerifyThread,(void **)&val);
    dwErrorCode = *val;
//    WaitForSingleObject(hMasterVerifyThread,INFINITE);
//    GetExitCodeThread(hMasterVerifyThread,&dwErrorCode);
    bResult &= dwErrorCode;
//    CloseHandle(hMasterVerifyThread);

    return bResult;
}

BOOL Disk::OnCleanDisk()
{
    BOOL bResult = FALSE;
    DWORD dwErrorCode;

    //子盘写线程
    if (m_CleanMode == CleanMode_Safe)
    {
        m_CleanMode = CleanMode_Full;

        m_nCleanTimes = 7;
        m_pCleanValues = new int[7];
        m_pCleanValues[0] = 0;
        m_pCleanValues[1] = 0xFF;
        m_pCleanValues[2] = RANDOM_VALUE;
        m_pCleanValues[3] = 0x96;
        m_pCleanValues[4] = 0;
        m_pCleanValues[5] = 0xFF;
        m_pCleanValues[6] = RANDOM_VALUE;

        char function[255] = {0};
        m_bEnd = FALSE;
        for (int times = 0; times < m_nCleanTimes;times++)
        {
            snprintf(function,255,"DISK CLEAN %d/%d",times+1,m_nCleanTimes);
//            ::SendMessage(m_hWnd,WM_UPDATE_FUNCTION,(WPARAM)function,0);

            if (times == m_nCleanTimes - 1)
            {
                m_bEnd = TRUE;
            }

            m_nFillValue = m_pCleanValues[times];

            DWORD dwLen = /*BYTES_PER_SECTOR*/m_dwBytesPerSector * m_nBlockSectors;

            if (m_pFillBytes != NULL)
            {
                delete []m_pFillBytes;
            }

            m_pFillBytes = new BYTE[dwLen];

            if (m_nFillValue != RANDOM_VALUE)
            {
                memset(m_pFillBytes,m_nFillValue,dwLen);
            }
            else
            {
                srand((unsigned int)time(NULL));

                for (DWORD i = 0;i < dwLen;i++)
                {
                    m_pFillBytes[i] = (BYTE)rand();
                }
            }

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("DISK CLEAN %d/%d,fill value %d"),times+1,m_nCleanTimes,m_nFillValue);

            UINT nCount = 0;
            UINT nCurrentTargetCount = GetCurrentTargetCount();
//            HANDLE *hWriteThreads = new HANDLE[nCurrentTargetCount];
            pthread_t *pthWriteThreads = new pthread_t[nCurrentTargetCount];
//            POSITION pos = m_TargetPorts->GetHeadPosition();
            PortList::const_iterator itPort = m_TargetPorts->begin();
            PortList::const_iterator itPort_end = m_TargetPorts->end();
            while (itPort != itPort_end)
            {
                CPort *port = (*itPort);
                if (port->IsConnected() && port->GetResult())
                {
                    LPVOID_PARM lpVoid = new VOID_PARM;
                    lpVoid->lpVoid1 = this;
                    lpVoid->lpVoid2 = port;

                    pthread_create(&pthWriteThreads[nCount],NULL,CleanDiskThreadProc,lpVoid);
//                    hWriteThreads[nCount] = CreateThread(NULL,0,CleanDiskThreadProc,lpVoid,0,&dwWriteId);

//                    CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - CreateCleanDiskThread,ThreadId=0x%04X,HANDLE=0x%04X")
//                        ,port->GetPortName(),port->GetDiskNum(),dwWriteId,hWriteThreads[nCount]);

                    nCount++;
                }
            }

            //等待线程结束
//            WaitForMultipleObjects(nCount,hWriteThreads,TRUE,INFINITE);
            int *val;
            for (UINT i = 0; i < nCount;i++)
            {
                pthread_join(pthWriteThreads[i],(void **)&val);
                dwErrorCode = *val;
//                GetExitCodeThread(hWriteThreads[i],&dwErrorCode);
                bResult |= dwErrorCode;
//                CloseHandle(hWriteThreads[i]);
//                hWriteThreads[i] = NULL;
            }
            delete []pthWriteThreads;

            if (!bResult)
            {
                return FALSE;
            }

            //是写过程中比较还是写之后比较
            if (m_bCompareClean && m_CompareCleanSeq == CompareCleanSeq_After)
            {
                snprintf(function,255,"COMPARE CLEAN %d/%d",times+1,m_nCleanTimes);
//                ::SendMessage(m_hWnd,WM_UPDATE_FUNCTION,(WPARAM)function,0);

//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Disk Clean - COMPARE CLEAN %d/%d"),times+1,m_nCleanTimes);

                nCurrentTargetCount = GetCurrentTargetCount();
//                HANDLE *hCompareCleanThreads = new HANDLE[nCurrentTargetCount];
                pthread_t *pthCompareCleanThreads = new pthread_t[nCurrentTargetCount];
                nCount = 0;
//                pos = m_TargetPorts->GetHeadPosition();
                itPort = m_TargetPorts->begin();
                itPort_end = m_TargetPorts->end();

                while (itPort != itPort_end)
                {
                    CPort *port = (*itPort);
                    itPort++;
                    if (port->IsConnected() && port->GetResult())
                    {
                        LPVOID_PARM lpVoid = new VOID_PARM;
                        lpVoid->lpVoid1 = this;
                        lpVoid->lpVoid2 = port;

                        pthread_create(&pthCompareCleanThreads[nCount],NULL,CompareCleanThreadProc,lpVoid);
//                        hCompareCleanThreads[nCount] = CreateThread(NULL,0,CompareCleanThreadProc,lpVoid,0,&dwWriteId);

//                        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - CreateCompareCleanThread,ThreadId=0x%04X,HANDLE=0x%04X")
//                            ,port->GetPortName(),port->GetDiskNum(),dwWriteId,hWriteThreads[nCount]);

                        nCount++;
                    }
                }

                //等待线程结束
//                WaitForMultipleObjects(nCount,hCompareCleanThreads,TRUE,INFINITE);

                bResult = FALSE;
                for (UINT i = 0; i < nCount;i++)
                {
                    pthread_join(pthCompareCleanThreads[i],(void **)&val);
//                    GetExitCodeThread(hCompareCleanThreads[i],&dwErrorCode);
                    dwErrorCode = *val;
                    bResult |= dwErrorCode;
//                    CloseHandle(hCompareCleanThreads[i]);
//                    hCompareCleanThreads[i] = NULL;
                }
                delete []pthCompareCleanThreads;

                if (!bResult)
                {
                    return FALSE;
                }
            }
        }
    }
    else
    {

        if (m_bCompareClean && m_CompareCleanSeq == CompareCleanSeq_After)
        {
            m_bEnd = FALSE;
        }

        DWORD dwLen = BYTES_PER_SECTOR * m_nBlockSectors;

        if (m_pFillBytes != NULL)
        {
            delete []m_pFillBytes;
        }

        m_pFillBytes = new BYTE[dwLen];

        if (m_nFillValue != RANDOM_VALUE)
        {
            memset(m_pFillBytes,m_nFillValue,dwLen);
        }
        else
        {
            srand((unsigned int)time(NULL));

            for (DWORD i = 0;i < dwLen;i++)
            {
                m_pFillBytes[i] = (BYTE)rand();
            }
        }

        UINT nCount = 0;
        UINT nCurrentTargetCount = GetCurrentTargetCount();
//        HANDLE *hWriteThreads = new HANDLE[nCurrentTargetCount];
        pthread_t *pthWriteThreads = new pthread_t[nCurrentTargetCount];
//        POSITION pos = m_TargetPorts->GetHeadPosition();
        PortList::const_iterator itPort = m_TargetPorts->begin();
        PortList::const_iterator itPort_end = m_TargetPorts->end();
        while (itPort != itPort_end)
        {
            CPort *port = (*itPort);
            itPort++;
            if (port->IsConnected() && port->GetResult())
            {
                LPVOID_PARM lpVoid = new VOID_PARM;
                lpVoid->lpVoid1 = this;
                lpVoid->lpVoid2 = port;

                pthread_create(&pthWriteThreads[nCount],NULL,CleanDiskThreadProc,lpVoid);
//                hWriteThreads[nCount] = CreateThread(NULL,0,CleanDiskThreadProc,lpVoid,0,&dwWriteId);

//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - CreateCleanDiskThread,ThreadId=0x%04X,HANDLE=0x%04X")
//                    ,port->GetPortName(),port->GetDiskNum(),dwWriteId,hWriteThreads[nCount]);

                nCount++;
            }
        }

        //等待线程结束
//        WaitForMultipleObjects(nCount,hWriteThreads,TRUE,INFINITE);

        int *val;
        bResult = FALSE;
        for (UINT i = 0; i < nCount;i++)
        {
            pthread_join(pthWriteThreads[i],(void **)&val);
//            GetExitCodeThread(hWriteThreads[i],&dwErrorCode);
            dwErrorCode = *val;
            bResult |= dwErrorCode;
//            CloseHandle(hWriteThreads[i]);
//            hWriteThreads[i] = NULL;
        }
        delete []pthWriteThreads;

        if (!bResult)
        {
            return FALSE;
        }

        //是写过程中比较还是写之后比较
        if (m_bCompareClean && m_CompareCleanSeq == CompareCleanSeq_After)
        {
            m_bEnd = TRUE;

            char function[255] = {NULL};
            snprintf(function,255,"COMPARE CLEAN");
//            ::SendMessage(m_hWnd,WM_UPDATE_FUNCTION,(WPARAM)function,0);

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Disk Clean - COMPARE CLEAN"));

            nCurrentTargetCount = GetCurrentTargetCount();
//            HANDLE *hCompareCleanThreads = new HANDLE[nCurrentTargetCount];
            pthread_t *pthCompareCleanThreads = new pthread_t[nCurrentTargetCount];
            nCount = 0;
//            pos = m_TargetPorts->GetHeadPosition();
            itPort = m_TargetPorts->begin();
            itPort_end = m_TargetPorts->end();

            while (itPort != itPort_end)
            {
                CPort *port = (*itPort);
                itPort ++;
                if (port->IsConnected() && port->GetResult())
                {
                    LPVOID_PARM lpVoid = new VOID_PARM;
                    lpVoid->lpVoid1 = this;
                    lpVoid->lpVoid2 = port;


                    pthread_create(&pthCompareCleanThreads[nCount],NULL,CompareCleanThreadProc,lpVoid);
//                    hCompareCleanThreads[nCount] = CreateThread(NULL,0,CompareCleanThreadProc,lpVoid,0,&dwWriteId);

//                    CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - CreateCompareCleanThread,ThreadId=0x%04X,HANDLE=0x%04X")
//                        ,port->GetPortName(),port->GetDiskNum(),dwWriteId,hWriteThreads[nCount]);

                    nCount++;
                }
            }

            //等待线程结束
//            WaitForMultipleObjects(nCount,hCompareCleanThreads,TRUE,INFINITE);

            bResult = FALSE;
            for (UINT i = 0; i < nCount;i++)
            {
                pthread_join(pthCompareCleanThreads[i],(void **)&val);
//                GetExitCodeThread(hCompareCleanThreads[i],&dwErrorCode);
                dwErrorCode = *val;
                bResult |= dwErrorCode;
//                CloseHandle(hCompareCleanThreads[i]);
//                hCompareCleanThreads[i] = NULL;
            }
            delete []pthCompareCleanThreads;

            if (!bResult)
            {
                return FALSE;
            }
        }
    }

    return bResult;
}

BOOL Disk::OnCopyFiles()
{
    return FALSE;
}

BOOL Disk::OnFormatDisk()
{
    BOOL bResult = FALSE;
    DWORD dwErrorCode;

    //子盘写线程
    UINT nCurrentTargetCount = GetCurrentTargetCount();
//    HANDLE *hWriteThreads = new HANDLE[nCurrentTargetCount];
    pthread_t *pthWriteThreads = new pthread_t[nCurrentTargetCount];
    UINT nCount = 0;
//    POSITION pos = m_TargetPorts->GetHeadPosition();
    PortList::const_iterator itPort = m_TargetPorts->begin();
    PortList::const_iterator itPort_end = m_TargetPorts->end();
    while (itPort != itPort_end)
    {
        CPort *port = (*itPort);
        itPort ++;
        if (port->IsConnected() && port->GetResult())
        {
            LPVOID_PARM lpVoid = new VOID_PARM;
            lpVoid->lpVoid1 = this;
            lpVoid->lpVoid2 = port;

            pthread_create(&pthWriteThreads[nCount],NULL,FormatDiskThreadProc,lpVoid);
//            hWriteThreads[nCount] = CreateThread(NULL,0,FormatDiskThreadProc,lpVoid,0,&dwWriteId);

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - CreateFormatDiskThread,ThreadId=0x%04X,HANDLE=0x%04X")
//                ,port->GetPortName(),port->GetDiskNum(),dwWriteId,hWriteThreads[nCount]);
            nCount++;
        }
    }

    //等待线程结束
//    WaitForMultipleObjects(nCount,hWriteThreads,TRUE,INFINITE);
    int *val;
    for (UINT i = 0; i < nCount;i++)
    {
//        GetExitCodeThread(hWriteThreads[i],&dwErrorCode);
        pthread_join(pthWriteThreads[i],(void **)&val);
        dwErrorCode = *val;
        bResult |= dwErrorCode;
//        CloseHandle(hWriteThreads[i]);
//        hWriteThreads[i] = NULL;
    }
    delete []pthWriteThreads;

    return bResult;
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

    m_pMasterPort->Active();

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

    while (itEff != itEff_end && !*m_lpCancel && bResult && m_pMasterPort->GetPortState() == PortState_Active)
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

//            QueryPerformanceCounter(&t1);
            if (!ReadSectors(m_hMaster,ullStartSectors,dwSectors,effData.wBytesPerSector,pByte,&dwErrorCode))
            {
                bResult = FALSE;

//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d,Speed=%.2f,system errorcode=%ld,%s")
//                    ,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum(),m_pMasterPort->GetRealSpeed(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
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

//                m_pMasterPort->AppendUsedWaitTimeS(dbTimeWait);
//                m_pMasterPort->AppendUsedNoWaitTimeS(dbTimeNoWait);
                m_pMasterPort->AppendCompleteSize(dwLen);


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
//            ,m_pMasterPort->GetPortName(),m_pMasterPort->GetDiskNum(),m_pMasterPort->GetRealSpeed(),dwErrorCode);
    }

    // 先设置为停止状态
    // 先设置为停止状态
    if (bResult)
    {
        m_pMasterPort->SetPortState(PortState_Stop);
    }
    else
    {
        m_pMasterPort->SetResult(FALSE);
        m_pMasterPort->SetPortState(PortState_Fail);
        m_pMasterPort->SetErrorCode(errType,dwErrorCode);
    }

    // 所有数据都拷贝完
    while (!m_bCompressComplete)
    {
        Sleep(100);
    }

    if (!m_pMasterPort->GetResult())
    {
        bResult = FALSE;
        errType = m_pMasterPort->GetErrorCode(&dwErrorCode);
    }

    m_pMasterPort->SetResult(bResult);
//    m_pMasterPort->SetEndTime(CTime::GetCurrentTime());

    if (bResult)
    {
        if (!m_bCompare || m_CompareMethod == CompareMethod_Hash)
        {
            m_pMasterPort->SetPortState(PortState_Pass);
        }

        if (m_bVerify)
        {
            m_pMasterPort->SetPortState(PortState_Pass);
        }

        if (m_bComputeHash)
        {
            m_pMasterPort->SetHash(m_pMasterHashMethod->digest(),m_pMasterHashMethod->getHashLength());

            for (int i = 0; i < m_pMasterHashMethod->getHashLength();i++)
            {
                CString strHash;
                strHash.Format(_T("%02X"),m_pMasterHashMethod->digest()[i]);
                m_strMasterHash += strHash;
            }

            CString strHashMethod(m_pMasterHashMethod->getHashMetod());
//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - %s,HashValue=%s")
//                ,m_pMasterPort->GetPortName().c_str(),m_pMasterPort->GetDiskNum(),strHashMethod.c_str(),m_strMasterHash.c_str());

        }
    }
    else
    {
        m_pMasterPort->SetPortState(PortState_Fail);
        m_pMasterPort->SetErrorCode(errType,dwErrorCode);
    }

    return bResult;
}

BOOL Disk::ReadImage()
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
            struct timeval tval;
            gettimeofday(&tval,NULL);

            port->SetEndTime(tval);
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
            struct timeval tval;
            gettimeofday(&tval,NULL);
            port->SetEndTime(tval);
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
        EFF_LIST::const_iterator it = m_EffList.begin();
        EFF_LIST::const_iterator it_end = m_EffList.end();

        while (it != it_end)
        {
            EFF_DATA effData = (*it);
            it++;

            if ( (effData.ullStartSector + effData.ullSectors) * effData.wBytesPerSector > port->GetTotalSize() )
            {
                struct timeval tval;
                gettimeofday(&tval,NULL);
                port->SetEndTime(tval);
                port->SetResult(FALSE);
                port->SetErrorCode(ErrorType_Custom,CustomError_Target_Small);
                port->SetPortState(PortState_Fail);
//                CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - Stop copy,custom errorcode=0x%X,target is small")
//                    ,port->GetPortName(),port->GetDiskNum(),CustomError_Target_Small);

                return FALSE;
            }
        }
    }

    HANDLE hDisk = GetHandleOnPhysicalDrive(port->GetDevicePath(),O_RDWR,&dwErrorCode);

    if (hDisk < 0)
    {
        struct timeval tval;
        gettimeofday(&tval,NULL);

        port->SetEndTime(tval);
        port->SetResult(FALSE);
        port->SetErrorCode(ErrorType_System,dwErrorCode);
        port->SetPortState(PortState_Fail);
//        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d - OpenDisk error,system errorcode=%ld,%s")
//            ,port->GetPortName(),port->GetDiskNum(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));

        return FALSE;
    }

//    SetDiskAtrribute(hDisk,FALSE,TRUE,&dwErrorCode);

    // 计算精确速度
    struct timeval t0,t1,t2;
    double dbTimeNoWait = 0.0,dbTimeWait = 0.0;

    DWORD dwBytesPerSector = port->GetBytesPerSector();

//    QueryPerformanceFrequency(&freq);

    // 是否需要先执行全盘擦除
    if (!m_bCleanDiskFirst)
    {
        QuickClean(hDisk,port,&dwErrorCode);
    }

//	BOOL bWriteLog = TRUE;

    while (!*m_lpCancel && m_pMasterPort->GetResult() && port->GetResult() && bResult && !port->IsKickOff())
    {
//        QueryPerformanceCounter(&t0);
        gettimeofday(&t0,NULL);

        while(pDataQueue->GetCount() <= 0 && !*m_lpCancel && m_pMasterPort->GetResult()
            && (m_pMasterPort->GetPortState() == PortState_Active || !m_bCompressComplete)
            && port->GetResult() && !port->IsKickOff())
        {
            //SwitchToThread();
            Sleep(5);
        }


        if (!m_pMasterPort->GetResult())
        {
            errType = m_pMasterPort->GetErrorCode(&dwErrorCode);
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

        if (pDataQueue->GetCount() <= 0 && m_pMasterPort->GetPortState() != PortState_Active && m_bCompressComplete)
        {
            dwErrorCode = 0;
            bResult = TRUE;
            break;
        }


        PDATA_INFO dataInfo = pDataQueue->GetHeadRemove();

        if (dataInfo == NULL)
        {
            /*
            if (pDataQueue->GetCount() <= 0 && m_pMasterPort->GetPortState() != PortState_Active && m_bCompressComplete)
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



//        QueryPerformanceCounter(&t1);
        gettimeofday(&t1,NULL);
        if (!WriteSectors(hDisk,ullStartSectors,dwSectors,dwBytesPerSector,dataInfo->pData,&dwErrorCode))
        {
            bResult = FALSE;

            delete []dataInfo->pData;
            delete dataInfo;

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d,Speed=%.2f,system errorcode=%ld,%s")
//                ,port->GetPortName(),port->GetDiskNum(),port->GetRealSpeed(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
            break;
        }
        else
        {
//            QueryPerformanceCounter(&t2);
            gettimeofday(&t2,NULL);
//            dbTimeWait = (t2.tv_sec - t0.tv_sec)+(t2.tv_usec-t0.tv_usec)/1000000.0;
//            dbTimeNoWait = (t2.tv_sec - t0.tv_sec)+(t2.tv_usec-t0.tv_usec)/1000000.0;
//            dbTimeWait = (double)(t2.QuadPart - t0.QuadPart) / (double)freq.QuadPart;

//            dbTimeNoWait = (double)(t2.QuadPart - t1.QuadPart) / (double)freq.QuadPart;

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

//    CloseHandle(hDisk);

    if (*m_lpCancel)
    {
        bResult = FALSE;
        dwErrorCode = CustomError_Cancel;
        errType = ErrorType_Custom;

//        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port=%s,Disk %d,Speed=%.2f,custom errorcode=0x%X,user cancelled.")
//            ,port->GetPortName(),port->GetDiskNum(),port->GetRealSpeed(),dwErrorCode);
    }

    if (port->IsKickOff())
    {
        bResult = FALSE;
        dwErrorCode = CustomError_Speed_Too_Slow;
        errType = ErrorType_Custom;

//        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Port %s,Disk %d,Speed=%.2f,custom errorcode=0x%X,speed too slow.")
//            ,port->GetPortName(),port->GetDiskNum(),port->GetRealSpeed(),CustomError_Speed_Too_Slow);
    }

    if (!m_pMasterPort->GetResult())
    {
        bResult = FALSE;
        errType = m_pMasterPort->GetErrorCode(&dwErrorCode);
    }

    if (!port->GetResult())
    {
        errType = port->GetErrorCode(&dwErrorCode);
        bResult = FALSE;
    }


    port->SetResult(bResult);
//    port->SetEndTime(CTime::GetCurrentTime());

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
    USE_PARAM(port);
    USE_PARAM(pHashMethod);

    return FALSE;
}

BOOL Disk::VerifyDisk(CPort *port, CDataQueue *pDataQueue)
{
    USE_PARAM(port);
    USE_PARAM(pDataQueue);
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
    while (!*m_lpCancel && m_pMasterPort->GetResult())
    {
//        QueryPerformanceCounter(&t0);
        while (m_CompressQueue.GetCount() <= 0
            && !*m_lpCancel
            && m_pMasterPort->GetResult()
            && m_pMasterPort->GetPortState() == PortState_Active)
        {
            Sleep(5);
        }

        if (!m_pMasterPort->GetResult() || *m_lpCancel )
        {
            bResult = FALSE;
            break;
        }

        if (m_CompressQueue.GetCount() <= 0 && m_pMasterPort->GetPortState() != PortState_Active)
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

        ULONG dwSourceLen = sizeof(ULONGLONG) + sizeof(DWORD) + dataInfo->dwDataSize;
        BYTE *pBuffer = new BYTE[dwSourceLen];
        ZeroMemory(pBuffer,dwSourceLen);
        memcpy(pBuffer,&dataInfo->ullOffset,sizeof(ULONGLONG));
        memcpy(pBuffer + sizeof(ULONGLONG),&dataInfo->dwDataSize,sizeof(DWORD));
        memcpy(pBuffer + sizeof(ULONGLONG) + sizeof(DWORD),dataInfo->pData,dataInfo->dwDataSize);

        delete []dataInfo->pData;

        ULONG dwDestLen = MAX_COMPRESS_BUF;
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
            m_pMasterPort->SetResult(FALSE);
            m_pMasterPort->SetPortState(PortState_Fail);
            m_pMasterPort->SetErrorCode(ErrorType_Custom,CustomError_Compress_Error);

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Compress data error."));
        }
        else if (ret == Z_MEM_ERROR)
        {
            bResult = FALSE;
            m_pMasterPort->SetResult(FALSE);
            m_pMasterPort->SetPortState(PortState_Fail);
            m_pMasterPort->SetErrorCode(ErrorType_Custom,CustomError_Compress_Error);

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Compress memory error."));
        }
        else if (ret == Z_BUF_ERROR)
        {
            bResult = FALSE;
            m_pMasterPort->SetResult(FALSE);
            m_pMasterPort->SetPortState(PortState_Fail);
            m_pMasterPort->SetErrorCode(ErrorType_Custom,CustomError_Compress_Error);

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Compress buffer error."));
        }
        else
        {
            bResult = FALSE;
            m_pMasterPort->SetResult(FALSE);
            m_pMasterPort->SetPortState(PortState_Fail);
            m_pMasterPort->SetErrorCode(ErrorType_Custom,CustomError_Compress_Error);

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

    while (!*m_lpCancel && m_pMasterPort->GetResult() && bResult)
    {
//		QueryPerformanceCounter(&t0);
        while (m_CompressQueue.GetCount() <= 0
            && !*m_lpCancel
            && m_pMasterPort->GetResult()
            && m_pMasterPort->GetPortState() == PortState_Active)
        {
            Sleep(5);
        }

        if (!m_pMasterPort->GetResult() || *m_lpCancel)
        {
            bResult = FALSE;
            break;
        }

        if (m_CompressQueue.GetCount() <= 0 && m_pMasterPort->GetPortState() != PortState_Active)
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

        ULONG dwDestLen = MAX_COMPRESS_BUF;
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
            m_EffList.push_back(effData);

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
            m_pMasterPort->SetResult(FALSE);
            m_pMasterPort->SetPortState(PortState_Fail);
            m_pMasterPort->SetErrorCode(ErrorType_Custom,CustomError_UnCompress_Error);

            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Uncompress data error."));
        }
        else if (ret == Z_MEM_ERROR)
        {
            bResult = FALSE;
            m_pMasterPort->SetResult(FALSE);
            m_pMasterPort->SetPortState(PortState_Fail);
            m_pMasterPort->SetErrorCode(ErrorType_Custom,CustomError_UnCompress_Error);

            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Uncompress memory error."));
        }
        else if (ret == Z_BUF_ERROR)
        {
            bResult = FALSE;
            m_pMasterPort->SetResult(FALSE);
            m_pMasterPort->SetPortState(PortState_Fail);
            m_pMasterPort->SetErrorCode(ErrorType_Custom,CustomError_UnCompress_Error);

            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Uncompress buffer error."));
        }
        else
        {
            bResult = FALSE;
            m_pMasterPort->SetResult(FALSE);
            m_pMasterPort->SetPortState(PortState_Fail);
            m_pMasterPort->SetErrorCode(ErrorType_Custom,CustomError_UnCompress_Error);

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
    USE_PARAM(port);
    return FALSE;
}

BOOL Disk::FullRWTest(CPort *port)
{
    USE_PARAM(port);
    return FALSE;
}

BOOL Disk::FadePicker(CPort *port)
{
    USE_PARAM(port);
    return FALSE;
}

BOOL Disk::SpeedCheck(CPort *port)
{
    USE_PARAM(port);
    return FALSE;
}

BOOL Disk::CompareClean(CPort *port)
{
    USE_PARAM(port);
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
    USE_PARAM(port);
    USE_PARAM(pDataQueue);
    return FALSE;
}

BOOL Disk::VerifyFiles(CPort *port, CHashMethod *pHashMethod)
{
    USE_PARAM(port);
    USE_PARAM(pHashMethod);
    return FALSE;
}

BOOL Disk::FormatDisk(CPort *port)
{
    USE_PARAM(port);
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

    HANDLE hFile = GetHandleOnFile(port->GetFileName(),O_RDWR|O_CREAT,&dwErrorCode);

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

    while (!*m_lpCancel && m_pMasterPort->GetResult() && port->GetResult())
    {
//        QueryPerformanceCounter(&t0);
        while(pDataQueue->GetCount() <=0 && !*m_lpCancel && m_pMasterPort->GetResult()
            && (m_pMasterPort->GetPortState() == PortState_Active || !m_bCompressComplete)
            && port->GetResult())
        {
            Sleep(5);
        }

        if (!m_pMasterPort->GetResult())
        {
            errType = m_pMasterPort->GetErrorCode(&dwErrorCode);
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

        if (pDataQueue->GetCount() <= 0 && m_pMasterPort->GetPortState() != PortState_Active && m_bCompressComplete)
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

        if (!WriteFileAsyn(hFile,ullOffset,dwLen,dataInfo->pData,&dwErrorCode))
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

    if (!m_pMasterPort->GetResult())
    {
        bResult = FALSE;
        errType = m_pMasterPort->GetErrorCode(&dwErrorCode);
    }

    if (!port->GetResult())
    {
        errType = port->GetErrorCode(&dwErrorCode);
        bResult = FALSE;
    }

    // 写IMAGE头
    if (bResult)
    {
        ULONGLONG ullSize = 0;
//        GetFileSizeEx(hFile,&liSize);
        // 随机生成一个数作为IMAGEID

        IMAGE_HEADER imgHead;
        ZeroMemory(&imgHead,sizeof(IMAGE_HEADER));
        memcpy(imgHead.szImageFlag,IMAGE_FLAG,strlen(IMAGE_FLAG));
        imgHead.ullImageSize = ullSize;
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
        imgHead.ullCapacitySize = m_pMasterPort->GetTotalSize();
        imgHead.dwBytesPerSector = m_pMasterPort->GetBytesPerSector();
        memcpy(imgHead.szZipVer,ZIP_VERSION,strlen(ZIP_VERSION));
        imgHead.byUnCompress = m_bDataCompress ? 0 : 1;
        imgHead.ullPkgCount = ullPkgIndex;
        imgHead.ullValidSize = m_pMasterPort->GetValidSize();
        imgHead.dwHashLen = m_pMasterHashMethod->getHashLength();
        imgHead.dwHashType = m_pMasterPort->GetHashMethod();
        memcpy(imgHead.byImageDigest,m_pMasterHashMethod->digest(),m_pMasterHashMethod->getHashLength());
        imgHead.byEnd = END_FLAG;

        dwLen = SIZEOF_IMAGE_HEADER;

        if (!WriteFileAsyn(hFile,0,dwLen,(LPBYTE)&imgHead,&dwErrorCode))
        {
            bResult = FALSE;

//            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Write image head error,filename=%s,Speed=%.2f,system errorcode=%ld,%s")
//                ,port->GetFileName(),port->GetRealSpeed(),dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
        }
    }

//    CloseHandle(hFile);

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
    USE_PARAM(port);
    USE_PARAM(pDataQueue);
    return FALSE;
}

BOOL Disk::SearchUserLog(CPort *port)
{
    USE_PARAM(port);
    return FALSE;
}

void Disk::SearchUserLog(CString strPath, CString strType, CStringArray *pArray)
{
    USE_PARAM(strPath);
    USE_PARAM(strType);
    USE_PARAM(pArray);
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
    USE_PARAM(port);
    return FALSE;
}

int Disk::EnumFile(CString strPath, CString strVolume, CMapStringToULL *pMap, CMapStringToString *pArray)
{
    USE_PARAM(strPath);
    USE_PARAM(strVolume);
    USE_PARAM(pMap);
    USE_PARAM(pArray);
    return FALSE;
}

BOOL Disk::EnumFileSize(CPort *port, CMapStringToULL *pMap, CMapStringToString *pArray)
{
    USE_PARAM(port);
    USE_PARAM(pMap);
    USE_PARAM(pArray);
    return FALSE;
}

BOOL Disk::ComputeHashValue(CPort *port, CMapStringToString *pMap)
{
    USE_PARAM(port);
    USE_PARAM(pMap);
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
    USE_PARAM(dwDiskData);
    USE_PARAM(nFirstIndex);
    USE_PARAM(nLastIndex);

    return 0;
}
