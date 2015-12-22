#ifndef __DISK_H_
#define __DISK_H_

#include <thread>
#include <mutex>

#include "GlobalData/GlobalDef.h"
#include "common/cini.h"
#include "common/cutils.h"

#include "HashMethod/Checksum32.h"
#include "HashMethod/CRC32.h"
#include "HashMethod/MD5.h"

#include "Disk/Port.h"

#include "SerialPortCommand/PortCommand.h"

class Disk
{
public:
    Disk();

    typedef enum _ENUM_DISK_INDEX
    {
        DISK_FIRST=0,
        DISK_SECOND,
        DISK_THIRD,
        DISK_FOURTH,
        DISK_FIFTH,
        DISK_SIXTH,
        DISK_SEVENTH,
        DISK_EIGHTH,
        DISK_NINETH,
        DISK_TEN,
        DISK_NUM
    }DISK_INDEX;
    //静态方法
    static HANDLE GetHandleOnPhysicalDrive(CString strDiskPath,DWORD dwFlagsAndAttributes,PDWORD pdwErrorCode);
    //open disk index of iDiskNumber,start from 0
    static HANDLE GetHandleOnPhysicalDrive(int iDiskNumber, DWORD dwFlagsAndAttributes, PDWORD pdwErrorCode);
    static HANDLE GetHandleOnVolume(CString letter,DWORD dwFlagsAndAttributes,PDWORD pdwErrorCode);
    static HANDLE GetHandleOnFile(CString strFileName,DWORD dwFlagsAndAttributes,PDWORD pdwErrorCode);
    static ULONGLONG GetNumberOfSectors(HANDLE hDevice,PDWORD pdwBytesPerSector,MEDIA_TYPE *type);
    static void DeleteDirectory( LPCTSTR lpszPath );
    static BOOL CreateDisk(DISK_INDEX disk,ULONG PartitionNumber);
//    static BOOL DestroyDisk(DISK_INDEX disk);
    static BOOL DestroyDisk(HANDLE hDisk);
    static BOOL ChangeLetter(LPCTSTR lpszVolumeName,LPCTSTR lpszVolumePath);
    static BOOL GetDiskSymbol(LPCTSTR lpszVolumeName);
    static BOOL LockOnVolume(HANDLE hVolume,DWORD *pdwErrorCode);
    static BOOL UnLockVolume(HANDLE hVolume,DWORD *pdwErrorCode);
    static BOOL UnmountVolume(HANDLE hVolume,DWORD *pdwErrorCode);
    static BOOL IsVolumeUnmount(HANDLE hVolume);
    static BOOL SetDiskAtrribute(HANDLE hDisk,BOOL bReadOnly,BOOL bOffline,PDWORD pdwErrorCode);
    static BOOL GetTSModelNameAndSerialNumber(HANDLE hDevice,LPTSTR lpszModulName,LPTSTR lpszSerialNum,DWORD *pdwErrorCode);
    static BOOL GetDiskModelNameAndSerialNumber(HANDLE hDevice,LPTSTR lpszModulName,LPTSTR lpszSerialNum,DWORD *pdwErrorCode);
    static BOOL GetUsbHDDModelNameAndSerialNumber(HANDLE hDevice,LPTSTR lpszModulName,LPTSTR lpszSerialNum,DWORD *pdwErrorCode);
    static BOOL GetCFSerialNumber(HANDLE hDevice, LPTSTR lpszSerialNum, DWORD *pdwErrorCode);
    static BOOL GetFileSize(int fd, ULONGLONG &ullSize);
    // 公共方法
    void Init(HWND hWnd,LPBOOL lpCancel,HANDLE hLogFile,PortCommand *pCommand,UINT nBlockSectors);
    void SetMasterPort(CPort *port);
    void SetSingleTargetPort(CPort *port);
    void SetTargetPorts(PortList *pList);
    void SetHashMethod(BOOL bComputeHash,HashMethod hashMethod);
    void SetWorkMode(WorkMode workMode);
    void SetGlobalParm(BOOL bMustSameCapacity);
    void SetCleanMode(CleanMode cleanMode,int nFillValue,BOOL bCompareClean,CompareCleanSeq seq);
    void SetCompareMode(CompareMode compareMode);
    void SetFileAndFolder(BOOL bCleanupTarget,const CStringArray &fileArray,const CStringArray &folderArray);
    void SetFormatParm(CString strVolumeLabel,FileSystem fileSystem,DWORD dwClusterSize,BOOL bQuickFormat);
    void SetMakeImageParm(int nImageType,BOOL bCompress,int compressLevel);
    void SetFullCopyParm(BOOL bAllowCapGap,UINT nPercent);
    void SetQuickCopyParm(RANGE_FROM_TO *ranges,int count);
    void SetCleanDiskFirst(BOOL bCleanDiskFist,BOOL bCompareClean,CompareCleanSeq seq,int times,int *values);
    void SetCompareParm(BOOL bCompare,CompareMethod method);
    void SetFullRWTestParm(BOOL bReadOnlyTest,BOOL bRetainOriginData,BOOL bFormatFinished,BOOL bStopBadBlock);
    void SetFadePickerParm(BOOL bRetainOriginData,BOOL bFormatFinished);
    void SetSpeedCheckParm(BOOL bCheckSpeed,double dbReaddSpeed,double dbWriteSpeed);
    BOOL Start();

