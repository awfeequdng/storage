#ifndef _GLOBAL_DEF_H
#define _GLOBAL_DEF_H

#include "common/cstring.h"
#include <list>
#include <vector>
#include <map>
#include <mutex>
#include <QWidget>
#include <unistd.h>

typedef unsigned char       UCHAR;
typedef char                CHAR;
typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned short      USHORT;
typedef unsigned wchar_t    WCHAR;
typedef long                LONG;
typedef unsigned long       ULONG;
typedef unsigned int          DWORD;
typedef int                 BOOL;
typedef unsigned char                BYTE;
typedef BYTE *              PBYTE;
typedef PBYTE               LPBYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef unsigned char *     PUCHAR;
typedef char *              PCHAR;
typedef /*const */char *    LPCTSTR;
typedef char *              LPTSTR;
typedef void *              LPVOID;
typedef DWORD *             DWORD_PTR;
typedef BOOL *              LPBOOL;
typedef unsigned long long  ULONGLONG;
typedef ULONGLONG *         PULONGLONG;
typedef DWORD *             PDWORD;
typedef int                 HANDLE;
typedef QWidget *           HWND;
typedef QWidget             CWnd;

//typedef char  TCHAR;
typedef void * MEDIA_TYPE;
typedef int  SOCKET;

typedef std::map<CString,ULONGLONG>CMapStringToULL;
typedef std::vector<CString>  CStringArray;
typedef std::vector<ULONGLONG> CULLArray;
class CPort;
typedef std::map<CPort,CStringArray*> CMapPortStringArray;
typedef std::map<CString,CString>CMapStringToString;



#if __WORDSIZE == 64
typedef unsigned long int       uint64_t;
#else
__extension__
typedef unsigned long long uint64_t;
#endif

#ifndef _T/*(s)*/
#define _T(s)   (s)
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif



#define LODWORD(_qw)    ((DWORD)(_qw))
#define HIDWORD(_qw)    ((DWORD)(((_qw) >> 32) & 0xffffffff))

#define RANDOM_VALUE -1

#define BUF_SECTORS             (2 * 512) //512KB
#define MAX_COMPRESS_BUF        ((BUF_SECTORS + 8) * 512)
#define CLEAN_LENGTH           (1 * 1024 * 1024) //1M
#define BYTES_PER_SECTOR        512

#define ALLOC(dwBytes)			GlobalAlloc(GPTR,(dwBytes))
#define REALLOC(hMem, dwBytes)	GlobalReAlloc((hMem), (dwBytes), (GMEM_MOVEABLE|GMEM_ZEROINIT))
#define FREE(hMem)				GlobalFree((hMem))

#define DISK_PATH_LEN     20
#define IDENTIFY_BUFFER_SIZE  512
#define LEN_SN			20 //��Ӳ�̵�SN����һ��
#define LEN_MODEL		40 //��Ӳ�̵�model����һ��

#define IDE_ATAPI_IDENTIFY 0xA1   // ��ȡATAPI�豸������
#define IDE_ATA_IDENTIFY  0xEC   // ��ȡATA�豸����

#include <stdio.h>
#include <sys/unistd.h>
inline DWORD GetLastError(){return 0;}
inline void Sleep(int ms){usleep(ms*1000);}
inline void DeleteFile(CString strLogFile){ unlink(strLogFile.c_str());}
inline BOOL PathFileExists(CString strRecentConfig){return access(strRecentConfig.c_str(),F_OK);}

#define USE_PARAM(param) ((param) = (param))

#ifndef ZeroMemory
#define ZeroMemory(pByte,size) memset(pByte,0,size)
#endif

typedef struct STRUCT_OVERLAPPED
{
    DWORD Offset; //= LODWORD(ullOffset);
    DWORD OffsetHigh;// = HIDWORD(ullOffset);
    std::mutex mx;      //mutex
}OVERLAPPED,*LPOVERLAPPED;


typedef struct _STRUCT_LPVOID_PARM
{
    LPVOID lpVoid1;   //CDisk
    LPVOID lpVoid2;   //CPort
    LPVOID lpVoid3;   //CHashMethod or CDataQueue
    LPVOID lpVoid4;   //Other
}VOID_PARM,*LPVOID_PARM;

typedef enum _ENUM_MACHINE_TYPE
{
	MT_TS,
	MT_USB,
    MT_NGFF,
    MT_CF
}MachineType;

// ��������
typedef enum _ENUM_ERROR_TYPE
{
	ErrorType_System,
	ErrorType_Custom
}ErrorType;

