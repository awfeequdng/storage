#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "config.h"
#include "Disk/makeimagedisk.h"

#include "common/PrintMacros.h"

#include <QFile>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

extern GcfgMainWindow *g_pGcfgMainWindow;


#define DEFAULT_WIDTH  800
#define DEFAULT_HEIGHT 600
extern BOOL g_bFilecopyType;
BOOL g_bMasterExist = FALSE;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initial();

}

MainWindow::~MainWindow()
{
    release();

}

void MainWindow::slotGlobalParamSettingChange()
{
    QString str;
    switch(g_pGcfgMainWindow->workMode())
    {
    case WorkMode_QuickCopy:
        str = tr("快速拷贝");
        break;
    case WorkMode_FullCopy:
        str = tr("全盘拷贝");
        break;
    case WorkMode_DifferenceCopy:
        str = tr("容量转换拷贝");
        break;
    case WorkMode_UserDefine:
        str = tr("自定义拷贝");
        break;
    case WorkMode_ImageMake:
        str = tr("制作映像");
        break;
    case WorkMode_ImageCopy:
        str = tr("从映像拷贝");
        break;
    case WorkMode_DiskClean:
        str = tr("清除磁盘");
        break;
    default:
        break;
    }
    ui->btnCurOperate->setText(str);
}

void MainWindow::on_btnSystemFunc_clicked()
{
//    p_SystemFuncDlg->show();
//    p_SystemFuncDlg->exec();
    SystemFuncDlg systemFuncDlg;
    systemFuncDlg.exec();

}

void MainWindow::on_btnCurOperate_clicked()
{
    SelectCurPatternDlg selectCurPattern;
    selectCurPattern.exec();
    slotGlobalParamSettingChange();
}

void MainWindow::on_btnParamSetting_clicked()
{
    switch(g_pGcfgMainWindow->workMode())
    {
    case WorkMode_QuickCopy :
    {
        VariSizeDupItemDlg variSizeDup;
        variSizeDup.exec();
        break;
    }
    case WorkMode_FullCopy/*GLOBAL_DUP*/:
    {
        GlobalDupItemDlg globalDup;
        globalDup.exec();
        break;
    }
    case WorkMode_DifferenceCopy/*CAP_CONVERT_DUP*/:
    {
        CapConvertDupItemDlg capConvertDup;
        capConvertDup.exec();

        break;
    }
    case WorkMode_UserDefine/*USER_DEFINE_DUP*/:
    {
        UserDefineItemDlg userDefine;
        userDefine.exec();
        break;
    }
    case WorkMode_ImageMake/*CREATE_IMAGE*/:
    {
        CreateImageItemDlg createImage;
        createImage.exec();
        break;
    }
    case WorkMode_ImageCopy/*DUP_FROM_IMAGE*/:
    {
        DupFromImageItemDlg dupFromImage;
        dupFromImage.exec();
        break;
    }
    case WorkMode_DiskClean/*CLEAR_DISK*/:
    {
        ClearDiskItemDlg clearDisk;
        clearDisk.exec();
        break;
    }
    default:
        break;
    }
}

void MainWindow::on_btnLockScreen_clicked()
{

}

void MainWindow::on_btnStart_clicked()
{
    switch(g_pGcfgMainWindow->workMode())
    {
    case WorkMode_ImageMake:
    {
        std::thread mkImageThd(makeImageThread);
        mkImageThd.detach();
        break;
    }
//        case WorkMode_
    default:
        break;
    }
}

void MainWindow::on_btnGiveUp_clicked()
{

}

void MainWindow::initial()
{
    m_nPortNum = 6;
    m_bLock = TRUE;
    m_bCancel = FALSE;
    m_bResult = TRUE;
    m_bRunning = FALSE;
    m_bUpdate = FALSE;
    m_bExportLog = FALSE;
    m_bBurnInTest = FALSE;
    m_bStart = FALSE;
    m_bLisence = FALSE;
    m_bSockeConnected = FALSE;
    m_bIsMTP = FALSE;
    m_nMachineType = MT_NGFF;
    m_bEnableButton = TRUE;
    m_bFileCopyFlag = FALSE;
//    m_pFilecopySet = NULL;
    m_bMasterExit = FALSE;
    m_bAuthorize = FALSE;
    m_pSinglePort = NULL;

    m_nCols = 4;
    m_nRows = 3;
    m_nResumeThread = 0;
    m_strMachineMode = _T("");



//    initialConfig();
    slotGlobalParamSettingChange();

}