    void SetSocket(SOCKET sClient,BOOL bServerFirst);

    // 2014-10-14 增量拷贝新增
    void SetDiffCopyParm(UINT nSourceType,BOOL bServer,UINT nCompareRule,BOOL bUpload,const CStringArray &logArray,BOOL bIncludeSunDir);
protected:
    BOOL ReadSectors(HANDLE hDevice,
        ULONGLONG ullStartSector,
        DWORD dwSectors,
        DWORD dwBytesPerSector,
        LPBYTE lpSectBuff,
        LPOVERLAPPED lpOverlap,
        DWORD *pdwErrorCode,
        DWORD dwTimeOut = 30000);
    BOOL WriteSectors(HANDLE hDevice,
        ULONGLONG ullStartSector,
        DWORD dwSectors,
        DWORD dwBytesPerSector,
        LPBYTE lpSectBuff,
        LPOVERLAPPED lpOverlap,
        DWORD *pdwErrorCode,
        DWORD dwTimeOut = 30000);
    BOOL WriteFile(HANDLE hDevice,
                  LPBYTE lpSectBuff,
                  DWORD dwLen,
                  DWORD &dwWriteLen,
                  LPOVERLAPPED lpOverlap);

    BOOL ReadFileAsyn(HANDLE hFile,
        ULONGLONG ullOffset,
        DWORD &dwSize,
        LPBYTE lpBuffer,
        LPOVERLAPPED lpOverlap,
        PDWORD pdwErrorCode,
        DWORD dwTimeOut = 50000);
    BOOL WriteFileAsyn(HANDLE hFile,
        ULONGLONG ullOffset,
        DWORD &dwSize,
        LPBYTE lpBuffer,
        LPOVERLAPPED lpOverlap,
        PDWORD pdwErrorCode,
        DWORD dwTimeOut= 50000);



        BOOL ReadSectors(HANDLE hDevice,
                ULONGLONG ullStartSector,
                DWORD dwSectors,
                DWORD dwBytesPerSector,
                LPBYTE lpSectBuff,
                DWORD *pdwErrorCode,
                DWORD dwTimeOut = 30000);
        BOOL ReadFile(HANDLE hDevice,
                      LPBYTE lpSectBuff,
                      DWORD dwLen,
                      DWORD &dwReadLen,
                      ULONGLONG offset);
            BOOL WriteSectors(HANDLE hDevice,
                ULONGLONG ullStartSector,
                DWORD dwSectors,
                DWORD dwBytesPerSector,
                LPBYTE lpSectBuff,
                DWORD *pdwErrorCode,
                DWORD dwTimeOut = 30000);
            BOOL WriteFile(HANDLE hDevice,
                          LPBYTE lpSectBuff,
                          DWORD dwLen,
                          DWORD &dwWriteLen,
                          ULONGLONG offset);
            BOOL ReadFileAsyn(HANDLE hFile,
                ULONGLONG ullOffset,
                DWORD &dwSize,
                LPBYTE lpBuffer,
                PDWORD pdwErrorCode,
                DWORD dwTimeOut = 50000);
            BOOL WriteFileAsyn(HANDLE hFile,
                ULONGLONG ullOffset,
                DWORD &dwSize,
                LPBYTE lpBuffer,
                PDWORD pdwErrorCode,
                DWORD dwTimeOut= 50000);

private:
    // 文件系统分析
    BootSector GetBootSectorType(const PBYTE pXBR);
    PartitionStyle GetPartitionStyle(const PBYTE pByte,BootSector bootSector);
    FileSystem GetFileSystem(const PBYTE pDBR,ULONGLONG ullStartSector);
    BOOL BriefAnalyze();
    BOOL AppendEffDataList(const PBYTE pDBR,FileSystem fileSystem,ULONGLONG ullStartSector,ULONGLONG ullMasterSectorOffset,ULONGLONG ullSectors,BOOL bMBR);
    ULONGLONG ReadOffset(const PBYTE pByte,DWORD offset,BYTE bytes);
    BOOL IsFATValidCluster(const PBYTE pByte,DWORD cluster,BYTE byFATBit);
    BOOL IsNTFSValidCluster(const PBYTE pByte,DWORD cluster);
    BOOL IsExtXValidBlock(const PBYTE pByte,DWORD block);