//�Զ������
typedef enum _ENUM_CUSTOM_ERROR
{
	CustomError_OK = 0,
	CustomError_Cancel,
	CustomError_No_Master,
	CustomError_No_Target,
	CustomError_Compare_Failed,
	CustomError_All_Targets_Failed,
	CustomError_Master_Failed,
	CustomError_Image_Format_Error,
	CustomError_Compress_Error,
	CustomError_UnCompress_Error,
	CustomError_Speed_Too_Slow,
	CustomError_Unrecognized_Partition,
	CustomError_No_File_Select,
	CustomError_Target_Small,
	CustomError_Format_Error,
	CustomError_Get_Data_From_Server_Error,
	CustomError_Image_Hash_Value_Changed,
	CustomError_Enum_File_Failed,
	CustomError_Master_Target_Size_Not_Same,

	// MTP ERROR
	CustomError_MTP_OpenDevice_Failed,
	CustomError_MTP_EnumStorageID_Failed,
	CustomError_MTP_CreateFolder_Failed,
	CustomError_MTP_NO_ObjectID,
	CustomError_MTP_CreateDataStream_Failed,
	CustomError_MTP_WriteFile_Failed,
	CustomError_MTP_ReadFile_Failed,

	CustomError_ReadSpeed_Slow,
	CustomError_WriteSpeed_Slow

}CustomError;

// ����ģʽ
typedef enum _ENUM_WORK_MODE
{
	WorkMode_None = 0,
	WorkMode_FullCopy = 1,
	WorkMode_QuickCopy,
	WorkMode_FileCopy,
	WorkMode_DifferenceCopy,
	WorkMode_ImageCopy,
	WorkMode_ImageMake,
	WorkMode_DiskClean,
	WorkMode_DiskCompare,
	WorkMode_DiskFormat,
    WorkMode_UserDefine,
	WorkMode_MTPCopy,
	WorkMode_Full_RW_Test,
	WorkMode_Fade_Picker,
	WorkMode_Speed_Check,
	WorkMode_Burnin_Test
}WorkMode;

// �˿�״̬
typedef enum _ENUM_PORT_STATE
{
	PortState_Offline = 0, //û�н����豸
	PortState_Online,    //���豸����
	PortState_Pending,
	PortState_Active,    //���ڹ�����
	PortState_Stop, // �м�ֵ
	PortState_Pass,
	PortState_Fail
}PortState;

typedef enum _ENUM_PORT_TYPE
{
	PortType_NULL = 0,
	PortType_MASTER_DISK,
	PortType_TARGET_DISK,
	PortType_TARGET_FILE,
	PortType_MASTER_FILE,
	PortType_SERVER
}PortType;

typedef enum _ENUM_CLEAN_MODE
{
	CleanMode_Full = 0,
	CleanMode_Quick,
	CleanMode_Safe,
}CleanMode;

typedef enum _ENUM_COMPARE_MODE
{
	CompareMode_Full = 0,
	CompareMode_Quick
}CompareMode;

typedef enum _ENUM_COMPARE_METHOD
{
	CompareMethod_Hash = 0,
	CompareMethod_Byte
}CompareMethod;

typedef enum _ENUM_COMPARE_CLEAN_SEQ
{
	CompareCleanSeq_In = 0,
	CompareCleanSeq_After
}CompareCleanSeq;

typedef enum _ENUM_SAVE_PATH
{
	PathType_Local = 0,
	PathType_Remote
}PathType;

typedef enum _ENUM_SOURCE_TYPE
{
	SourceType_Package = 0,
	SourceType_Master
}SourceType;

typedef enum _ENUM_COMPARE_RULE
{
	CompareRule_Fast = 0,
	CompareRule_Slow
}CompareRule;

typedef enum _ENUM_HASH_METHOD
{
	HashMethod_CHECKSUM32 = 0,
	HashMethod_CRC32,
	HashMethod_MD5
	
}HashMethod;

typedef enum _ENUM_HASH_LENGTH
{
	CHECKSUM32_LEN = 4,
	CRC32_LEN = 4,
	MD5_LEN = 16
}HashLen;

typedef enum _ENUM_LED_COLOR
{
	LedColor_Empty = 0,
	LedColor_Blue,
	LedColor_Green,
	LedColor_Red
}LedColor;

