#include "makeimagedisk.h"

MakeImageDisk::MakeImageDisk()
{
    m_nBlockSectors = 256;
    m_dwBytesPerSector = 512;
}

BOOL MakeImageDisk::make()
{
    return FALSE;
}
CString MakeImageDisk::strImagePath() const
{
    return m_strImagePath;
}

void MakeImageDisk::setStrImagePath(const CString &strImagePath)
{
    m_strImagePath = strImagePath;
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

BOOL MakeImageDisk::ReadFile(HANDLE hDevice, LPBYTE lpSectBuff, DWORD dwLen, DWORD &dwReadLen, ULONGLONG offset)
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
    return bResult;
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