    void SetValidSize(ULONGLONG ullSize);
    ULONGLONG GetValidSize();
    BOOL IsAllFailed(ErrorType &errType,PDWORD pdwErrorCode);

    void AddDataQueueList(PDATA_INFO dataInfo);
    BOOL IsReachLimitQty(int limit);

    BOOL IsTargetsReady();

    void ClearDataQueueList();

    int EnumFile(CString strSource, int level = 0);

    UINT GetCurrentTargetCount();

    BOOL QuickClean(CPort *port,PDWORD pdwErrorCode);
    BOOL QuickClean(HANDLE hDisk,CPort *port,PDWORD pdwErrorCode);

    // 线程
    static void*  ReadDiskThreadProc(LPVOID lpParm);
    static void*  ReadImageThreadProc(LPVOID lpParm);
    static void*  WriteDiskThreadProc(LPVOID lpParm);
    static void*  WriteDiskWithDataThreadProc(LPVOID lpParam);
    static void*  WriteImageThreadProc(LPVOID lpParm);
    static void*  VerifyThreadProc(LPVOID lpParm);
    static void*  CompressThreadProc(LPVOID lpParm);
    static void*  UnCompressThreadProc(LPVOID lpParm);
    static void*  CleanDiskThreadProc(LPVOID lpParm);
    static void*  VerifySectorThreadProc(LPVOID lpParm);

    static void*  ReadFilesThreadProc(LPVOID lpParm);
    static void*  WriteFilesThreadProc(LPVOID lpParm);
    static void*  VerifyFilesThreadProc(LPVOID lpParm);
    static void*  VerifyFilesByteThreadProc(LPVOID lpParm);

    static void*  FormatDiskThreadProc(LPVOID lpParm);

    // 2014-10-14 增量拷贝新增
    static void*  SearchUserLogThreadProc(LPVOID lpParm);
    static void*  DeleteChangeThreadProc(LPVOID lpParm);
    static void*  EnumFileThreadProc(LPVOID lpParm);
    static void*  ComputeHashThreadProc(LPVOID lpParm);

    // 2014-11-21 卡检测
    static void*  FullRWTestThreadProc(LPVOID lpParm);
    static void*  FadePickerThreadProc(LPVOID lpParm);
    static void*  SpeedCheckThreadProc(LPVOID lpParm);

    static void*  CompareCleanThreadProc(LPVOID lpParm);

    // 2015-05-09 异步拷贝
    static void*  CopyFilesAsyncThreadProc(LPVOID lpParm);
    //        BOOL CopyFilesAsync(CPort *port, LPOVERLAPPED *pOverlapped);
    BOOL OnCopyFilesAsync();

    BOOL OnCopyDisk();
    BOOL OnCopyImage();
    BOOL OnMakeImage();
    BOOL OnCompareDisk();
    BOOL OnCleanDisk();
    BOOL OnCopyFiles();
    BOOL OnFormatDisk();
    // 2014-10-14 增量拷贝新增
    BOOL OnDiffCopy();

    BOOL OnFullRWTest();
    BOOL OnFadePickerTest();
    BOOL OnSpeedCheck();

    BOOL ReadDisk();
    BOOL ReadImage();
    BOOL WriteDisk(CPort *port,CDataQueue *pDataQueue);
    BOOL WriteImage(CPort *port,CDataQueue *pDataQueue);
    BOOL VerifyDisk(CPort *port,CHashMethod *pHashMethod);
    BOOL VerifyDisk(CPort *port,CDataQueue *pDataQueue);
    BOOL Compress();
    BOOL Uncompress();
    BOOL CleanDisk(CPort *port);
    BOOL FullRWTest(CPort *port);
    BOOL FadePicker(CPort *port);
    BOOL SpeedCheck(CPort *port);

    BOOL CompareClean(CPort *port);

    BOOL ReadFiles();
    BOOL ReadLocalFiles();
    BOOL WriteFiles(CPort *port,CDataQueue *pDataQueue);
    BOOL VerifyFiles(CPort *port,CHashMethod *pHashMethod);
    BOOL VerifyFiles(CPort *port,CDataQueue *pDataQueue);

    BOOL FormatDisk(CPort *port);

    BOOL ReadLocalImage();
    BOOL ReadRemoteImage();

    BOOL WriteLocalImage(CPort *port,CDataQueue *pDataQueue);
    BOOL WriteRemoteImage(CPort *port,CDataQueue *pDataQueue);