// ������ϵ
typedef enum __ENUM_PARTITION_STYLE
{
	PartitionStyle_UNKNOWN,
	PartitionStyle_DOS,
	PartitionStyle_GPT,
	PartitionStyle_APPLE,
	PartitionStyle_BSD
}PartitionStyle;

typedef enum _ENUM_BOOT_SECTOR
{
	BootSector_UNKNOWN,
	BootSector_MBR,
	BootSector_DBR
}BootSector;

typedef enum _ENUM_FILE_SYSTEM
{
	FileSystem_UNKNOWN,
	FileSystem_EXTEND,
	FileSystem_FAT12,
	FileSystem_FAT16,
	FileSystem_FAT32,
	FileSystem_EXFAT,
	FileSystem_NTFS,
	FileSystem_EXT_X,
	FileSystem_EFI_PART
}FileSystem;

typedef enum _ENUM_IMAGE_TYPE
{
	FULL_IMAGE,
	QUICK_IMAGE,
	MTP_IMAGE,
	FILE_IMAGE
}ImageType;

#pragma pack(push,1)
/****************************************************************************
*     ��������ṹ(16�ֽ�)
****************************************************************************/
typedef struct _STRUCT_PARTITION_ENTRY
{
	UCHAR BootIndicator;  // �ܷ�������־
	UCHAR StartHead;   // �÷�����ʼ��ͷ��
	UCHAR StartSector;  // ��ʼ����Ÿ�2λ��6λ��ʼ������
	UCHAR StartCylinder;  // ��ʼ����ŵ�8λ
	UCHAR PartitionType;  // ��������
	UCHAR EndHead;   // �÷�����ֹ��ͷ��
	UCHAR EndSector;   // ��ֹ����Ÿ�2λ��6λ��ֹ������
	UCHAR EndCylinder;  // ��ֹ����ŵ�8λ
    DWORD StartLBA;   // ��ʼ������                          4 byte
    DWORD TotalSector;  // �����ߴ磨����������                4 byte
}PARTITION_ENTRY,*PPARTITION_ENTRY;

/****************************************************************************
*     ��������¼(MBR)�ṹ 
****************************************************************************/
typedef struct _STRUCT_MASTER_BOOT_RECORD
{
	UCHAR    BootCode[446];
	PARTITION_ENTRY  Partition[4];
	USHORT    Signature;
}MASTER_BOOT_RECORD,*PMASTER_BOOT_RECORD;

//EFI��Ϣͷ
typedef struct _STRUCT_EFI_HEAD_INFO
{
	UCHAR Signature[8];                // 00~07 8
    DWORD RevisionNumber;              // 08~0B 4
    DWORD HeaderSize;                  // 0C~0F 4
    DWORD CRC32Checksum;               // 10~13 4
    DWORD Reserver;                    // 14~17 4
	ULONGLONG PrimaryLBA;              // 18~1F 8
	ULONGLONG BackupLBA;               // 20~27 8
	ULONGLONG FirstUsableLBA;          // 28~2F 8
	ULONGLONG LastUsableLBA;           // 30~37 8
	UCHAR DiskGUID[16];                // 38~47 16
	ULONGLONG PartitionEntryLBA;       // 48~4F 8
    DWORD MaxNumberOfPartitionEntries; // 50~53 4
    DWORD SizeOfPartitionEntry;        // 54~57 4
    DWORD PartitionEntryArrayCRC32;    // 58~5B 4
}EFI_HEAD,*PEFI_HEAD;

// GPT�����������ݽṹ
typedef struct _STRUCT_GPT_PARTITION_ENTRY
{
	UCHAR PartitionTypeGUID[16];    // 00~0F 16
	UCHAR UniquePartitionGUID[16];  // 10~1F 16
	ULONGLONG StartingLBA;          // 20~27 8
	ULONGLONG EndingLBA;            // 28~2F 8
	ULONGLONG AttributeBits;        // 30~37 8
	UCHAR     PartitionName[72];     // 38~7F 72  Unicode
}GPT_PARTITION_ENTRY,*PGPT_PARTITION_ENTRY;

#pragma pack(pop)

// ��Ч����
typedef struct _STRUCT_EFF_DATA
{
	ULONGLONG ullStartSector;
	ULONGLONG ullSectors;
	WORD      wBytesPerSector;
}EFF_DATA,*PEFF_DATA;

#include <list>
typedef std::list<EFF_DATA> EFF_LIST;
typedef std::list<EFF_DATA> *PEFF_LIST;

