#ifndef MAKEIMAGEDISK_H
#define MAKEIMAGEDISK_H

#include "common/cstring.h"
#include "GlobalData/GlobalDef.h"

#include "Disk/disk.h"

class MakeImageDisk
{
public:
    MakeImageDisk();
    MakeImageDisk(CString filePath,CString imagePath);
    BOOL make();

    CString strImagePath() const;
    void setStrImagePath(const CString &strImagePath);

    HANDLE hImage() const;
    void setHImage(const HANDLE &hImage);

    CString strDiskPath() const;
    void setStrDiskPath(const CString &strDiskPath);

    HANDLE hDisk() const;
    void setHDisk(const HANDLE &hDisk);
    DWORD nBlockSectors() const;
    void setNBlockSectors(const DWORD &nBlockSectors);

    DWORD dwBytesPerSector() const;
    void setDwBytesPerSector(const DWORD &dwBytesPerSector);

    ULONGLONG ullSectorNums() const;
    void setUllSectorNums(const ULONGLONG &ullSectorNums);

protected:
    BOOL IsReachLimitQty(int limit);
    BOOL ReadRemoteImage();
    BOOL ReadLocalImage();
    BOOL WriteDisk(CString strDskPath, CDataQueue *pDataQueue);
private:
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

    BOOL ReadDisk();
    BOOL ReadImage();

    static void  ReadDiskThreadProc();
    static void  ReadImageThreadProc();
    static void  WriteDiskThreadProc();
    static void  WriteImageThreadProc();
private:
    CString  m_strImagePath;
    HANDLE   m_hImage;

    CString  m_strDiskPath;
    HANDLE   m_hDisk;

    DWORD m_nBlockSectors;
    DWORD m_dwBytesPerSector;

    ULONGLONG m_ullSectorNums;
    ULONGLONG m_ullImageSize;
    ULONGLONG m_ullValidSize;

    BOOL m_bMustSameCapacity;
    BOOL m_bAllowCapGap;

    EFF_LIST  m_EffList;
    CDataQueue m_CompressQueue;
    CDataQueue m_DataQueue;

    BOOL m_bCompressComplete;
    BOOL m_bReadDiskOver;

    BOOL    m_bDataCompress;
    BOOL    m_bServerFirst;

    BOOL WriteImage(CDataQueue *pDataQueue);
    BOOL WriteLocalImage(CDataQueue *pDataQueue);
    BOOL WriteRemoteImage(CDataQueue *pDataQueue);
    BOOL GetFileSize(int fd,ULONGLONG &ullSize);
    BootSector GetBootSectorType(const PBYTE pXBR);
    ULONGLONG ReadOffset(const PBYTE pByte, DWORD offset, BYTE bytes);
    BOOL BriefAnalyze();
    FileSystem GetFileSystem(const PBYTE pDBR, ULONGLONG ullStartSector);
    PartitionStyle GetPartitionStyle(const PBYTE pByte, BootSector bootSector);
    BOOL AppendEffDataList(const PBYTE pDBR, FileSystem fileSystem, ULONGLONG ullStartSector, ULONGLONG ullMasterSectorOffset, ULONGLONG ullSectors, BOOL bMBR);
    BOOL IsExtXValidBlock(const PBYTE pByte, DWORD block);
    BOOL IsFATValidCluster(const PBYTE pByte, DWORD cluster, BYTE byFATBit);
    BOOL IsNTFSValidCluster(const PBYTE pByte, DWORD cluster);
};

#endif // MAKEIMAGEDISK_H