//BOOL MainWindow::initialConfig()
//{
//    CString strPath = CUtils::GetAppPath();
//    m_strAppPath = CUtils::GetFilePathWithoutName(strPath);

//    CString strLogFile = m_strAppPath + LOG_FILE;
//    m_hLogFile = open(strLogFile.c_str(),O_RDWR|O_CREAT,0666);

//    if (m_hLogFile < 0)
//    {
//        DEBUG(_T("Create Log File Error"));
//        this->close();
//    }


//    // 如果log文件大小超过2M，备份一下
//    ULONGLONG ullFillSize = 0;
//    Disk::GetFileSize(m_hLogFile,ullFillSize);
//    if (ullFillSize > 2 * 1024 * 1024)
//    {
//        BackupLogfile(m_hLogFile,(DWORD)ullFillSize);
//        ::close(m_hLogFile);
//        DeleteFile(strLogFile);
//////        unlink(strLogFile.c_str());
////        ::remove(strLogFile.c_str());

//        m_hLogFile = open(strLogFile.c_str(),O_RDWR|O_CREAT,0666);

//        if (m_hLogFile < 0)
//        {
//            DEBUG(_T("CreateFile Error"));
//            this->close();
//        }

//    }

//    //添加状态栏
//    CString strVersion,strSN,strModel,strAlias;

////    CFileStatus status;
////    CFile::GetStatus(strPath,status);
//    struct stat st;
//    if(stat(strPath.c_str(),&st)< 0)
//    {

//    }

//    CUtils::WriteLogFile(m_hLogFile,FALSE,_T(""));
//    CUtils::WriteLogFile(m_hLogFile,FALSE,_T("********************************************************************************"));
////    CUtils::WriteLogFile(m_hLogFile,TRUE,_T("PE1051 Ver:%s BLD:%s"),CUtils::GetAppVersion(strPath),status.m_mtime.Format(_T("%Y-%m-%d")));

//    // 读机器信息
//    m_Config.SetPathName(m_strAppPath + MACHINE_INFO);
////    strVersion.Format(_T("Ver:%s  BLD:%s "),CUtils::GetAppVersion(strPath),status.m_mtime.Format(_T("%Y-%m-%d")));
//    strSN.Format(_T("%s"),m_Config.GetString(_T("MachineInfo"),_T("SN")).c_str());
//    strModel.Format(_T("%s"),m_Config.GetString(_T("MachineInfo"),_T("Model")).c_str());
//    strAlias.Format(_T("%s"),m_Config.GetString(_T("MachineInfo"),_T("Alias"),_T("PHIYO")).c_str());
//    m_strMachineMode = strModel;

//    // 判断是USB接口还是TF/SD接口
//    if (strModel.Find(_T("TS")) != -1)
//    {
//        m_nMachineType = MT_TS;
//    }
//    else if (strModel.Find(_T("USB")) != -1)
//    {
//        m_nMachineType = MT_USB;
//    }
//    else if (strModel.Find(_T("NGFF")) != -1)
//    {
//        m_nMachineType = MT_NGFF;
//    }
//    else if (strModel.Find(_T("CF")) !=  -1)
//    {
//        m_nMachineType = MT_CF;
//    }

//    // 把配置文件设置回USBCopy.ini
//    m_strConfigPath = m_strAppPath + CONFIG_NAME;
//    m_Config.SetPathName(m_strConfigPath);

//    // 获取最近加载的配置
//    CString strRecentConfig = m_Config.GetString(_T("AppSetting"),_T("RecentConfig"));

//    if (!strRecentConfig.IsEmpty() && PathFileExists(strRecentConfig))
//    {
//        m_Config.SetPathName(strRecentConfig);
//        m_strConfigPath = strRecentConfig;
//    }