#pragma pack(push,1)
typedef struct _STRUCT_FAT_INFO
{
	WORD  wBytesPerSector;     // 0B~0C 2
	BYTE  bySectorsPerCluster;  // 0D~0D 1
	WORD  wReserverSectors;     // 0E~0F 2
	BYTE  byNumsOfFAT;          // 10~10 1
	DWORD dwVolumeLength;// 20~23 4
	DWORD dwFATLength;      // FAT32 20~23 4   FAT16 16~17 2
	WORD  wSectorsOfRoot;  // ����ֻ���FAT12/16 11~12 2
	BYTE  byFATBit;
	ULONGLONG ullStartSector;
}FAT_INFO,*PFAT_INFO;

typedef struct _STRUCT_EXFAT_INFO
{
	ULONGLONG ullPartitionOffset; //40~47 8
	ULONGLONG ullVolumeLength;// 48~4F 8
	DWORD dwFatOffset;// 50~53 4
	DWORD dwFatLength;// 54~57 4
	DWORD dwClusterHeapOffset;// 58~5B 4
	DWORD dwClusterCount; //5C~5F 4
	DWORD dwRootDirectoryCluster; //60~63 4
	WORD wBytesPerSector;// 6C~6C 1    Power of 2. Minimum 9 (512 bytes per sector), maximum 12 (4096 bytes per sector)
	DWORD dwSectorsPerCluster;//6D~6D 1    Power of 2. Minimum 0 (1 sector per cluster), maximum 25 �C BytesPerSectorShift, so max cluster size is 32 MB
	BYTE byNumsOfFat;//6E~6E 1 
}EXFAT_INFO,*PEXFAT_INFO;

typedef struct _STRUCT_NTFS_INFO
{
	WORD  wBytesPerSectors;           // 0B~0C 2
	BYTE  bySectorsPerCluster;        // 0D~0D 1
	ULONGLONG ullVolumeLength; // 28~2F 8
	ULONGLONG ullStartCluserOfMFT;    // 30~37 8
	BYTE  byMFTSize;                  // 40~40 1
	ULONGLONG ullStartSector;
}NTFS_INFO,*PNTFS_INFO;

#pragma pack(pop)

typedef struct _STRUCT_SUPER_BLOCK_INFO
{
	DWORD dwBlocksOfVolume;   // 04~07 4
	DWORD dwStartBlockOfGroup0;   // 14~17 4
	DWORD dwBytesPerBlock;  // 18~1B 4  dwBytesPerBlock = 1024 << N;
	DWORD dwBlocksPerBlockGroup; // 20~23 4
	DWORD dwNodesPerBlockGroup;  // 28~2B
	DWORD dwSectorsPerBlock;
}SUPER_BLOCK_INFO,*PSUPER_BLOCK_INFO;

typedef struct _STRUCT_RANGE_FROM_TO
{
	ULONGLONG ullStartLBA;
	ULONGLONG ullEndingLBA;
}RANGE_FROM_TO,*PRANGE_FROM_TO;

typedef std::list<RANGE_FROM_TO> CListRangeFromTo;

// IMAGE define
#define LEN_DIGEST 64
#define SIZEOF_IMAGE_HEADER 1024
#define SIZEOF_IMAGE_DISCRP 256
#define IMAGE_FLAG "PY-IMG"
#define END_FLAG 0xED
#define START_FLAG 0xBE
#define APP_VER_LENGTH						(16)	// this is the length of the FILE_HANDLER_VER
#define PKG_HEADER_SIZE  12
#define ZIP_VERSION   "1.28"
#define APP_VERSION   "1.0.0"

#pragma pack(push, 1)
typedef struct _STRUCT_IMAGE_HEADER
{
	char      szImageFlag[8];  //8
	ULONGLONG ullImageSize;    //8
	char	  szAppVersion[APP_VER_LENGTH]; //16
	DWORD	  dwImageType;  //4
	DWORD	  dwMaxSizeOfPackage; //4
	ULONGLONG ullCapacitySize; //8
	DWORD     dwBytesPerSector; //4
	char	  szZipVer[APP_VER_LENGTH-1]; //15
	BYTE      byUnCompress; // 1 �Ƿ�ѹ��,Ϊ�˼���֮ǰ��IMAGE��0��ʾѹ������0��ʾ��ѹ��
	ULONGLONG ullPkgCount; //8
	ULONGLONG ullValidSize; //8
	DWORD     dwHashLen; //4
	DWORD	  dwHashType; //4
	BYTE	  byImageDigest[LEN_DIGEST]; // 64
	char      szDescription[SIZEOF_IMAGE_DISCRP]; //256
	DWORD     dwPkgOffset; // 4
	DWORD     dwFolderCount; // 4
	DWORD     dwFileCount; // 4
	DWORD     dwListSize; // 4
	BYTE      byReserved[SIZEOF_IMAGE_HEADER-8*5-4*9-APP_VER_LENGTH*2-LEN_DIGEST-SIZEOF_IMAGE_DISCRP-1]; //595
	BYTE      byEnd; //1
}IMAGE_HEADER,*PIMAGE_HEADER;

