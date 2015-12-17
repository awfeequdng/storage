#ifndef MAKEIMAGEDISK_H
#define MAKEIMAGEDISK_H

#include "common/cstring.h"
#include "GlobalData/GlobalDef.h"

#include "Disk/disk.h"

class MakeImageDisk
{
public:
    MakeImageDisk();
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

private:
    CString  m_strImagePath;
    HANDLE   m_hImage;

    CString  m_strDiskPath;
    HANDLE   m_hDisk;

    DWORD m_nBlockSectors;
    DWORD m_dwBytesPerSector;
    ULONGLONG m_ullSectorNums;

    EFF_LIST  m_EffList;
    CDataQueue m_CompressQueue;
    CDataQueue m_DataQueue;

};

#endif // MAKEIMAGEDISK_H