//    m_Config.WriteString(_T("Option"),_T("MachineAlias"),strAlias.c_str());

//    int cx = m_Config.GetInt(_T("AppSetting"),_T("Width"),DEFAULT_WIDTH);
//    int cy = m_Config.GetInt(_T("AppSetting"),_T("Height"),DEFAULT_HEIGHT);

//    if (cx == 0 || cy == 0)
//    {
//        cx = DEFAULT_WIDTH;
//        cy = DEFAULT_HEIGHT;
//    }
//    else if (cx == -1 || cy == -1)
//    {
////        cx  = GetSystemMetrics(SM_CXSCREEN);
////        cy = GetSystemMetrics(SM_CYSCREEN);
//    }


//    switch (m_nMachineType)
//    {
//    case MT_TS:
////        SetDlgItemText(IDC_TEXT_LOGO,LOGO_TS);
//        break;

//    case MT_USB:
////        SetDlgItemText(IDC_TEXT_LOGO,LOGO_USB);
//        break;

//    case MT_NGFF:
////        SetDlgItemText(IDC_TEXT_LOGO,LOGO_NGFF);
//        break;
//    case MT_CF:
////        SetDlgItemText(IDC_TEXT_LOGO,LOGO_CF);
//        break;
//    }

//    BOOL bShowCursor = m_Config.GetBool(_T("Option"),_T("ShowCursor"),TRUE);
////    ShowCursor(bShowCursor);

//    // 初始化套接字库
////    WSADATA wsd;
////    if (WSAStartup(MAKEWORD(2,2),&wsd) != 0)
////    {
////        DWORD dwErrorCode  = WSAGetLastError();
////        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("WSAStartup() failed with system error code:%d,%s")
////            ,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
////    }
////    else
////    {
////        // 连接网络，同步时间
////        pthread_t pthConnectSocket;
////        pthread_create(&pthConnectSocket,NULL,ConnectSocketThreadProc,this);
////    }

//    m_bLock = m_Config.GetBool(_T("Option"),_T("En_Lock"),FALSE);
//    m_strPassWord = m_Config.GetString(_T("Option"),_T("Password"),_T("123456"));

//    // Button
////    m_BtnStart.SubclassDlgItem(IDC_BTN_START,this);
////    m_BtnStart.SetBitmaps(IDB_START,RGB(255,255,255));
////    m_BtnStart.DrawBorder(FALSE);
////    SetDlgItemText(IDC_BTN_START,_T(""));

////    m_BtnLock.SubclassDlgItem(IDC_BTN_LOCK,this);
////    m_BtnLock.SetBitmaps(IDB_LOCK,RGB(255,255,255));
////    m_BtnLock.SetFlat(FALSE);

//    if (m_bLock)
//    {
//        EnableControls(FALSE);

//        CString strLock;
////        strLock.LoadString(IDS_UNLOCK);
////        SetDlgItemText(IDC_BTN_LOCK,strLock);
////        m_BtnLock.SetBitmaps(IDB_UNLOCK,RGB(255,255,255));
//    }

////    m_BtnMenu.SubclassDlgItem(IDC_BTN_SYSTEM,this);
////    m_BtnMenu.SetBitmaps(IDB_MENU,RGB(255,255,255));
////    m_BtnMenu.SetFlat(FALSE);

////    m_BtnSetting.SubclassDlgItem(IDC_BTN_SETTING,this);
////    m_BtnSetting.SetBitmaps(IDB_SETTING,RGB(255,255,255));
////    m_BtnSetting.SetFlat(FALSE);

////    m_BtnWorkMode.SubclassDlgItem(IDC_BTN_WORK_SELECT,this);
////    m_BtnWorkMode.SetFlat(FALSE);

//    //软件认证``````
////    CString strCom(_T(""));
////    int nCom = m_Config.GetUInt(_T("SerialPort"), _T("PortNumber"), 5);
////    strCom.Format(_T("COM%d"), nCom);
////    m_hCommHandle = ::CreateFile(strCom, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
////    if (INVALID_HANDLE_VALUE != m_hCommHandle)
////    {
////        //m_bAuthorize = IsAuthorized(m_hCommHandle);      //  调试时，屏蔽不进行软件认证····
////        ::CloseHandle(m_hCommHandle);
////        m_hCommHandle = INVALID_HANDLE_VALUE;
////    }
//    m_bAuthorize = TRUE;                                   //  且m_bAuthorize为TRUE  ``````````````````````````````````