#pragma pack(pop)


//////////////////////////////////////////////////////////////////////////
//���粿��
//////////////////////////////////////////////////////////////////////////

#ifndef DEF_COMMU_CMD
#define DEF_COMMU_CMD const unsigned long
#endif

#define MAX_BUFFER (512 + 12)*1024

//#pragma pack(push, 1)
typedef struct _STRUCT_CMD_IN
{
	DWORD dwCmdIn;
	DWORD dwSizeSend;
	BYTE  byStop;
}CMD_IN,*PCMD_IN;

typedef struct _STRUCT_CMD_OUT
{
	DWORD dwCmdOut;
	DWORD dwSizeSend;
	ErrorType errType;
	DWORD dwErrorCode;
}CMD_OUT,*PCMD_OUT;

typedef struct _STRUCT_CMD_SYNC_TIME_IN
{
	DWORD dwCmdIn;
	DWORD dwSizeSend;
	BYTE  byStop;
}SYNC_TIME_IN,*PSYNC_TIME_IN;

typedef struct _STRUCT_CMD_SYNC_TIME_OUT
{
	DWORD dwCmdOut;
	DWORD dwSizeSend;
	WORD wYmdHMS[6];
}SYNC_TIME_OUT,*PSYNC_TIME_OUT;

typedef struct _STRUCT_CMD_COPY_IMAGE_IN
{
	DWORD dwCmdIn;
	DWORD dwSizeSend;
	BYTE  byStopFlag;
	char  *pszImageName;
	BYTE  byFlag;

}COPY_IMAGE_IN,*PCOPY_IMAGE_IN;

typedef struct _STRUCT_CMD_COPY_IMAGE_OUT
{
	DWORD dwCmdOut;
	DWORD dwSizeSend;
	ErrorType errType;
	DWORD dwErrorCode;
	BYTE  *pContext;
	BYTE  byFlag;
	
}COPY_IMAGE_OUT,*PCOPY_IMAGE_OUT;

typedef struct _STRUCT_CMD_MAKE_IMAGE_IN
{
	DWORD dwCmdIn;
	DWORD dwSizeSend;
	BYTE  byStopFlag;
	char  *pszImageName;
	BYTE  *pContext;
	BYTE  byFlag;

}MAKE_IMAGE_IN,*PMAKE_IMAGE_IN;

typedef struct _STRUCT_CMD_MAKE_IMAGE_OUT
{
	DWORD dwCmdOut;
	DWORD dwSizeSend;
	ErrorType errType;
	DWORD dwErrorCode;
}MAKE_IMAGE_OUT,*PMAKE_IMAGE_OUT;

typedef struct _STRUCT_CMD_UPLOAD_LOG_IN
{
	DWORD dwCmdIn;
	DWORD dwSizeSend;
	BYTE  byStop;
	char  *pszImageName;
	BYTE  *pContext;
	BYTE  byFlag;

}UPLOAD_LOG_IN,*PUPLOAD_LOG_IN;

typedef struct _STRUCT_CMD_UPLOAD_LOG_OUT
{
	DWORD dwCmdOut;
	DWORD dwSizeSend;
	ErrorType errType;
	DWORD dwErrorCode;
}UPLOAD_LOG_OUT,*PUPLOAD_LOG_OUT;

typedef struct _STRUCT_CMD_QUERY_IMAGE_IN
{
	DWORD dwCmdIn;
	DWORD dwSizeSend;
	BYTE  byStop;
	char  *pszImageName;
	BYTE  byFlag;
}QUERY_IMAGE_IN,*PQUERY_IMAGE_IN;

typedef struct _STRUCT_CMD_QUERY_IMAGE_OUT
{
	DWORD dwCmdOut;
	DWORD dwSizeSend;
	ErrorType errType;
	DWORD dwErrorCode;
	BYTE  byHead[SIZEOF_IMAGE_HEADER];
	BYTE  byFlag;
}QUERY_IMAGE_OUT,*PQUERY_IMAGE_OUT;