    // 2014-10-14 增量拷贝新增
    BOOL SearchUserLog(CPort *port);
    void SearchUserLog(CString strPath,CString strType,CStringArray *pArray);
    void CleanMapPortStringArray();
    BOOL UploadUserLog();
    BOOL ReadRemoteFiles();
    BOOL DownloadChangeList();
    BOOL QueryChangeList();
    BOOL DeleteChange(CPort *port);
    int EnumFile(CString strPath,CString strVolume,CMapStringToULL *pMap,CMapStringToString *pArray);

    BOOL EnumFileSize(CPort *port,CMapStringToULL *pMap,CMapStringToString *pArray);
    BOOL ComputeHashValue(CPort *port,CMapStringToString *pMap);
    void CompareFileSize();
    void CompareHashValue();

    static CHAR *ConvertSENDCMDOUTPARAMSBufferToString(const DWORD *dwDiskData, DWORD nFirstIndex, DWORD nLastIndex);

private:
    HWND     m_hWnd;
    CUtils   m_utils;
    CIni     m_ini;
    CPort    *m_pMasterPort;
    CPort    *m_pSinglePort;
    PortList *m_TargetPorts;
    int      m_hLogFile;
    LPBOOL   m_lpCancel;
    int      m_hMaster;

    ULONGLONG m_ullCapacity;
    ULONGLONG m_ullImageSize; // 用于记录IMAGE Size,在映像拷贝中会用到
    ULONGLONG m_ullSectorNums;
    DWORD     m_dwBytesPerSector;
    ULONGLONG m_ullValidSize;

    // 相同参数
    BOOL m_bComputeHash;
    BOOL m_bCompare;
    CompareMethod m_CompareMethod;
    HashMethod m_HashMethod;
    CHashMethod *m_pMasterHashMethod;
    BYTE m_ImageHash[LEN_DIGEST];
    CString m_strMasterHash;
    WorkMode m_WorkMode;

    // 拷贝之前擦除
    BOOL m_bCleanDiskFirst;
    int  m_nCleanTimes;
    int  *m_pCleanValues;
    CompareCleanSeq m_CompareCleanSeq;
    PBYTE m_pFillBytes;

    //全盘拷贝参数
    BOOL m_bAllowCapGap;
    UINT m_nCapGapPercent;

    // 磁盘擦除参数
    CleanMode m_CleanMode;
    int       m_nFillValue;

    // 磁盘比较参数
    CompareMode m_CompareMode;

    EFF_LIST  m_EffList;

    CDataQueue m_CompressQueue;
    CDataQueueList m_DataQueueList;
    CDataQueue m_DataQueue;

    CStringArray  m_FileArray;
    CStringArray  m_FolderArray;

    CMapStringToULL m_MapCopyFiles;
    CStringArray m_CopyFolderArray;

    // 格式化参数
    BOOL m_bQuickFormat;
    FileSystem m_FileSystem;
    DWORD m_dwClusterSize;
    CString m_strVolumnLabel;

    PortCommand *m_pCommand;

    BOOL m_bCompressComplete; //压缩线程和解压线程是否结束

    //映像拷贝参数
    SOCKET m_ClientSocket;
    BOOL   m_bServerFirst;

    //映像制作参数
    int m_nImageType;
    int m_iCompressLevel;

    UINT m_nBlockSectors;

    // 记录是否显示结果
    BOOL m_bEnd;
    BOOL m_bCompareClean;
    BOOL m_bVerify;

    // 全盘读写测试参数
    BOOL m_bReadOnlyTest;
    BOOL m_bRetainOrginData;
    BOOL m_bFormatFinished;
    BOOL m_bStopBadBlock;

    BOOL m_bCheckSpeed;
    double m_dbMinReadSpeed;
    double m_dbMinWriteSpeed;

    // 2014-12-19
    BOOL m_bMustSameCapacity;
    CListRangeFromTo m_ListRangeFromTo;
    BOOL m_bDataCompress;

    // 2015-01-30 增加全盘文件拷贝
    BOOL m_bCleanupTargets;


    // 20140-10-14 新增增量拷贝
    CMapPortStringArray m_MapPortStringArray;
    CStringArray m_ArrayLogPath;
    BOOL m_bUploadUserLog;
    BOOL m_bIncludeSubDir;
    UINT m_nCompareRule; //0 - FileSize, 1 - HashValue
    UINT m_nSourceType;  //0 - Package, 1 - Master
    CStringArray m_ArrayDelFiles;
    CStringArray m_ArrayDelFolders;
    CMapStringToULL m_MapSizeSourceFiles;
    CMapStringToULL m_MapSizeDestFiles;
    CMapStringToString m_MapHashSourceFiles;
    CMapStringToString m_MapHashDestFiles;
    CStringArray m_ArraySameFile;
    ULONGLONG m_ullHashFilesSize;
    CMapStringToString m_MapSourceFolders;
    CMapStringToString m_MapDestFolders;
};

#endif // __DISK_H_
