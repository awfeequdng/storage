#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <mutex>
#include <thread>

#include <QMainWindow>

#include "GlobalData/GlobalDef.h"
#include "GlobalData/gcfgmainwindow.h"

#include "UI/capconvertdupitemdlg.h"
#include "UI/cleardiskitemdlg.h"
#include "UI/createimageitemdlg.h"
#include "UI/dupfromimageitemdlg.h"
#include "UI/globaldupitemdlg.h"
#include "UI/globalparamsettingdlg.h"
#include "UI/localimagemanagedlg.h"
#include "UI/loginauthendlg.h"
#include "UI/runbatchcmddlg.h"
#include "UI/selectcurpatterndlg.h"
#include "UI/softwareupdateitemdlg.h"
#include "UI/synchronizingdlg.h"
#include "UI/syncimagedlg.h"
#include "UI/systemfuncdlg.h"
#include "UI/userdefineitemdlg.h"
#include "UI/varisizedupitemdlg.h"
#include "UI/viewimagedlg.h"


#include "common/cini.h"
#include "common/cutils.h"
#include "common/cstring.h"

#include "SerialPortCommand/PortCommand.h"

#include "Disk/Port.h"
#include "Disk/disk.h"

#define MAX_RUN_TIMES  500

#define TIMER_UPDATE_STATISTIC 1
#define TIMER_SEND_BITMAP      2
#define TIMER_LISENCE          3
#define TIMER_CONNECT_SERVER   4
#define TIMER_MASTER_EXIST     5
#define TIMER_AUTHORIZE        6

namespace Ui {
class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT
public:

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void slotGlobalParamSettingChange();
private slots:
    void on_btnSystemFunc_clicked();

    void on_btnCurOperate_clicked();

    void on_btnParamSetting_clicked();

    void on_btnLockScreen_clicked();

    void on_btnStart_clicked();

    void on_btnGiveUp_clicked();
private:
    void initial();
    void initialConfig();
    void release();


private:
    Ui::MainWindow *ui;


    CString       m_strAppPath;
    HANDLE        m_hLogFile;
    HANDLE        m_hBurninEvent;
    HANDLE        m_hCommHandle;
    BOOL          m_bLock;
    BOOL          m_bCancel;
    BOOL          m_bAuthorize;
    CString       m_strPassWord;
    CString       m_strMachineMode;

    UINT          m_nResumeThread;
    UINT          m_nPortNum;
    CIni          m_Config;

    CPort         m_MasterPort;
    CPort         m_FilePort;
    CPort         *m_pSinglePort;
    PortList      m_TargetPorts;

    WorkMode      m_WorkMode;

    QSerialPort   m_SerialPort;
    PortCommand  m_Command;

//    CTime         m_StartTime;
//    CTime         m_StartTimeTemp;

//    CFont         m_font;
//    CFont         m_LogoFont;

    CString       m_strMsg;
    BOOL          m_bResult;
    BOOL          m_bRunning;
    BOOL          m_bUpdate;
    BOOL          m_bExportLog;
    BOOL          m_bFileCopyFlag;
    BOOL          m_bMasterExit;

//    CWinThread    *m_ThreadListen;

    BOOL          m_bBurnInTest;    // 判断是从母盘文件拷贝还是从本地文件拷贝

    // 网络部分
    SOCKET m_ClientSocket;
    BOOL   m_bSockeConnected;
    BOOL   m_bServerFirst;  // 仅仅在Copy Image时有用
//    CCriticalSection m_CSConnectSever;
    std::mutex m_mtx;

    BOOL  m_bStart; //用于标记当前是开始状态还是结束状态

    BOOL  m_bEnableButton; // 开始和结束键是否可以按

    BOOL  m_bLisence;

    int   m_nMachineType;
    BOOL  m_bIsMTP;

    //记录监控界面的列和行
    UINT m_nCols;
    UINT m_nRows;

    CString m_strConfigPath; //记录配置文件的完整路径

    void ChangeSize( CWnd *pWnd,int cx, int cy );

    void InitialMachine();

    void InitialPortFrame();
    void ResetPortFrame();
    void UpdatePortFrame(BOOL bStart);
    void EnableKickOff(BOOL bEnable);

    void InitialPortPath();
    void InitialSerialPort();
    void ResetPortInfo();

    void InitialCurrentWorkMode();
    void InitialStatisticInfo();
    void UpdateStatisticInfo();

    void InitialBurnInTest(UINT cycle,UINT nCycleCount);

    // 网络部分
    DWORD CreateSocketConnect();
    BOOL SyncTime();

    void EnableControls(BOOL bEnable);

    BOOL IsReady();
    BOOL IsExistTarget();
    BOOL IsExistMaster();

    CString GetCustomErrorMsg(CustomError customError);
    CString GetWorkModeString(WorkMode mode);

    void OnStart();
    void OnStartAsync();

    void EnumDevice();
    void MatchDevice();

    void EnumMTPDevice();
    void MatchMTPDevice();

    void BackupLogfile(HANDLE hFile,DWORD dwFileSize);

    static DWORD  StartThreadProc(LPVOID lpParm);
    static DWORD  StopThreadProc(LPVOID lpParm);
    static DWORD  InitialMachineThreadProc(LPVOID lpParm);
    static DWORD  EnumDeviceThreadProc(LPVOID lpParm);
    static DWORD  BurnInTestThreadProc(LPVOID lpParm);
    static DWORD  ConnectSocketThreadProc(LPVOID lpParm);
    static DWORD  StartAsyncThreadProc(LPVOID lpParm);

    void BurnInTest();
    DWORD UploadLog(CString strLogName,CString strLog);
    CString GetUploadLogString();
    void WriteUploadLog(CString strLog);
    void SetAllFailed(DWORD dwErrorCode,ErrorType errType);
    void OnStop();

    BOOL IsLisence();
    void CleanRecord();

};

#endif // MAINWINDOW_H