// 2014-10-11 ����
typedef struct _STRUCT_CMD_DOWN_PKG_IN
{
	DWORD dwCmdIn;
	DWORD dwSizeSend;
	BYTE  byStop;
	char  *pszImageName;
	BYTE  byFlag;
}DOWN_PKG_IN,DOWN_LIST_IN,*PDOWN_PKG_IN,*PDOWN_LIST_IN;

typedef struct _STRUCT_CMD_DOWN_PKG_OUT
{
	DWORD dwCmdOut;
	DWORD dwSizeSend;
	ErrorType errType;
	DWORD dwErrorCode;
	char  *pszFileName;
	BYTE  *pContext;
	BYTE  byFlag;
}DOWN_PKG_OUT,*PDOWN_PKG_OUT;

typedef struct _STRUCT_CMD_DOWN_LIST_OUT
{
	DWORD dwCmdOut;
	DWORD dwSizeSend;
	ErrorType errType;
	DWORD dwErrorCode;
	BYTE  *pContext;
	BYTE  byFlag;
}DOWN_LIST_OUT,*PDOWN_LIST_OUT;

typedef struct _STRUCT_CMD_CUSTOM_LOG_IN
{
	DWORD dwCmdIn;
	DWORD dwSizeSend;
	BYTE  byStop;
	char  *pszImageName;
	BYTE  *pContext;
	BYTE  byFlag;
}CUSTOM_LOG_IN,*PCUSTOM_LOG_IN;

typedef struct _STRUCT_CMD_CUSTOM_LOG_OUT
{
	DWORD dwCmdOut;
	DWORD dwSizeSend;
	ErrorType errType;
	DWORD dwErrorCode;
}CUSTOM_LOG_OUT,*PCUSTOM_LOG_OUT;

typedef struct _STRUCT_CMD_QUERY_PKG_IN
{
	DWORD dwCmdIn;
	DWORD dwSizeSend;
	BYTE  byStop;
	char  *pszPkgName;
	BYTE  byFlag;
}QUERY_PKG_IN,QUERY_LIST_IN,*PQUERY_PKG_IN,*PQUERY_LIST_IN;

typedef struct _STRUCT_CMD_QUERY_PKG_OUT
{
	DWORD dwCmdOut;
	DWORD dwSizeSend;
	ErrorType errType;
	DWORD dwErrorCode;
	ULONGLONG ullFileSize;
	BYTE  byFlag;
}QUERY_PKG_OUT,QUERY_LIST_OUT,*PQUERY_PKG_OUT,*PQUERY_LIST_OUT;

//#pragma pack(pop)

/************************************************************************/
/* COMMAND:CMD_SYNC_TIME
 * <Input>:
 * DEF_COMMU_CMD	CMD_SYNC_TIME_IN;
 * DWORD dwSizeSend; //���͵��ֽ���;
 * <Output>:
 * DWORD dwSizeOfReturn; //���������ܴ�С��
 * DEF_COMMU_CMD	CMD_SYNC_TIME_OUT;
 * WORD wYmdHMS[4]; //������ʱ����*/
/************************************************************************/
DEF_COMMU_CMD CMD_SYNC_TIME_IN    = 0x43440000;
DEF_COMMU_CMD CMD_SYNC_TIME_OUT   = 0x43440100;

/************************************************************************/
/* COMMAND:CMD_COPY_IMAGE
 * <Input>:
 * DEF_COMMU_CMD CMD_COPY_IMAGE_IN;
 * DWORD dwSizeSend; //���͵��ֽ���;
 * BYTE byStop; //ֹͣ��־
 * char *pszImageName; //IMAGE����
 * BYTE byFlag; //��ʼ������־����ʼ��BE,������ED
 * <Output>:
 * DWORD dwSizeOfReturn; //���������ܴ�С��
 * DEF_COMMU_CMD CMD_COPY_IMAGE_OUT;
 * DWORD ErrorType; //��������
 * DWORD dwErrorCode; //������룬0��ʾ�ɹ�
 * BYTE *pContext; // IMAGE����
 * BYTE byFlag; //��ʼ������־����ʼ��BE,������ED*/
/************************************************************************/
DEF_COMMU_CMD CMD_COPY_IMAGE_IN   = 0x43440001;
DEF_COMMU_CMD CMD_COPY_IMAGE_OUT  = 0x43440101;