//    // 初始化
//    InitialPortPath();

//    InitialPortFrame();

//    InitialCurrentWorkMode();

////    m_ProgressCtrl.ShowPercent(TRUE);
////    m_ProgressCtrl.SetRange32(0,100);

//    InitialStatisticInfo();
//    InitialSerialPort();

//    m_Command.Init(&m_SerialPort,m_hLogFile);

//    pthread_t pthInitialMachine;
//    pthread_create(&pthInitialMachine,NULL,InitialMachineThreadProc,this);

//    UpdatePortFrame(TRUE);

//    //如果是NGFF不用实时监控
//    if (m_nMachineType != MT_NGFF)
//    {

////        m_ThreadListen = AfxBeginThread((AFX_THREADPROC)EnumDeviceThreadProc,this,0,CREATE_SUSPENDED);

////        if (m_ThreadListen != NULL)
////        {
////            m_ThreadListen->m_bAutoDelete = FALSE;

////                m_nResumeThread = 1;
////            m_ThreadListen->ResumeThread();
////        }
//    }

//    // 本地目录是否存在···
//    CString strPathini(_T(""));
//    strPathini = m_Config.GetString(_T("FilePath"), _T("filepath"));
//    if (strPathini.IsEmpty())
//    {
//        strPathini = m_Config.GetString(_T("ImagePath"), _T("ImagePath"));
//        strPathini = strPathini.Left(strPathini.ReverseFind(_T('\\')) + 1) + _T("File");
//        m_Config.WriteString(_T("FilePath"), _T("filepath"), strPathini.c_str());
//        if (!PathFileExists(strPathini))
//        {
////            SHCreateDirectory(NULL, strPathini);
//            ::mkdir(strPath.c_str(),0777);
//        }
//    }


////    SetTimer(TIMER_MASTER_EXIST, 500, NULL);
////    SetTimer(TIMER_AUTHORIZE, 500, NULL);

//    return TRUE;  // return TRUE  unless you set the focus to a control

//}





void MainWindow::release()
{
    delete ui;
}

void *MainWindow::makeImageThread()
{
    MakeImageDisk makeDiskImage;
    makeDiskImage.make();
}

//void MainWindow::InitialPortFrame()
//{

//}

//void MainWindow::UpdatePortFrame(BOOL bStart)
//{

//}

//void MainWindow::InitialPortPath()
//{

//}

//void MainWindow::InitialSerialPort()
//{

//}

//void MainWindow::ResetPortInfo()
//{

//}

//void MainWindow::InitialCurrentWorkMode()
//{

//}

//void MainWindow::InitialStatisticInfo()
//{

//}

//void MainWindow::UpdateStatisticInfo()
//{

//}

//void MainWindow::InitialBurnInTest(UINT cycle, UINT nCycleCount)
//{

//}

//DWORD MainWindow::CreateSocketConnect()
//{

//}

//BOOL MainWindow::SyncTime()
//{
//    return FALSE;
//}

//void MainWindow::EnableControls(BOOL bEnable)
//{

//}

//BOOL MainWindow::IsReady()
//{
//    return TRUE;
//}

//void MainWindow::OnStart()
//{

//}

void *MainWindow::StartThreadProc(LPVOID lpParm)
{

}

void *MainWindow::StopThreadProc(LPVOID lpParm)
{

}

void *MainWindow::InitialMachineThreadProc(LPVOID lpParm)
{

}

void *MainWindow::EnumDeviceThreadProc(LPVOID lpParm)
{

}

void *MainWindow::BurnInTestThreadProc(LPVOID lpParm)
{

}

void *MainWindow::ConnectSocketThreadProc(LPVOID lpParm)
{

}

void *MainWindow::StartAsyncThreadProc(LPVOID lpParm)
{

}