/************************************************************************/
/* COMMAND:CMD_MAKE_IMAGE
 * <Input>:
 * DEF_COMMU_CMD CMD_MAKE_IMAGE_IN;
 * DWORD dwSizeSend; //���͵��ֽ���;
 * BYTE byStop; //ֹͣ��־
 * char *pszImageName; //IMAGE���� 0�ַ���β
 * BYTE *pContext; //IMAGE����
 * BYTE byFlag; //��ʼ������־����ʼ��BE,������ED
 * <Output>:
 * DWORD dwSizeOfReturn; //���������ܴ�С��
 * DEF_COMMU_CMD CMD_MAKE_IMAGE_OUT;
 * DWORD ErrorType; //��������
 * DWORD dwErrorCode; //������룬0��ʾ�ɹ� */
/************************************************************************/
DEF_COMMU_CMD CMD_MAKE_IMAGE_IN   = 0x43440002;
DEF_COMMU_CMD CMD_MAKE_IMAGE_OUT  = 0x43440102;

/************************************************************************/
/* COMMAND:CMD_UPLOAD_LOG
 * <Input>:
 * DEF_COMMU_CMD CMD_UPLOAD_LOG_IN;
 * DWORD dwSizeSend; //���͵��ֽ���;
 * char *pszLogName; //LOG���ƣ�0�ַ���β
 * BYTE *pContext; //LOG�ļ�����
 * BYTE byFlag; //��ʼ������־����ʼ��BE,������ED
 * <Output>:
 * DWORD dwSizeOfReturn; //���������ܴ�С��
 * DEF_COMMU_CMD CMD_UPLOAD_LOG_OUT;
 * DWORD ErrorType; //��������
 * DWORD dwErrorCode; //������룬0��ʾ�ɹ� */
/************************************************************************/
DEF_COMMU_CMD CMD_UPLOAD_LOG_IN   = 0x43440003;
DEF_COMMU_CMD CMD_UPLOAD_LOG_OUT  = 0x43440103;

/************************************************************************/
/* COMMAND:CMD_QUERY_IMAGE
 * <Input>:
 * DEF_COMMU_CMD CMD_QUERY_IMAGE_IN;
 * DWORD dwSizeSend; //���͵��ֽ���;
 * char *pszImageName; //IMAGE����
 * BYTE byFlag; //��ʼ������־����ʼ��BE,������ED
 * <Output>:
 * DWORD dwSizeOfReturn; //���������ܴ�С��
 * DEF_COMMU_CMD CMD_QUERY_IMAGE_OUT;
 * DWORD ErrorType; //��������
 * DWORD dwErrorCode; //������룬0��ʾ�ɹ�
 * BYTE head[1024]; // 0�ַ���β*/
/************************************************************************/
DEF_COMMU_CMD CMD_QUERY_IMAGE_IN  = 0x43440004;
DEF_COMMU_CMD CMD_QUERY_IMAGE_OUT = 0x43440104;

/************************************************************************/
/* COMMAND:CMD_SHOW_SCREEN
 * <Input>:
 * DEF_COMMU_CMD CMD_SHOW_SCREEN_IN;
 * DWORD dwSizeSend; //���͵��ֽ���;
 * BYTE *bitmap
 * BYTE byFlag; //��ʼ������־����ʼ��BE,������ED
 * <Output>:
 * DWORD dwSizeOfReturn; //���������ܴ�С��
 * DEF_COMMU_CMD CMD_SHOW_SCREEN_OUT;
 * DWORD ErrorType; //��������
 * DWORD dwErrorCode; //������룬0��ʾ�ɹ�*/
/************************************************************************/
DEF_COMMU_CMD CMD_SHOW_SCREEN_IN  = 0x43440005;
DEF_COMMU_CMD CMD_SHOW_SCREEN_OUT = 0x43440105;

// 2014-10-11 ����

/************************************************************************/
/* COMMAND:CMD_DOWN_PKG
 * <Input>:
 * DEF_COMMU_CMD CMD_DOWN_PKG_IN;
 * DWORD dwSizeSend; //���͵��ֽ���;
 * BYTE byStop; //ֹͣ��־
 * char *pszPkgName; //Package����
 * BYTE byFlag; //��ʼ������־����ʼ��BE,������ED
 * <Output>:
 * DWORD dwSizeOfReturn; //���������ܴ�С��
 * DEF_COMMU_CMD CMD_DOWN_PKG_OUT;
 * DWORD ErrorType; //��������
 * DWORD dwErrorCode; //������룬0��ʾ�ɹ�
 * ULONGLONG ullFileSize;//Package�ļ���С
 * BYTE *pContext; // Package����
 * BYTE byFlag; //��ʼ������־����ʼ��BE,������ED*/
/************************************************************************/
DEF_COMMU_CMD CMD_DOWN_PKG_IN  = 0x43440006;
DEF_COMMU_CMD CMD_DOWN_PKG_OUT = 0x43440106;

/************************************************************************/
/* COMMAND:CMD_USER_LOG
 * <Input>:
 * DEF_COMMU_CMD CMD_USER_LOG_IN;
 * DWORD dwSizeSend; //���͵��ֽ���;
 * char *pszLogName; //LOG���ƣ�0�ַ���β
 * BYTE *pContext; //LOG�ļ�����
 * BYTE byFlag; //��ʼ������־����ʼ��BE,������ED
 * <Output>:
 * DWORD dwSizeOfReturn; //���������ܴ�С��
 * DEF_COMMU_CMD CMD_USER_LOG_IN;
 * DWORD ErrorType; //��������
 * DWORD dwErrorCode; //������룬0��ʾ�ɹ� */
/************************************************************************/
DEF_COMMU_CMD CMD_USER_LOG_IN   = 0x43440007;
DEF_COMMU_CMD CMD_USER_LOG_OUT  = 0x43440107;

/************************************************************************/
/* COMMAND:CMD_QUERY_PKG
 * <Input>:
 * DEF_COMMU_CMD CMD_QUERY_PKG_IN;
 * DWORD dwSizeSend; //���͵��ֽ���;
 * char *pszPkgName; //IMAGE����
 * BYTE byFlag; //��ʼ������־����ʼ��BE,������ED
 * <Output>:
 * DWORD dwSizeOfReturn; //���������ܴ�С��
 * DEF_COMMU_CMD CMD_QUERY_PKG_OUT;
 * DWORD ErrorType; //��������
 * DWORD dwErrorCode; //������룬0��ʾ�ɹ�
 * ULONGLONG ullFileSize; //�ļ���С
 * BYTE byFlag; //��ʼ������־����ʼ��BE,������ED */
/************************************************************************/
DEF_COMMU_CMD CMD_QUERY_PKG_IN  = 0x43440008;
DEF_COMMU_CMD CMD_QUERY_PKG_OUT = 0x43440108;

/************************************************************************/
/* COMMAND:CMD_DOWN_LIST
 * <Input>:
 * DEF_COMMU_CMD CMD_DOWN_LIST_IN;
 * DWORD dwSizeSend; //���͵��ֽ���;
 * BYTE byStop; //ֹͣ��־
 * char *pszPkgName; //Package����
 * BYTE byFlag; //��ʼ������־����ʼ��BE,������ED
 * <Output>:
 * DWORD dwSizeOfReturn; //���������ܴ�С��
 * DEF_COMMU_CMD CMD_DOWN_LIST_OUT;
 * DWORD ErrorType; //��������
 * DWORD dwErrorCode; //������룬0��ʾ�ɹ�
 * ULONGLONG ullFileSize;//Package�ļ���С
 * BYTE *pContext; // Package����
 * BYTE byFlag; //��ʼ������־����ʼ��BE,������ED*/
/************************************************************************/
DEF_COMMU_CMD CMD_DOWN_LIST_IN  = 0x43440009;
DEF_COMMU_CMD CMD_DOWN_LIST_OUT = 0x43440109;

/************************************************************************/
/* COMMAND:CMD_QUERY_LIST
 * <Input>:
 * DEF_COMMU_CMD CMD_QUERY_LIST_IN;
 * DWORD dwSizeSend; //���͵��ֽ���;
 * char *pszPkgName; //IMAGE����
 * BYTE byFlag; //��ʼ������־����ʼ��BE,������ED
 * <Output>:
 * DWORD dwSizeOfReturn; //���������ܴ�С��
 * DEF_COMMU_CMD CMD_QUERY_LIST_OUT;
 * DWORD ErrorType; //��������
 * DWORD dwErrorCode; //������룬0��ʾ�ɹ�
 * ULONGLONG ullFileSize; //�ļ���С
 * BYTE byFlag; //��ʼ������־����ʼ��BE,������ED */
/************************************************************************/
DEF_COMMU_CMD CMD_QUERY_LIST_IN  = 0x4344000A;
DEF_COMMU_CMD CMD_QUERY_LIST_OUT = 0x4344010A;

#endif
