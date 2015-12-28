#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "config.h"
#include "Disk/makeimagedisk.h"

#include "common/PrintMacros.h"

#include <QFile>

#include <stdio.h>
#include <assert.h>

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

    initialConfig();

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
    DEBUG("work mode = "<<g_pGcfgMainWindow->workMode());
    switch(g_pGcfgMainWindow->workMode())
    {
    case WorkMode_ImageMake:
    {
        std::thread mkImageThd(makeImageThread);
        mkImageThd.detach();
        break;
    }
    case WorkMode_ImageCopy:
    {
        std::thread cpImageThd(copyImageThread);
        cpImageThd.detach();
        break;
    }
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
    //    m_bBurnInTest = FALSE;
    m_bStart = FALSE;
    m_bLisence = FALSE;
    m_bSockeConnected = FALSE;
    //    m_bIsMTP = FALSE;
    m_nMachineType = MT_NGFF;
    m_bEnableButton = TRUE;
    m_bFileCopyFlag = FALSE;
    //    m_pFilecopySet = NULL;
    m_bMasterExit = FALSE;
    m_bAuthorize = FALSE;
    m_pSinglePort = NULL;

    m_nResumeThread = 0;
    m_strMachineMode = _T("");


    connect(&m_timerUpdateStatistic,SIGNAL(timeout()),this,SLOT(slotOnTimerUpdateStatistic()));
    slotGlobalParamSettingChange();

}

BOOL MainWindow::initialConfig()
{
    CString strPath = CUtils::GetAppPath();
    m_strAppPath = CUtils::GetFilePathWithoutName(strPath);

    CString strLogFile = m_strAppPath + LOG_FILE;
    m_hLogFile = open(strLogFile.c_str(),O_RDWR|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO);

    if (m_hLogFile < 0)
    {
        DEBUG(_T("Create Log File Error"));
        assert(m_hLogFile>=0);
    }


    // 如果log文件大小超过2M，备份一下
    ULONGLONG ullFillSize = 0;
    Disk::GetFileSize(m_hLogFile,ullFillSize);
    if (ullFillSize > 2 * 1024 * 1024)
    {
        BackupLogfile(m_hLogFile,(DWORD)ullFillSize);
        ::close(m_hLogFile);
        DeleteFile(strLogFile);


        m_hLogFile = open(strLogFile.c_str(),O_RDWR|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO);

        assert(m_hLogFile>=0);

    }

    //添加状态栏
    CString strVersion,strSN,strModel,strAlias;

    //    CFileStatus status;
    //    CFile::GetStatus(strPath,status);
    //    struct stat st;
    //    if(stat(strPath.c_str(),&st)< 0)
    //    {

    //    }

    CUtils::WriteLogFile(m_hLogFile,FALSE,_T(""));
    CUtils::WriteLogFile(m_hLogFile,FALSE,_T("********************************************************************************"));
    //    CUtils::WriteLogFile(m_hLogFile,TRUE,_T("PE1051 Ver:%s BLD:%s"),CUtils::GetAppVersion(strPath),status.m_mtime.Format(_T("%Y-%m-%d")));

    // 读机器信息
    m_Config.SetPathName(m_strAppPath + MACHINE_INFO);
    //    strVersion.Format(_T("Ver:%s  BLD:%s "),CUtils::GetAppVersion(strPath),status.m_mtime.Format(_T("%Y-%m-%d")));
    strSN.Format(_T("%s"),m_Config.GetString(_T("MachineInfo"),_T("SN")).c_str());
    strModel.Format(_T("%s"),m_Config.GetString(_T("MachineInfo"),_T("Model")).c_str());
    strAlias.Format(_T("%s"),m_Config.GetString(_T("MachineInfo"),_T("Alias"),_T("PHIYO")).c_str());
    m_strMachineMode = strModel;

    // 判断是USB接口还是TF/SD接口
    if (strModel.Find(_T("TS")) != -1)
    {
        m_nMachineType = MT_TS;
    }
    else if (strModel.Find(_T("USB")) != -1)
    {
        m_nMachineType = MT_USB;
    }
    else if (strModel.Find(_T("NGFF")) != -1)
    {
        m_nMachineType = MT_NGFF;
    }
    else if (strModel.Find(_T("CF")) !=  -1)
    {
        m_nMachineType = MT_CF;
    }

    // 把配置文件设置回USBCopy.ini
    m_strConfigPath = m_strAppPath + CONFIG_NAME;
    m_Config.SetPathName(m_strConfigPath);

    // 获取最近加载的配置
    CString strRecentConfig = m_Config.GetString(_T("AppSetting"),_T("RecentConfig"));

    bool bFileExist = PathFileExists(strRecentConfig);
    if (!strRecentConfig.IsEmpty() && bFileExist)
    {
        m_Config.SetPathName(strRecentConfig);
        m_strConfigPath = strRecentConfig;
    }

    m_Config.WriteString(_T("Option"),_T("MachineAlias"),strAlias.c_str());

    int cx = m_Config.GetInt(_T("AppSetting"),_T("Width"),DEFAULT_WIDTH);
    int cy = m_Config.GetInt(_T("AppSetting"),_T("Height"),DEFAULT_HEIGHT);

    if (cx == 0 || cy == 0)
    {
        cx = DEFAULT_WIDTH;
        cy = DEFAULT_HEIGHT;
    }



    switch (m_nMachineType)
    {
    case MT_TS:
        //        SetDlgItemText(IDC_TEXT_LOGO,LOGO_TS);
        break;

    case MT_USB:
        //        SetDlgItemText(IDC_TEXT_LOGO,LOGO_USB);
        break;

    case MT_NGFF:
        //        SetDlgItemText(IDC_TEXT_LOGO,LOGO_NGFF);
        break;
    case MT_CF:
        //        SetDlgItemText(IDC_TEXT_LOGO,LOGO_CF);
        break;
    }

    //    BOOL bShowCursor = m_Config.GetBool(_T("Option"),_T("ShowCursor"),TRUE);
    //    ShowCursor(bShowCursor);

    // 初始化套接字库
    //    WSADATA wsd;
    //    if (WSAStartup(MAKEWORD(2,2),&wsd) != 0)
    //    {
    //        DWORD dwErrorCode  = WSAGetLastError();
    //        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("WSAStartup() failed with system error code:%d,%s")
    //            ,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
    //    }
    //    else
    //    {
    //        // 连接网络，同步时间
    //        pthread_t pthConnectSocket;
    //        pthread_create(&pthConnectSocket,NULL,ConnectSocketThreadProc,this);
    //    }

    m_bLock = m_Config.GetBool(_T("Option"),_T("En_Lock"),FALSE);
    m_strPassWord = m_Config.GetString(_T("Option"),_T("Password"),_T("123456"));

    // Button
    //    m_BtnStart.SubclassDlgItem(IDC_BTN_START,this);
    //    m_BtnStart.SetBitmaps(IDB_START,RGB(255,255,255));
    //    m_BtnStart.DrawBorder(FALSE);
    //    SetDlgItemText(IDC_BTN_START,_T(""));

    //    m_BtnLock.SubclassDlgItem(IDC_BTN_LOCK,this);
    //    m_BtnLock.SetBitmaps(IDB_LOCK,RGB(255,255,255));
    //    m_BtnLock.SetFlat(FALSE);

    if (m_bLock)
    {
        //        EnableControls(FALSE);

        CString strLock;
        //        strLock.LoadString(IDS_UNLOCK);
        //        SetDlgItemText(IDC_BTN_LOCK,strLock);
        //        m_BtnLock.SetBitmaps(IDB_UNLOCK,RGB(255,255,255));
    }

    //    m_BtnMenu.SubclassDlgItem(IDC_BTN_SYSTEM,this);
    //    m_BtnMenu.SetBitmaps(IDB_MENU,RGB(255,255,255));
    //    m_BtnMenu.SetFlat(FALSE);

    //    m_BtnSetting.SubclassDlgItem(IDC_BTN_SETTING,this);
    //    m_BtnSetting.SetBitmaps(IDB_SETTING,RGB(255,255,255));
    //    m_BtnSetting.SetFlat(FALSE);

    //    m_BtnWorkMode.SubclassDlgItem(IDC_BTN_WORK_SELECT,this);
    //    m_BtnWorkMode.SetFlat(FALSE);

    //软件认证``````
    //    CString strCom(_T(""));
    //    int nCom = m_Config.GetUInt(_T("SerialPort"), _T("PortNumber"), 5);
    //    strCom.Format(_T("COM%d"), nCom);
    //    m_hCommHandle = ::CreateFile(strCom, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
    //    if (INVALID_HANDLE_VALUE != m_hCommHandle)
    //    {
    //        //m_bAuthorize = IsAuthorized(m_hCommHandle);      //  调试时，屏蔽不进行软件认证····
    //        ::CloseHandle(m_hCommHandle);
    //        m_hCommHandle = INVALID_HANDLE_VALUE;
    //    }
    m_bAuthorize = TRUE;                                   //  且m_bAuthorize为TRUE  ``````````````````````````````````


    // 初始化
    InitialPortPath();

    InitialPortFrame();

    //    InitialCurrentWorkMode();

    ui->prgBarPro->setRange(0,100);
    ui->prgBarPro->setTextVisible(TRUE);
    //    m_ProgressCtrl.ShowPercent(TRUE);
    //    m_ProgressCtrl.SetRange32(0,100);

    //    InitialStatisticInfo();
    InitialSerialPort();

    m_Command.Init(&m_SerialPort,m_hLogFile);

    pthread_t pthInitialMachine;
    pthread_create(&pthInitialMachine,NULL,InitialMachineThreadProc,this);

    //    UpdatePortFrame(TRUE);

    //如果是NGFF不用实时监控
    if (m_nMachineType != MT_NGFF)
    {

        //        m_ThreadListen = AfxBeginThread((AFX_THREADPROC)EnumDeviceThreadProc,this,0,CREATE_SUSPENDED);

        //        if (m_ThreadListen != NULL)
        //        {
        //            m_ThreadListen->m_bAutoDelete = FALSE;

        //                m_nResumeThread = 1;
        //            m_ThreadListen->ResumeThread();
        //        }
    }

    // 本地目录是否存在···
    CString strPathini(_T(""));
    strPathini = m_Config.GetString(_T("FilePath"), _T("filepath"));
    if (strPathini.IsEmpty())
    {
        strPathini = m_Config.GetString(_T("ImagePath"), _T("ImagePath"));
        strPathini = strPathini.Left(strPathini.ReverseFind(_T('\\')) + 1) + _T("File");
        m_Config.WriteString(_T("FilePath"), _T("filepath"), strPathini.c_str());
        if (!PathFileExists(strPathini))
        {
            //            SHCreateDirectory(NULL, strPathini);
            ::mkdir(strPath.c_str(),0777);
        }
    }


    //    SetTimer(TIMER_MASTER_EXIST, 500, NULL);
    //    SetTimer(TIMER_AUTHORIZE, 500, NULL);

    return TRUE;  // return TRUE  unless you set the focus to a control

}





void MainWindow::release()
{
    delete ui;
}

void *MainWindow::makeImageThread()
{
    MakeImageDisk makeDiskImage;
    makeDiskImage.OnMakeImage();
    return NULL;
}

void *MainWindow::copyImageThread()
{
    MakeImageDisk makeDiskImage;
    makeDiskImage.OnCopyImage();
    return NULL;
}

void MainWindow::slotOnTimerUpdateStatistic()
{

}

void MainWindow::InitialPortFrame()
{
    m_pPortFrames = new PortDlg[m_nPortNum];

    //    QRect rect = ui->horiLyM1->geometry();
    //    QSize size = ui->horiLyM1->sizeHint();
    //    QSize size = ui->horiLyM1->maximumSize();
    //    QSize size = ui->horiLyM1->widget()->sizeHint();
    //    QSize size = ui->horiLyA1->widget()->size();
    //    std::cout<<"height = "<<size.height()<<" width = "<<size.width()<<std::endl;
    //    m_pPortFrames[0].setFixedHeight(rect.height());
    //    m_pPortFrames[0].setFixedWidth(rect.width());
    //    m_pPortFrames[0].setFixedSize(59,79);

    ui->horiLyM1->addWidget(&m_pPortFrames[0]);
    //    m_pPortFrames[0].adjustSize();
    //    m_pPortFrames[0].show();
    // 母盘
    m_pPortFrames[0].SetPort(&m_Config,m_hLogFile,&m_Command,&m_MasterPort,&m_TargetPorts,m_nMachineType);
    m_pPortFrames[0].SetModel(m_strMachineMode);

    //    m_pPortFrames[0].Create(nIDTemplate,this);

    //    m_pPortFrames[0].MoveWindow(rectFrame.left + 7,
    //        rectFrame.top + 1,
    //        nWidth - 10,
    //        nHeight);


    // 子盘
    PortList::const_iterator itPort = m_TargetPorts.begin();
    PortList::const_iterator itPort_end = m_TargetPorts.end();

    int nItem = 1;
    while (itPort != itPort_end)
    {
        CPort *port = (*itPort);
        itPort++;

        m_pPortFrames[nItem].SetPort(&m_Config,m_hLogFile,&m_Command,port,&m_TargetPorts,m_nMachineType);
        m_pPortFrames[nItem].SetModel(m_strMachineMode);
        if(1 == nItem)
        {
            ui->horiLyA1->addWidget(&m_pPortFrames[nItem]);
        }
        else if(2 == nItem)
        {
            ui->horiLyA2->addWidget(&m_pPortFrames[nItem]);
        }
        else if(3 == nItem)
        {
            ui->horiLyA3->addWidget(&m_pPortFrames[nItem]);
        }
        else if(4 == nItem)
        {
            ui->horiLyA4->addWidget(&m_pPortFrames[nItem]);
        }
        else if(5 == nItem)
        {
            ui->horiLyA5->addWidget(&m_pPortFrames[nItem]);
        }
        else
        {
            ERROR("invalid target item!");
            abort();
        }

        nItem++;
    }
}

void MainWindow::ResetPortFrame()
{
    for (UINT i = 0; i < m_nPortNum;i++)
    {
        m_pPortFrames[i].Initial();
    }
}

void MainWindow::UpdatePortFrame(BOOL bStart)
{
    for (UINT i = 0; i < m_nPortNum; i++)
    {
        m_pPortFrames[i].Update(bStart);
    }
}

void MainWindow::EnableKickOff(BOOL bEnable)
{
    for (UINT i = 0;i < m_nPortNum;i++)
    {
        m_pPortFrames[i].EnableKickOff(bEnable);
    }
}

void MainWindow::InitialPortPath()
{
    // 清空List
    m_TargetPorts.clear();

    // Master Port
    CString strPath1,strPath2;
    m_MasterPort.SetPortNum(0);
    m_MasterPort.SetPortType(PortType_MASTER_DISK);
    strPath1 = m_Config.GetString(_T("MasterDrives"),_T("M1-1"));
    strPath2 = m_Config.GetString(_T("MasterDrives"),_T("M1-2"));

    m_MasterPort.SetPath1AndIndex(strPath1);

    m_MasterPort.SetPath2AndIndex(strPath2);

    // Target Port
    int nNumOfTargetDrives = m_Config.GetInt(_T("TargetDrives"),_T("NumOfTargetDrives"),0);
    m_nPortNum = nNumOfTargetDrives + 1;

    for (int i = 1;i <= nNumOfTargetDrives;i++)
    {
        CPort *port = new CPort;
        port->SetPortNum(i);
        port->SetPortType(PortType_TARGET_DISK);

        CString strKey1,strKey2;
        strKey1.Format(_T("T%d-1"),i);
        strKey2.Format(_T("T%d-2"),i);

        strPath1 = m_Config.GetString(_T("TargetDrives"),strKey1);
        strPath2 = m_Config.GetString(_T("TargetDrives"),strKey2);

        port->SetPath1AndIndex(strPath1);

        port->SetPath2AndIndex(strPath2);

        m_TargetPorts.push_back(port);
    }
}

void MainWindow::InitialSerialPort()
{
    CString strSerialPortName = m_Config.GetString(_T("SerialPort"),_T("PortName"),"/dev/ttyUSB0") ;
    //    DWORD dwPortNum = m_Config.GetUInt(_T("SerialPort"),_T("PortNumber"),1);
    DWORD dwBaudRate = m_Config.GetUInt(_T("SerialPort"),_T("BaudRate"),9600);
    BYTE byDataBits = (BYTE)m_Config.GetUInt(_T("SerialPort"),_T("DataBits"),8);
    double byStopBits = m_Config.GetDouble(_T("SerialPort"),_T("StopBits"),1);

    QSerialPort::StopBits stBt;
    if(0.9 <= byStopBits &&byStopBits <1.3)
    {
        stBt = QSerialPort::OneStop;
    }
    else if(1.3<=byStopBits&&byStopBits<1.7)
    {
        stBt = QSerialPort::OneAndHalfStop;
    }
    else if(1.7 <= byStopBits &&byStopBits<2.1)
    {
        stBt = QSerialPort::TwoStop;
    }
    else
    {
        stBt = QSerialPort::UnknownStopBits;
    }


    m_SerialPort.setPortName(strSerialPortName.c_str());
    m_SerialPort.setBaudRate(dwBaudRate);
    m_SerialPort.setDataBits((QSerialPort::DataBits)byDataBits);
    m_SerialPort.setParity(QSerialPort::NoParity);
    m_SerialPort.setStopBits(stBt);
    m_SerialPort.setFlowControl(QSerialPort::NoFlowControl);


    //    BOOL bSerialOpen = m_SerialPort.open(QIODevice::ReadWrite);
    //    if (!bSerialOpen) {
    //        DEBUG("Open SerialPort error!");
    //    } else {
    //        DEBUG("Open SerialPort successful!");
    //    }


    CString strSerialPort;
    strSerialPort.Format(_T("%ld,n,%d,%d"),dwBaudRate,byDataBits,byStopBits);

    CString strLog;
    strLog.Format(_T("[SerialPort] %s,%s"),strSerialPortName.c_str(),strSerialPort.c_str());
    CUtils::WriteLogFile(m_hLogFile,TRUE,strLog.c_str());



    if (!m_SerialPort.open(QIODevice::ReadWrite))
    {
        QString strSerialPortErr = m_SerialPort.errorString();
        strLog.Format("[SerialPort] Open %s failed,system error=%s",strSerialPortName.c_str(),strSerialPortErr.toStdString().c_str());
        CUtils::WriteLogFile(m_hLogFile,TRUE,strLog.c_str());
        ERROR("Open serial port error!");
    }
    else
    {
        strLog.Format(_T("[SerialPort] Open %s success."),strSerialPortName.c_str());
        CUtils::WriteLogFile(m_hLogFile,TRUE,strLog.c_str());
    }
}

void MainWindow::ResetPortInfo()
{
    m_MasterPort.Initial();
    m_FilePort.Initial();

    PortList::const_iterator itPort = m_TargetPorts.begin();
    PortList::const_iterator itPort_end = m_TargetPorts.end();
    while (itPort != itPort_end)
    {
        CPort *port = (*itPort);
        itPort++;
        port->Initial();
    }
}

void MainWindow::InitialCurrentWorkMode()
{
    m_WorkMode = (WorkMode)m_Config.GetUInt(_T("Option"),_T("FunctionMode"),1);

    CString strWorkMode,strWorkModeParm;
    CString strResText1,strResText2,strResText3;
    //    UINT nBitMap = IDB_QUICK_COPY;
    m_bIsMTP = FALSE;
    switch (m_WorkMode)
    {
    case WorkMode_FullCopy:
    {
        //            nBitMap = IDB_FULL_COPY;
        //            strResText1.LoadString(IDS_WORK_MODE_FULL_COPY);
        strResText1 = "FullCopy";

        strWorkMode = strWorkModeParm = strResText1;

        if (m_Config.GetBool(_T("FullCopy"),_T("En_CleanDiskFirst"),FALSE))
        {
            //                strResText3.LoadString(IDS_WORK_MODE_DISK_CLEAN);
            strResText3 = "DiskClean";

            strWorkModeParm = strResText3;
            strWorkModeParm += _T(" + ");
            strWorkModeParm += strResText1;
        }

        if (m_Config.GetBool(_T("FullCopy"),_T("En_Compare"),FALSE))
        {
            //                strResText2.LoadString(IDS_COMPARE);
            strResText2 = "Compare";
            strWorkModeParm += _T(" + ");
            strWorkModeParm += strResText2;
        }
    }
        break;

    case WorkMode_QuickCopy:
    {
        //            nBitMap = IDB_QUICK_COPY;

        //            strResText1.LoadString(IDS_WORK_MODE_QUICK_COPY);
        strResText1 = "QuickCopy";
        strWorkMode = strWorkModeParm = strResText1;


        if (m_Config.GetBool(_T("QuickCopy"),_T("En_CleanDiskFirst"),FALSE))
        {
            //            strResText3.LoadString(IDS_WORK_MODE_DISK_CLEAN);
            strResText3 = "DiskClean";
            strWorkModeParm = strResText3;
            strWorkModeParm += _T(" + ");
            strWorkModeParm += strResText1;
        }

        if (m_Config.GetBool(_T("QuickCopy"),_T("En_Compare"),FALSE))
        {
            //            strResText2.LoadString(IDS_COMPARE);
            strResText2 = "Compare";
            strWorkModeParm += _T(" + ");
            strWorkModeParm += strResText2;
        }
    }
        break;

    case WorkMode_ImageCopy:
    {
        //        nBitMap = IDB_IMAGE_COPY;

        //        strResText1.LoadString(IDS_WORK_MODE_IMAGE_COPY);
        strResText1 = "ImageCopy";
        strWorkMode = strWorkModeParm = strResText1;

        if (m_Config.GetBool(_T("ImageCopy"),_T("En_Compare"),FALSE))
        {
            //            strResText2.LoadString(IDS_COMPARE);
            strResText2 = "Compare";
            strWorkModeParm.Format(_T("%s + %s"),strResText1.c_str(),strResText2.c_str());
        }

        // MTP Copy
        if (m_Config.GetUInt(_T("ImageCopy"),_T("ImageType"),0) == 1)
        {
            m_bIsMTP = TRUE;
            //            strResText3.LoadString(IDS_MTP_IMAGE);
            strResText3 = "MTPCopy";
            strWorkModeParm += _T(" - ");
            strWorkModeParm += strResText3;
        }
        else
        {
            if (m_Config.GetBool(_T("ImageCopy"),_T("En_CleanDiskFirst"),FALSE))
            {
                //                strResText3.LoadString(IDS_WORK_MODE_DISK_CLEAN);
                strResText1 = "DiskClean";

                strWorkModeParm = strResText3 + _T(" + ") + strWorkModeParm;
            }
        }
    }
        break;

    case WorkMode_ImageMake:
    {
        //        nBitMap = IDB_MAKE_IMAGE;
        //        strResText1.LoadString(IDS_WORK_MODE_IMAGE_MAKE);
        strResText1  = "ImageMake";
        strWorkMode = strWorkModeParm = strResText1;

        UINT nSaveMode = m_Config.GetUInt(_T("ImageMake"),_T("SaveMode"),0);

        switch (nSaveMode)
        {
        case 0: //Full image
            //            strResText3.LoadString(IDS_FULL_IMAGE);
            strResText3 = "FullImage";

            break;

        case 1: //Quick image
            //            strResText3.LoadString(IDS_QUICK_IMAGE);
            strResText3 = "QuickImage";
            break;

        case 2: //MTP image
            m_bIsMTP = TRUE;
            //            strResText3.LoadString(IDS_MTP_IMAGE);
            strResText3 = "MTPImage";
            break;

        }

        strWorkModeParm += _T(" - ");
        strWorkModeParm += strResText3;
    }

        break;

    case WorkMode_DiskClean:
    {
        //        nBitMap = IDB_DISK_CLEAN;
        //        strResText1.LoadString(IDS_WORK_MODE_DISK_CLEAN);
        strResText1 = "DiskClean";
        strWorkMode = strWorkModeParm = strResText1;

        CleanMode cleanMode = (CleanMode)m_Config.GetInt(_T("DiskClean"),_T("CleanMode"),0);
        switch (cleanMode)
        {
        case CleanMode_Full:
            //            strResText2.LoadString(IDS_FULL_CLEAN);
            strResText2 = "FullClean";
            break;

        case CleanMode_Quick:
            //            strResText2.LoadString(IDS_QUICK_CLEAN);
            strResText2 = "QuickClean";
            break;

        case CleanMode_Safe:
            //            strResText2.LoadString(IDS_SAFE_CLEAN);
            strResText2 = "SafeClean";
            break;
        }

        strWorkModeParm.Format(_T("%s - %s"),strResText1.c_str(),strResText2.c_str());
    }

        break;

    case WorkMode_DiskCompare:
    {
        //        nBitMap = IDB_DISK_COMPARE;
        //        strResText1.LoadString(IDS_WORK_MODE_DISK_COMPARE);
        strResText1 = "DiskCompare";
        strWorkMode = strWorkModeParm = strResText1;

        CompareMode compareMode = (CompareMode)m_Config.GetInt(_T("DiskCompare"),_T("CompareMode"),0);

        switch (compareMode)
        {
        case CompareMode_Full:
            //            strResText2.LoadString(IDS_FULL_COMPARE);
            strResText2 ="FullCompare";
            break;

        case CompareMode_Quick:
            //            strResText2.LoadString(IDS_QUICK_COMPARE);
            strResText2 = "QuickCompare";
            break;
        }

        strWorkModeParm.Format(_T("%s - %s"),strResText1.c_str(),strResText2.c_str());
    }
        break;

    case WorkMode_FileCopy:
    {
        //        nBitMap = IDB_FILE_COPY;
        //        strResText1.LoadString(IDS_WORK_MODE_FILE_COPY);
        strResText1 = "FileCopy";
        strWorkMode = strWorkModeParm = strResText1;

        if (m_Config.GetBool(_T("FileCopy"),_T("En_Compare"),FALSE))
        {
            //            strResText2.LoadString(IDS_COMPARE);
            strResText2 = "Compare";
            strWorkModeParm.Format(_T("%s + %s"),strResText1.c_str(),strResText2.c_str());
        }
    }
        break;

    case WorkMode_DiskFormat:
    {
        //        nBitMap = IDB_DISK_FORMAT;
        //        strResText1.LoadString(IDS_WORK_MODE_DISK_FORMAT);
        strResText1 = "DiskFormat";
        strWorkMode = strWorkModeParm = strResText1;
    }
        break;

    case WorkMode_DifferenceCopy:
    {
        //        nBitMap = IDB_DIFF_COPY;
        //        strResText1.LoadString(IDS_WORK_MODE_DIFF_COPY);
        strResText1 = "DifferentCopy";
        strWorkMode = strWorkModeParm = strResText1;
    }
        break;

    case WorkMode_MTPCopy:
    {
        //        nBitMap = IDB_MTP_COPY;
        //        strResText1.LoadString(IDS_WORK_MODE_MTP_COPY);
        strResText1 = "MTPCopy";
        strWorkMode = strWorkModeParm = strResText1;

        if (m_Config.GetBool(_T("MTPCopy"),_T("En_Compare"),FALSE))
        {
            //            strResText2.LoadString(IDS_COMPARE);
            strResText2 = "Compare";
            strWorkModeParm.Format(_T("%s + %s"),strResText1.c_str(),strResText2.c_str());
        }

        m_bIsMTP = TRUE;
    }
        break;

    case WorkMode_Full_RW_Test:
    {
        //        nBitMap = IDB_FULL_SCAN;
        //        strResText1.LoadString(IDS_WORK_MODE_FULL_RW_TEST);
        strResText1 = "FullRWTest";
        strWorkMode = strWorkModeParm = strResText1;
    }
        break;

    case WorkMode_Fade_Picker:
    {
        //        nBitMap = IDB_FAKE_PICKER;
        //        strResText1.LoadString(IDS_WORK_MODE_FADE_PICKER);
        strResText1 = "FadePicker";
        strWorkMode = strWorkModeParm = strResText1;
    }
        break;

    case WorkMode_Speed_Check:
    {
        //        nBitMap = IDB_SPEED_TEST;
        //        strResText1.LoadString(IDS_WORK_MODE_SPEED_CHECK);
        strResText1 = "SpeedCheck";
        strWorkMode = strWorkModeParm = strResText1;
    }
        break;

    case WorkMode_Burnin_Test:
    {
        //        nBitMap = IDB_BURN_IN;
        //        strResText1.LoadString(IDS_WORK_MODE_BURN_IN);
        strResText1 = "BurnIn";
        strWorkMode = strWorkModeParm = strResText1;
    }
        break;
    default:
        break;
    }

    //    SetDlgItemText(IDC_BTN_WORK_SELECT,strWorkMode);
    //    SetDlgItemText(IDC_GROUP_WORK_MODE,strWorkModeParm);

    //    m_BtnWorkMode.SetBitmaps(nBitMap,RGB(255,255,255));
}

void MainWindow::InitialStatisticInfo()
{
    //    SetDlgItemText(IDC_TEXT_FUNCTION2,_T(""));
    //	SetDlgItemText(IDC_TEXT_SPEED2,_T("0"));
    //	SetDlgItemText(IDC_TEXT_TIME_ELAPSED2,_T("00:00:00"));
    //	SetDlgItemText(IDC_TEXT_TIME_REMAINNING2,_T("00:00:00"));
    //	SetDlgItemText(IDC_TEXT_USAGE,_T(""));
    //	m_ProgressCtrl.SetPos(0);
    ui->prgBarPro->setValue(0);
}

void MainWindow::UpdateStatisticInfo()
{
    // 除了Make Image之外，都不计算母盘
    int iMinPercent = 100; //m_MasterPort.GetPercent();
    ULONGLONG ullMinCompleteSize = -1; //m_MasterPort.GetCompleteSize();
    ULONGLONG ullMaxValidSize = 0; //m_MasterPort.GetValidSize();

    int nIndex = 0;

    if (m_WorkMode == WorkMode_DiskCompare)
    {
        iMinPercent = m_MasterPort.GetPercent();
        ullMinCompleteSize = m_MasterPort.GetCompleteSize();
        ullMaxValidSize = m_MasterPort.GetValidSize();
        nIndex++;
    }

    // 以下几种模式不做速度比分比和剩余时间统计
    if (m_WorkMode == WorkMode_Speed_Check)
    {
        //        CTime time = CTime::GetCurrentTime();
        //        CTimeSpan spanTotal = time - m_StartTime;
        //        SetDlgItemText(IDC_TEXT_TIME_ELAPSED2,spanTotal.Format(_T("%H:%M:%S")));

        return;
    }


    if (m_WorkMode != WorkMode_ImageMake)
    {
        // 拷贝过程中
        //        POSITION pos = m_TargetPorts.GetHeadPosition();
        PortList::const_iterator itPort = m_TargetPorts.begin();
        PortList::const_iterator itPort_end = m_TargetPorts.end();

        while (itPort != itPort_end)
        {
            CPort *port = (*itPort);
            itPort++;
            if (port->IsConnected())
            {
                if (port->GetResult() && port->GetPortState() == PortState_Active)
                {
                    if (port->GetPercent() < iMinPercent)
                    {
                        iMinPercent = port->GetPercent();
                    }

                    if (port->GetCompleteSize() < ullMinCompleteSize)
                    {
                        ullMinCompleteSize = port->GetCompleteSize();
                    }

                    if (port->GetValidSize() > ullMaxValidSize)
                    {
                        ullMaxValidSize = port->GetValidSize();
                    }

                    nIndex++;
                }

            }
        }

        // 全部完成
        if (nIndex == 0)
        {
            //            pos = m_TargetPorts.GetHeadPosition();
            itPort = m_TargetPorts.begin();
            itPort_end = m_TargetPorts.end();

            while (itPort != itPort_end)
            {
                CPort *port = (*itPort);
                itPort++;
                if (port->IsConnected())
                {
                    if (port->GetResult() && port->GetPortState() != PortState_Active)
                    {
                        if (port->GetPercent() < iMinPercent)
                        {
                            iMinPercent = port->GetPercent();
                        }

                        if (port->GetValidSize() > ullMaxValidSize)
                        {
                            ullMaxValidSize = port->GetValidSize();
                            ullMinCompleteSize = ullMaxValidSize;
                        }

                        nIndex++;
                    }
                }
            }
        }

    }
    else
    {
        if (m_FilePort.GetPercent() < iMinPercent)
        {
            iMinPercent = m_FilePort.GetPercent();
        }

        if (m_FilePort.GetCompleteSize() < ullMinCompleteSize)
        {
            ullMinCompleteSize = m_FilePort.GetCompleteSize();
        }

        if (m_FilePort.GetValidSize() > ullMaxValidSize)
        {
            ullMaxValidSize = m_FilePort.GetValidSize();
        }

        nIndex++;
    }

    if (nIndex > 0)
    {
        if (ullMaxValidSize > 0)
        {
            int nPercent = (int)(ullMinCompleteSize * 100 / ullMaxValidSize);
            //            m_ProgressCtrl.SetPos(nPercent);
            ui->prgBarPro->setValue(nPercent);
        }

        CString strText;
        strText.Format(_T("%s / %s"),CUtils::AdjustFileSize(ullMinCompleteSize).c_str(),CUtils::AdjustFileSize(ullMaxValidSize).c_str());
        //        SetDlgItemText(IDC_TEXT_USAGE,strText);


        //        CTime time = CTime::GetCurrentTime();
        //        CTimeSpan spanTotal = time - m_StartTime;
        //        CTimeSpan spanTemp = time - m_StartTimeTemp;
        //        SetDlgItemText(IDC_TEXT_TIME_ELAPSED2,spanTotal.Format(_T("%H:%M:%S")));

        ULONGLONG ullCompleteSize = ullMinCompleteSize;

        ULONGLONG ullAvgSpeed = 0;

        //        if (spanTemp.GetTotalSeconds() > 0)
        //        {
        //            ullAvgSpeed = ullCompleteSize / spanTemp.GetTotalSeconds();
        //        }

        // 超过5s，使能踢盘功能
        //        if (spanTotal.GetTotalSeconds() > 5)
        //        {
        //            EnableKickOff(TRUE);
        //        }
        //        else
        //        {
        //            EnableKickOff(FALSE);
        //        }

        strText = CUtils::AdjustSpeed(ullAvgSpeed);
        //        SetDlgItemText(IDC_TEXT_SPEED2,strText);

        if (ullAvgSpeed > 0)
        {
            //            __time64_t time = (__time64_t)((ullMaxValidSize - ullMinCompleteSize)/ullAvgSpeed);
            //            CTimeSpan spanR(time);
            //            SetDlgItemText(IDC_TEXT_TIME_REMAINNING2,spanR.Format(_T("%H:%M:%S")));
        }

    }
}

void MainWindow::EnableControls(BOOL bEnable)
{
    if (m_bLock)
    {
        ui->btnSystemFunc->setEnabled(FALSE);
        ui->btnCurOperate->setEnabled(FALSE);
        ui->btnParamSetting->setEnabled(FALSE);
    }
    else
    {
        ui->btnSystemFunc->setEnabled(bEnable);
        ui->btnCurOperate->setEnabled(bEnable);
        ui->btnParamSetting->setEnabled(bEnable);
    }
}

BOOL MainWindow::IsReady()
{
    CString strWorkMode,strMsg;
    //GetDlgItemText(IDC_BTN_WORK_SELECT,strWorkMode);
    strWorkMode = GetWorkModeString(m_WorkMode);

    BOOL bRet = TRUE;

    switch (m_WorkMode)
    {
    case WorkMode_ImageCopy:
    case WorkMode_DiskClean:
    case WorkMode_DiskFormat:
    case WorkMode_Fade_Picker:
    case WorkMode_Full_RW_Test:
    case WorkMode_Speed_Check:
        if (!IsExistTarget())
        {
            strMsg.Format(_T("%s failed!\r\nCustom errorcode=0x%X, No Targets."),strWorkMode.c_str(),CustomError_No_Target);
            CUtils::WriteLogFile(m_hLogFile,TRUE,strMsg);

            bRet = FALSE;

            SetAllFailed(CustomError_No_Target,ErrorType_Custom);
        }
        break;

    case WorkMode_DifferenceCopy:
    {
        if (!IsExistTarget())
        {
            strMsg.Format(_T("%s failed!\r\nCustom errorcode=0x%X, No Targets."),strWorkMode.c_str(),CustomError_No_Target);
            CUtils::WriteLogFile(m_hLogFile,TRUE,strMsg);

            bRet = FALSE;

            SetAllFailed(CustomError_No_Target,ErrorType_Custom);
        }

        if (m_Config.GetUInt(_T("DifferenceCopy"),_T("SourceType"),0) == SourceType_Master
                || m_Config.GetUInt(_T("DifferenceCopy"),_T("PkgLocation"),0) == PathType_Local)
        {
            if (!IsExistMaster())
            {
                strMsg.Format(_T("%s failed!\r\nCustom errorcode=0x%X, No Master."),strWorkMode.c_str(),CustomError_No_Master);
                CUtils::WriteLogFile(m_hLogFile,TRUE,strMsg);

                SetAllFailed(CustomError_No_Master,ErrorType_Custom);

                bRet = FALSE;
            }
        }
    }
        break;

    case WorkMode_ImageMake:
        if (!IsExistMaster())
        {
            strMsg.Format(_T("%s failed!\r\nCustom errorcode=0x%X, No Master."),strWorkMode.c_str(),CustomError_No_Master);
            CUtils::WriteLogFile(m_hLogFile,TRUE,strMsg);

            bRet = FALSE;

            SetAllFailed(CustomError_No_Master,ErrorType_Custom);
        }
        break;

    default:
    {
        BOOL bCopyType = m_Config.GetBool(_T("FileCopy"), _T("FilecopyType"), FALSE);
        if (!(bCopyType && (m_WorkMode == WorkMode_FileCopy)))
        {
            if (!IsExistMaster())
            {
                strMsg.Format(_T("%s failed!\r\nCustom errorcode=0x%X, No Master."),strWorkMode.c_str(),CustomError_No_Master);
                CUtils::WriteLogFile(m_hLogFile,TRUE,strMsg);

                SetAllFailed(CustomError_No_Master,ErrorType_Custom);

                bRet = FALSE;
            }
        }


        if (!IsExistTarget())
        {
            strMsg.Format(_T("%s failed!\r\nCustom errorcode=0x%X, No Targets."),strWorkMode.c_str(),CustomError_No_Target);
            CUtils::WriteLogFile(m_hLogFile,TRUE,strMsg);

            SetAllFailed(CustomError_No_Target,ErrorType_Custom);

            bRet = FALSE;
        }
        break;
    }


    }

    m_strMsg = strMsg;
    m_bResult = bRet;

    return bRet;
}

BOOL MainWindow::IsExistTarget()
{
    BOOL bOK = FALSE;
    PortList::const_iterator itPort = m_TargetPorts.begin();
    PortList::const_iterator itPort_end = m_TargetPorts.end();

    while (itPort != itPort_end)
    {
        CPort *port = (*itPort);
        itPort++;
        if (port->IsConnected())
        {
            bOK = TRUE;
            break;
        }
    }

    return bOK;
}

BOOL MainWindow::IsExistMaster()
{
    return  m_MasterPort.IsConnected();
}

CString MainWindow::GetCustomErrorMsg(CustomError customError)
{
    CString strError;
    switch (customError)
    {
    case CustomError_Cancel:
        strError = _T("Cancelled by user.");
        break;

    case CustomError_No_Master:
        strError = _T("No master.");
        break;

    case CustomError_No_Target:
        strError = _T("No target.");
        break;

    case CustomError_Compare_Failed:
        strError = _T("Compare faied.");
        break;

    case CustomError_All_Targets_Failed:
        strError = _T("All targets failed.");
        break;

    case CustomError_Master_Failed:
        strError = _T("Master failed.");
        break;

    case CustomError_Image_Format_Error:
        strError = _T("Image format error.");
        break;

    case CustomError_Compress_Error:
        strError = _T("Compress error.");
        break;

    case CustomError_UnCompress_Error:
        strError = _T("Uncompress error.");
        break;

    case CustomError_Speed_Too_Slow:
        strError = _T("Speed too slow.");
        break;

    case CustomError_Unrecognized_Partition:
        strError = _T("Unrecognized partition.");
        break;

    case CustomError_No_File_Select:
        strError = _T("No file has been selected.");
        break;

    case CustomError_Target_Small:
        strError = _T("Target is small.");
        break;

    case CustomError_Format_Error:
        strError = _T("Disk format error.");
        break;

    case CustomError_Get_Data_From_Server_Error:
        strError = _T("Get data from server error.");
        break;

    case CustomError_Image_Hash_Value_Changed:
        strError = _T("Image hash value was changed.");
        break;

    case CustomError_Enum_File_Failed:
        strError = _T("Enum file failed.");
        break;

    case CustomError_MTP_OpenDevice_Failed:
        strError = _T("MTP open device failed.");
        break;

    case CustomError_MTP_EnumStorageID_Failed:
        strError = _T("MTP enum storage id failed.");
        break;

    case CustomError_MTP_CreateFolder_Failed:
        strError = _T("MTP create folder failed.");
        break;

    case CustomError_MTP_NO_ObjectID:
        strError = _T("MTP no object id.");
        break;

    case CustomError_MTP_CreateDataStream_Failed:
        strError = _T("MTP create data stream failed.");
        break;

    case CustomError_MTP_WriteFile_Failed:
        strError = _T("MTP write file failed.");
        break;

    case CustomError_MTP_ReadFile_Failed:
        strError = _T("MTP read file failed.");
        break;

    case CustomError_ReadSpeed_Slow:
        strError = _T("Read speed slow.");
        break;

    case CustomError_WriteSpeed_Slow:
        strError = _T("Write speed slow.");
        break;

    case CustomError_Master_Target_Size_Not_Same:
        strError = _T("Not same size.");
        break;
    default:
        break;
    }

    return strError;
}

CString MainWindow::GetWorkModeString(WorkMode mode)
{
    CString strWorkMode;
    switch (mode)
    {
    case WorkMode_FullCopy:
        strWorkMode = _T("FULL COPY");
        break;
    case WorkMode_QuickCopy:
        strWorkMode = _T("QUICK COPY");
        break;
    case WorkMode_FileCopy:
        strWorkMode = _T("FILE COPY");
        break;
    case WorkMode_ImageCopy:
        strWorkMode = _T("IMAGE COPY");
        break;
    case WorkMode_ImageMake:
        strWorkMode = _T("IMAGE MAKE");
        break;
    case WorkMode_DiskClean:
        strWorkMode = _T("DISK CLEAN");
        break;

    case WorkMode_DiskCompare:
        strWorkMode = _T("DISK COMPARE");
        break;

    case WorkMode_DiskFormat:
        strWorkMode = _T("DISK FORMAT");
        break;

    case WorkMode_DifferenceCopy:
        strWorkMode = _T("DIFF. COPY");
        break;

    case WorkMode_MTPCopy:
        strWorkMode = _T("MTP COPY");
        break;

    case WorkMode_Full_RW_Test:
        strWorkMode = _T("FULL RW TEST");
        break;

    case WorkMode_Fade_Picker:
        strWorkMode = _T("FADE PICKER");
        break;

    case WorkMode_Speed_Check:
        strWorkMode = _T("SPEED CHECK");
        break;
    default:
        break;
    }

    return strWorkMode;
}

void MainWindow::OnStart()
{
    if (m_bBurnInTest)
    {
//        m_StartTimeTemp = CTime::GetCurrentTime();
    }
    else
    {
//        m_StartTimeTemp = m_StartTime = CTime::GetCurrentTime();
    }

    // 开始线程
    SetTimer(TIMER_UPDATE_STATISTIC,1000,NULL);

    HashMethod hashMethod = (HashMethod)m_Config.GetInt(_T("Option"),_T("HashMethod"),0);
    UINT nBlockSectors = m_Config.GetUInt(_T("Option"),_T("BlockSectors"),256); // 默认128KB
    BOOL bMustSameCapacity = m_Config.GetBool(_T("Option"),_T("En_SameCapacity"),FALSE);

    CDisk disk;
    disk.Init(m_hWnd,&m_bCancel,m_hLogFile,&m_Command,nBlockSectors);
    disk.SetWorkMode(m_WorkMode);
    disk.SetGlobalParm(bMustSameCapacity);

    CString strMsg,strWorkMode;
    BOOL bResult = TRUE;

    strWorkMode = GetWorkModeString(m_WorkMode);

    switch(m_WorkMode)
    {
    case WorkMode_FullCopy:
        {
            BOOL bComputeHash = m_Config.GetBool(_T("FullCopy"),_T("En_ComputeHash"),FALSE);
            BOOL bCompare = m_Config.GetBool(_T("FullCopy"),_T("En_Compare"),FALSE);
            CompareMethod compareMothod = (CompareMethod)m_Config.GetInt(_T("FullCopy"),_T("CompareMethod"),0);

            BOOL bAllowCapGap = m_Config.GetBool(_T("FullCopy"),_T("En_AllowCapaGap"),FALSE);
            UINT nPercent = m_Config.GetUInt(_T("FullCopy"),_T("CapacityGap"),0);
            BOOL bCleanDiskFirst = m_Config.GetBool(_T("FullCopy"),_T("En_CleanDiskFirst"),FALSE);
            BOOL bCompareClean = m_Config.GetBool(_T("FullCopy"),_T("En_CompareClean"),FALSE);
            UINT nCleanTimes = m_Config.GetUInt(_T("FullCopy"),_T("CleanTimes"),1);
            CString strFillValues = m_Config.GetString(_T("FullCopy"),_T("FillValues"));
            CompareCleanSeq seq = (CompareCleanSeq)m_Config.GetInt(_T("FullCopy"),_T("CompareCleanSeq"),0);

            if (nCleanTimes < 0 || nCleanTimes > 3)
            {
                nCleanTimes = 1;
            }

            int *pCleanFillValues = new int[nCleanTimes];
            memset(pCleanFillValues,0,nCleanTimes * sizeof(int));

            if (bCleanDiskFirst)
            {
                int nCurPos = 0;

                CString strToken;
                UINT nCount = 0;

                strToken = strFillValues.Tokenize(_T(";"),nCurPos);
                while (nCurPos != -1 && nCount < nCleanTimes)
                {
                    int nFillValue = _tcstoul(strToken, NULL, 16);

                    if (nFillValue == 0 && strToken.GetAt(0) != _T('0'))
                    {
                        nFillValue = RANDOM_VALUE;
                    }

                    pCleanFillValues[nCount++] = nFillValue;

                    strToken = strFillValues.Tokenize(_T(";"),nCurPos);
                }
            }

            // 设置端口状态
            m_MasterPort.SetHashMethod(hashMethod);
            m_MasterPort.SetWorkMode(m_WorkMode);
            m_MasterPort.SetStartTime(m_StartTime);

            POSITION pos = m_TargetPorts.GetHeadPosition();
            while (pos)
            {
                CPort *port = m_TargetPorts.GetNext(pos);
                if (port->IsConnected())
                {
                    port->SetHashMethod(hashMethod);
                    port->SetWorkMode(m_WorkMode);
                    port->SetStartTime(m_StartTime);
                }
            }

            disk.SetMasterPort(&m_MasterPort);
            disk.SetTargetPorts(&m_TargetPorts);
            disk.SetHashMethod(bComputeHash,hashMethod);
            disk.SetCompareParm(bCompare,compareMothod);
            disk.SetFullCopyParm(bAllowCapGap,nPercent);
            disk.SetCleanDiskFirst(bCleanDiskFirst,bCompareClean,seq,nCleanTimes,pCleanFillValues);
            delete []pCleanFillValues;

            bResult = disk.Start();

            CString strHashValue;
            if (bResult)
            {
                if (bComputeHash)
                {
                    int len = m_MasterPort.GetHashLength();
                    BYTE *pHash = new BYTE[len];
                    ZeroMemory(pHash,len);
                    m_MasterPort.GetHash(pHash,len);
                    for (int i = 0;i < len;i++)
                    {
                        CString strHash;
                        strHash.Format(_T("%02X"),pHash[i]);

                        strHashValue += strHash;
                    }
                    delete []pHash;
                    strMsg.Format(_T("%s Completed !\r\nHashMethod=%s, HashValue=%s")
                        ,strWorkMode,m_MasterPort.GetHashMethodString(),strHashValue);
                }
                else
                {
                    strMsg.Format(_T("%s Completed !!!"),strWorkMode);
                }
            }
            else
            {
                ErrorType errType = ErrorType_System;
                DWORD dwErrorCode = 0;
                errType = m_MasterPort.GetErrorCode(&dwErrorCode);

                if (dwErrorCode == 0)
                {
                    // 任意取一个错误
                    pos = m_TargetPorts.GetHeadPosition();
                    while (pos)
                    {
                        CPort *port = m_TargetPorts.GetNext(pos);

                        if (port->IsConnected() && !port->GetResult())
                        {
                            errType = port->GetErrorCode(&dwErrorCode);
                            break;
                        }
                    }
                }

                if (errType == ErrorType_System)
                {
                    strMsg.Format(_T("%s Failed !\r\nSystem errorCode=%d,%s")
                        ,strWorkMode,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
                }
                else
                {
                    strMsg.Format(_T("%s Failed !\r\nCustom errorCode=0x%X,%s")
                        ,strWorkMode,dwErrorCode,GetCustomErrorMsg((CustomError)dwErrorCode));
                }

            }

            CUtils::WriteLogFile(m_hLogFile,TRUE,strMsg);
        }

        break;
    case WorkMode_QuickCopy:
        {
            BOOL bComputeHash = m_Config.GetBool(_T("QuickCopy"),_T("En_ComputeHash"),FALSE);
            BOOL bCompare = m_Config.GetBool(_T("QuickCopy"),_T("En_Compare"),FALSE);
            CompareMethod compareMethod = (CompareMethod)m_Config.GetInt(_T("QuickCopy"),_T("CompareMethod"),0);

            BOOL bCleanDiskFirst = m_Config.GetBool(_T("QuickCopy"),_T("En_CleanDiskFirst"),FALSE);
            BOOL bCompareClean = m_Config.GetBool(_T("QuickCopy"),_T("En_CompareClean"),FALSE);
            UINT nCleanTimes = m_Config.GetUInt(_T("QuickCopy"),_T("CleanTimes"),1);
            CString strFillValues = m_Config.GetString(_T("QuickCopy"),_T("FillValues"));
            CompareCleanSeq seq = (CompareCleanSeq)m_Config.GetInt(_T("QuickCopy"),_T("CompareCleanSeq"),0);

            int nNumOfArea = m_Config.GetInt(_T("QuickCopy"),_T("NumOfArea"),0);

            PRANGE_FROM_TO ranges = NULL;

            if (nNumOfArea > 0)
            {
                ranges = new RANGE_FROM_TO[nNumOfArea];

                CString strKey;
                ULONGLONG ullStartLBA = 0, ullEndLBA = 0;
                for (int i = 0; i < nNumOfArea;i++)
                {
                    strKey.Format(_T("StartLBA_%d"),i);

                    ullStartLBA = m_Config.GetUInt64(_T("QuickCopy"),strKey,0);

                    strKey.Format(_T("EndLBA_%d"),i);

                    ullEndLBA = m_Config.GetUInt64(_T("QuickCopy"),strKey,0);

                    ranges[i].ullStartLBA = ullStartLBA;
                    ranges[i].ullEndingLBA = ullEndLBA;
                }
            }

            if (nCleanTimes < 0 || nCleanTimes > 3)
            {
                nCleanTimes = 1;
            }

            int *pCleanFillValues = new int[nCleanTimes];
            memset(pCleanFillValues,0,nCleanTimes * sizeof(int));

            if (bCleanDiskFirst)
            {
                int nCurPos = 0;

                CString strToken;
                UINT nCount = 0;
                strToken = strFillValues.Tokenize(_T(";"),nCurPos);
                while (nCurPos != -1 && nCount < nCleanTimes)
                {
                    int nFillValue = _tcstoul(strToken, NULL, 16);

                    if (nFillValue == 0 && strToken.GetAt(0) != _T('0'))
                    {
                        nFillValue = RANDOM_VALUE;
                    }

                    pCleanFillValues[nCount++] = nFillValue;

                    strToken = strFillValues.Tokenize(_T(";"),nCurPos);
                }
            }

            // 设置端口状态
            m_MasterPort.SetHashMethod(hashMethod);
            m_MasterPort.SetWorkMode(m_WorkMode);
            m_MasterPort.SetStartTime(m_StartTime);

            POSITION pos = m_TargetPorts.GetHeadPosition();
            while (pos)
            {
                CPort *port = m_TargetPorts.GetNext(pos);
                if (port->IsConnected())
                {
                    port->SetHashMethod(hashMethod);
                    port->SetWorkMode(m_WorkMode);
                    port->SetStartTime(m_StartTime);
                }
            }

            disk.SetMasterPort(&m_MasterPort);
            disk.SetTargetPorts(&m_TargetPorts);
            disk.SetHashMethod(bComputeHash,hashMethod);
            disk.SetCompareParm(bCompare,compareMethod);
            disk.SetCleanDiskFirst(bCleanDiskFirst,bCompareClean,seq,nCleanTimes,pCleanFillValues);
            disk.SetQuickCopyParm(ranges,nNumOfArea);

            if (ranges)
            {
                delete []ranges;
            }
            delete []pCleanFillValues;

            bResult = disk.Start();

            CString strHashValue;
            if (bResult)
            {
                if (bComputeHash)
                {
                    int len = m_MasterPort.GetHashLength();
                    BYTE *pHash = new BYTE[len];
                    ZeroMemory(pHash,len);
                    m_MasterPort.GetHash(pHash,len);
                    for (int i = 0;i < len;i++)
                    {
                        CString strHash;
                        strHash.Format(_T("%02X"),pHash[i]);

                        strHashValue += strHash;
                    }
                    delete []pHash;
                    strMsg.Format(_T("%s Completed !\r\nHashMethod=%s, HashValue=%s")
                        ,strWorkMode,m_MasterPort.GetHashMethodString(),strHashValue);
                }
                else
                {
                    strMsg.Format(_T("%s Completed !!!"),strWorkMode);
                }
            }
            else
            {
                ErrorType errType = ErrorType_System;
                DWORD dwErrorCode = 0;
                errType = m_MasterPort.GetErrorCode(&dwErrorCode);

                if (dwErrorCode == 0)
                {
                    // 任意取一个错误
                    pos = m_TargetPorts.GetHeadPosition();
                    while (pos)
                    {
                        CPort *port = m_TargetPorts.GetNext(pos);

                        if (port->IsConnected() && !port->GetResult())
                        {
                            errType = port->GetErrorCode(&dwErrorCode);
                            break;
                        }
                    }
                }

                if (errType == ErrorType_System)
                {
                    strMsg.Format(_T("%s Failed !\r\nSystem errorCode=%d,%s")
                        ,strWorkMode,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
                }
                else
                {
                    strMsg.Format(_T("%s Failed !\r\nCustom errorCode=0x%X,%s")
                        ,strWorkMode,dwErrorCode,GetCustomErrorMsg((CustomError)dwErrorCode));
                }

            }

            CUtils::WriteLogFile(m_hLogFile,TRUE,strMsg);
        }

        break;

    case WorkMode_DiskClean:
        {
            CleanMode cleanMode = (CleanMode)m_Config.GetInt(_T("DiskClean"),_T("CleanMode"),0);
            int nFillVal = m_Config.GetInt(_T("DiskClean"),_T("FillValue"),0,16);
            BOOL bCompareClean = m_Config.GetBool(_T("DiskClean"),_T("En_CompareClean"),FALSE);
            CompareCleanSeq seq = (CompareCleanSeq)m_Config.GetInt(_T("DiskClean"),_T("CompareCleanSeq"),0);

            // 设置端口状态
            m_MasterPort.Initial();

            POSITION pos = m_TargetPorts.GetHeadPosition();
            while (pos)
            {
                CPort *port = m_TargetPorts.GetNext(pos);
                if (port->IsConnected())
                {
                    port->SetHashMethod(hashMethod);
                    port->SetWorkMode(m_WorkMode);
                    port->SetStartTime(m_StartTime);
                }
            }

            disk.SetTargetPorts(&m_TargetPorts);
            disk.SetCleanMode(cleanMode,nFillVal,bCompareClean,seq);

            bResult = disk.Start();

            if (bResult)
            {
                strMsg.Format(_T("%s Completed !!!"),strWorkMode);
            }
            else
            {
                // 任意取一个错误
                ErrorType errType = ErrorType_System;
                DWORD dwErrorCode = 0;
                pos = m_TargetPorts.GetHeadPosition();
                while (pos)
                {
                    CPort *port = m_TargetPorts.GetNext(pos);

                    if (port->IsConnected() && !port->GetResult())
                    {
                        errType = port->GetErrorCode(&dwErrorCode);
                        break;
                    }
                }

                if (errType == ErrorType_System)
                {
                    strMsg.Format(_T("%s Failed !\r\nSystem errorCode=%d,%s")
                        ,strWorkMode,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
                }
                else
                {
                    strMsg.Format(_T("%s Failed !\r\nCustom errorCode=0x%X,%s")
                        ,strWorkMode,dwErrorCode,GetCustomErrorMsg((CustomError)dwErrorCode));
                }
            }

            CUtils::WriteLogFile(m_hLogFile,TRUE,strMsg);
        }

        break;

    case WorkMode_DiskCompare:
        {
            CompareMode compareMode = (CompareMode)m_Config.GetInt(_T("DiskCompare"),_T("CompareMode"),0);
            CompareMethod compareMethod = (CompareMethod)m_Config.GetInt(_T("DiskCompare"),_T("CompareMethod"),0);

            BOOL bComputeHash = TRUE;

            if (compareMethod == CompareMethod_Byte)
            {
                bComputeHash = FALSE;
            }
            // 设置端口状态
            m_MasterPort.SetHashMethod(hashMethod);
            m_MasterPort.SetWorkMode(m_WorkMode);
            m_MasterPort.SetStartTime(m_StartTime);

            POSITION pos = m_TargetPorts.GetHeadPosition();
            while (pos)
            {
                CPort *port = m_TargetPorts.GetNext(pos);
                if (port->IsConnected())
                {
                    port->SetHashMethod(hashMethod);
                    port->SetWorkMode(m_WorkMode);
                    port->SetStartTime(m_StartTime);
                }
            }

            disk.SetMasterPort(&m_MasterPort);
            disk.SetTargetPorts(&m_TargetPorts);
            disk.SetHashMethod(bComputeHash,hashMethod);
            disk.SetCompareParm(TRUE,compareMethod);
            disk.SetCompareMode(compareMode);

            bResult = disk.Start();

            CString strHashValue;
            if (bResult)
            {
                if (bComputeHash)
                {
                    int len = m_MasterPort.GetHashLength();
                    BYTE *pHash = new BYTE[len];
                    ZeroMemory(pHash,len);
                    m_MasterPort.GetHash(pHash,len);
                    for (int i = 0;i < len;i++)
                    {
                        CString strHash;
                        strHash.Format(_T("%02X"),pHash[i]);

                        strHashValue += strHash;
                    }
                    delete []pHash;
                    strMsg.Format(_T("%s Completed !\r\nHashMethod=%s, HashValue=%s")
                        ,strWorkMode,m_MasterPort.GetHashMethodString(),strHashValue);
                }
                else
                {
                    strMsg.Format(_T("%s Completed !"),strWorkMode);
                }


            }
            else
            {
                ErrorType errType = ErrorType_System;
                DWORD dwErrorCode = 0;
                errType = m_MasterPort.GetErrorCode(&dwErrorCode);

                if (dwErrorCode == 0)
                {
                    // 任意取一个错误
                    pos = m_TargetPorts.GetHeadPosition();
                    while (pos)
                    {
                        CPort *port = m_TargetPorts.GetNext(pos);

                        if (port->IsConnected() && !port->GetResult())
                        {
                            errType = port->GetErrorCode(&dwErrorCode);
                            break;
                        }
                    }
                }

                if (errType == ErrorType_System)
                {
                    strMsg.Format(_T("%s Failed !\r\nSystem errorCode=%d,%s")
                        ,strWorkMode,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
                }
                else
                {
                    strMsg.Format(_T("%s Failed !\r\nCustom errorCode=0x%X,%s")
                        ,strWorkMode,dwErrorCode,GetCustomErrorMsg((CustomError)dwErrorCode));
                }

            }

            CUtils::WriteLogFile(m_hLogFile,TRUE,strMsg);
        }

        break;

    case WorkMode_ImageMake:
        {
            CString strImagePath = m_Config.GetString(_T("ImagePath"),_T("ImagePath"),_T("d:\\image"));
            CString strImageName = m_Config.GetString(_T("ImagePath"),_T("ImageName"));
            BOOL bServerFirst = m_Config.GetBool(_T("ImageMake"),_T("SavePath"),FALSE);
            int compressLevel = m_Config.GetInt(_T("ImageMake"),_T("CompressLevel"),1);
            UINT nImageMode = m_Config.GetInt(_T("ImageMake"),_T("SaveMode"),0);
            BOOL bDataCompress = m_Config.GetBool(_T("ImageMake"),_T("En_DataCompress"),TRUE);
            //BOOL bMtpImage = (nImageMode == 2);

            int nNumOfArea = m_Config.GetInt(_T("ImageMake"),_T("NumOfArea"),0);

            PRANGE_FROM_TO ranges = NULL;

            if (nNumOfArea > 0)
            {
                ranges = new RANGE_FROM_TO[nNumOfArea];

                CString strKey;
                ULONGLONG ullStartLBA = 0, ullEndLBA = 0;
                for (int i = 0; i < nNumOfArea;i++)
                {
                    strKey.Format(_T("StartLBA_%d"),i);

                    ullStartLBA = m_Config.GetUInt64(_T("ImageMake"),strKey,0);

                    strKey.Format(_T("EndLBA_%d"),i);

                    ullEndLBA = m_Config.GetUInt64(_T("ImageMake"),strKey,0);

                    ranges[i].ullStartLBA = ullStartLBA;
                    ranges[i].ullEndingLBA = ullEndLBA;
                }
            }

            // 设置端口状态
            m_MasterPort.SetHashMethod(hashMethod);
            m_MasterPort.SetWorkMode(m_WorkMode);
            m_MasterPort.SetStartTime(m_StartTime);

            strImagePath.TrimRight(_T('\\'));

            switch (nImageMode)
            {
            case FULL_IMAGE: //磁盘映像
            case QUICK_IMAGE:
                if (strImageName.Right(4).CompareNoCase(_T(".IMG")) != 0)
                {
                    strImageName += _T(".IMG");
                }
                break;

            case MTP_IMAGE: //MTP映像
                if (strImageName.Right(4).CompareNoCase(_T(".MTP")) != 0)
                {
                    strImageName += _T(".MTP");
                }
                break;

            case FILE_IMAGE: //文件映像
                if (strImageName.Right(4).CompareNoCase(_T(".FIL")) != 0)
                {
                    strImageName += _T(".FIL");
                }
                break;
            }

            CString strTempFile,strImageFile;
            strTempFile.Format(_T("%s\\%s.$$$"),strImagePath,strImageName.Left(strImageName.GetLength() - 4));
            strImageFile.Format(_T("%s\\%s"),strImagePath,strImageName);

            m_FilePort.SetPortType(PortType_TARGET_FILE);
            m_FilePort.SetFileName(strTempFile);
            m_FilePort.SetConnected(TRUE);
            m_FilePort.SetPortState(PortState_Online);
            m_FilePort.SetHashMethod(hashMethod);
            m_FilePort.SetWorkMode(m_WorkMode);

            PortList filePortList;
            filePortList.AddTail(&m_FilePort);

            if (nImageMode == MTP_IMAGE)
            {
                CWpdDevice wpdDevice;
                wpdDevice.Init(m_hWnd,&m_bCancel,m_hLogFile,&m_Command);
                wpdDevice.SetWorkMode(WorkMode_ImageMake);

                wpdDevice.SetMasterPort(&m_MasterPort);
                wpdDevice.SetTargetPorts(&filePortList);
                wpdDevice.SetHashMethod(TRUE,hashMethod);
                wpdDevice.SetSocket(m_ClientSocket,bServerFirst);
                wpdDevice.SetMakeImageParm(bDataCompress,compressLevel);

                bResult = wpdDevice.Start();
            }
            else
            {
                disk.SetMasterPort(&m_MasterPort);
                disk.SetTargetPorts(&filePortList);
                disk.SetHashMethod(TRUE,hashMethod);
                disk.SetSocket(m_ClientSocket,bServerFirst);
                disk.SetMakeImageParm(nImageMode,bDataCompress,compressLevel);
                disk.SetQuickCopyParm(ranges,nNumOfArea);

                bResult = disk.Start();
            }

            if (ranges)
            {
                delete []ranges;
            }

            CString strHashValue;
            if (bResult)
            {
                MoveFileEx(strTempFile,strImageFile,MOVEFILE_REPLACE_EXISTING);

                int len = m_MasterPort.GetHashLength();
                BYTE *pHash = new BYTE[len];
                ZeroMemory(pHash,len);
                m_MasterPort.GetHash(pHash,len);
                for (int i = 0;i < len;i++)
                {
                    CString strHash;
                    strHash.Format(_T("%02X"),pHash[i]);

                    strHashValue += strHash;
                }
                delete []pHash;
                strMsg.Format(_T("%s Completed !\r\nHashMethod=%s, HashValue=%s")
                    ,strWorkMode,m_MasterPort.GetHashMethodString(),strHashValue);

            }
            else
            {
                DeleteFile(strTempFile);

                ErrorType errType = ErrorType_System;
                DWORD dwErrorCode = 0;
                errType = m_MasterPort.GetErrorCode(&dwErrorCode);

                if (dwErrorCode == 0)
                {
                    // 任意取一个错误
                    errType = m_FilePort.GetErrorCode(&dwErrorCode);
                }

                if (errType == ErrorType_System)
                {
                    strMsg.Format(_T("%s Failed !\r\nSystem errorCode=%d,%s")
                        ,strWorkMode,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
                }
                else
                {
                    strMsg.Format(_T("%s Failed !\r\nCustom errorCode=0x%X,%s")
                        ,strWorkMode,dwErrorCode,GetCustomErrorMsg((CustomError)dwErrorCode));
                }

            }

            CUtils::WriteLogFile(m_hLogFile,TRUE,strMsg);
        }

        break;

    case  WorkMode_ImageCopy:
        {
            BOOL bCompare = m_Config.GetBool(_T("ImageCopy"),_T("En_Compare"),FALSE);
            CompareMethod compareMethod = (CompareMethod)m_Config.GetInt(_T("ImageCopy"),_T("CompareMethod"),0);
            UINT nImageType = m_Config.GetBool(_T("ImageCopy"),_T("ImageType"),0);

            BOOL bCleanDiskFirst = m_Config.GetBool(_T("ImageCopy"),_T("En_CleanDiskFirst"),FALSE);
            BOOL bCompareClean = m_Config.GetBool(_T("ImageCopy"),_T("En_CompareClean"),FALSE);
            UINT nCleanTimes = m_Config.GetUInt(_T("ImageCopy"),_T("CleanTimes"),1);
            CString strFillValues = m_Config.GetString(_T("ImageCopy"),_T("FillValues"));
            CompareCleanSeq seq = (CompareCleanSeq)m_Config.GetInt(_T("ImageCopy"),_T("CompareCleanSeq"),0);

            //BOOL bMtpImage = (nImageType == 1);

            if (nCleanTimes < 0 || nCleanTimes > 3)
            {
                nCleanTimes = 1;
            }

            int *pCleanFillValues = new int[nCleanTimes];
            memset(pCleanFillValues,0,nCleanTimes * sizeof(int));

            if (bCleanDiskFirst)
            {
                int nCurPos = 0;

                CString strToken;
                UINT nCount = 0;

                strToken = strFillValues.Tokenize(_T(";"),nCurPos);
                while (nCurPos != -1 && nCount < nCleanTimes)
                {
                    int nFillValue = _tcstoul(strToken, NULL, 16);

                    if (nFillValue == 0 && strToken.GetAt(0) != _T('0'))
                    {
                        nFillValue = RANDOM_VALUE;
                    }

                    pCleanFillValues[nCount++] = nFillValue;

                    strToken = strFillValues.Tokenize(_T(";"),nCurPos);
                }
            }

            // 设置端口状态
            CString strImagePath = m_Config.GetString(_T("ImagePath"),_T("ImagePath"),_T("d:\\image"));
            CString strImageName = m_Config.GetString(_T("ImagePath"),_T("ImageName"));
            strImagePath.TrimRight(_T('\\'));

            switch (nImageType)
            {
            case 0: //磁盘映像
                if (strImageName.Right(4).CompareNoCase(_T(".IMG")) != 0)
                {
                    strImageName += _T(".IMG");
                }
                break;

            case 1: //MTP映像
                if (strImageName.Right(4).CompareNoCase(_T(".MTP")) != 0)
                {
                    strImageName += _T(".MTP");
                }
                break;

            case 2: //文件映像
                if (strImageName.Right(4).CompareNoCase(_T(".FIL")) != 0)
                {
                    strImageName += _T(".FIL");
                }
                break;
            }

            CString strImageFile,strTempFile;
            strTempFile.Format(_T("%s\\%s.$$$"),strImagePath,strImageName.Left(strImageName.GetLength() - 4));
            strImageFile.Format(_T("%s\\%s"),strImagePath,strImageName);

            if (m_bServerFirst)
            {
                m_FilePort.SetPortType(PortType_SERVER);
            }
            else
            {
                m_FilePort.SetPortType(PortType_MASTER_FILE);
            }

            m_FilePort.SetFileName(strImageFile);
            m_FilePort.SetConnected(TRUE);
            m_FilePort.SetPortState(PortState_Online);
            m_FilePort.SetWorkMode(m_WorkMode);
            m_FilePort.SetStartTime(m_StartTime);

            POSITION pos = m_TargetPorts.GetHeadPosition();
            while (pos)
            {
                CPort *port = m_TargetPorts.GetNext(pos);
                if (port->IsConnected())
                {
                    port->SetWorkMode(m_WorkMode);
                    port->SetStartTime(m_StartTime);
                }
            }

            // if
            if (1 == nImageType)  //MTP Image
            {
                CWpdDevice wpdDevice;
                wpdDevice.Init(m_hWnd,&m_bCancel,m_hLogFile,&m_Command);
                wpdDevice.SetWorkMode(WorkMode_ImageCopy);

                wpdDevice.SetMasterPort(&m_FilePort);
                wpdDevice.SetTargetPorts(&m_TargetPorts);
                wpdDevice.SetHashMethod(TRUE,hashMethod);
                wpdDevice.SetCompareParm(bCompare,CompareMethod_Hash);
                wpdDevice.SetSocket(m_ClientSocket,m_bServerFirst);

                bResult = wpdDevice.Start();
            }
            else
            {
                disk.SetMasterPort(&m_FilePort);
                disk.SetTargetPorts(&m_TargetPorts);
                disk.SetHashMethod(TRUE,hashMethod);
                disk.SetCompareParm(bCompare,compareMethod);
                disk.SetSocket(m_ClientSocket,m_bServerFirst);
                disk.SetCleanDiskFirst(bCleanDiskFirst,bCompareClean,seq,nCleanTimes,pCleanFillValues);
                delete []pCleanFillValues;

                bResult = disk.Start();
            }

            CString strHashValue;
            if (bResult)
            {

                MoveFileEx(strTempFile,strImageFile,MOVEFILE_REPLACE_EXISTING);

                int len = m_FilePort.GetHashLength();
                BYTE *pHash = new BYTE[len];
                ZeroMemory(pHash,len);
                m_FilePort.GetHash(pHash,len);
                for (int i = 0;i < len;i++)
                {
                    CString strHash;
                    strHash.Format(_T("%02X"),pHash[i]);

                    strHashValue += strHash;
                }
                delete []pHash;
                strMsg.Format(_T("%s Completed !\r\nHashMethod=%s, HashValue=%s")
                    ,strWorkMode,m_FilePort.GetHashMethodString(),strHashValue);

            }
            else
            {
                DeleteFile(strTempFile);

                ErrorType errType = ErrorType_System;
                DWORD dwErrorCode = 0;
                errType = m_FilePort.GetErrorCode(&dwErrorCode);

                if (dwErrorCode == 0)
                {
                    // 任意取一个错误
                    pos = m_TargetPorts.GetHeadPosition();
                    while (pos)
                    {
                        CPort *port = m_TargetPorts.GetNext(pos);

                        if (port->IsConnected() && !port->GetResult())
                        {
                            errType = port->GetErrorCode(&dwErrorCode);
                            break;
                        }
                    }
                }

                if (errType == ErrorType_System)
                {
                    strMsg.Format(_T("%s Failed !\r\nSystem errorCode=%d,%s")
                        ,strWorkMode,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
                }
                else
                {
                    strMsg.Format(_T("%s Failed !\r\nCustom errorCode=0x%X,%s")
                        ,strWorkMode,dwErrorCode,GetCustomErrorMsg((CustomError)dwErrorCode));
                }

            }

            CUtils::WriteLogFile(m_hLogFile,TRUE,strMsg);
        }

        break;

    case WorkMode_FileCopy:
        {
            BOOL bComputeHash = m_Config.GetBool(_T("FileCopy"),_T("En_ComputeHash"),FALSE);
            BOOL bCompare = m_Config.GetBool(_T("FileCopy"),_T("En_Compare"),FALSE);
            CompareMethod compareMethod = (CompareMethod)m_Config.GetInt(_T("FileCopy"),_T("CompareMethod"),0);
            BOOL bCopyAllFiles = m_Config.GetBool(_T("FileCopy"),_T("En_CopyAllFiles"),TRUE);
            BOOL bCleanupTargets = m_Config.GetBool(_T("FileCopy"),_T("En_CleanupTargets"),FALSE);

            UINT nNumOfFolders = m_Config.GetUInt(_T("FileCopy"),_T("NumOfFolders"),0);
            UINT nNumOfFiles = m_Config.GetUInt(_T("FileCopy"),_T("NumOfFiles"),0);
            BOOL bFileCopyType = m_Config.GetBool(_T("FileCopy"), _T("FileCopyType"), FALSE);


            CStringArray filesArray,folderArray;
            CString strKey,strFile,strPath;

            if (bCopyAllFiles)
            {
                strPath = MASTER_PATH;

                folderArray.Add(strPath);
            }
            else
            {
                for (UINT i = 0;i < nNumOfFolders;i++)
                {
                    strKey.Format(_T("Folder_%d"),i);
                    strFile = m_Config.GetString(_T("FileCopy"),strKey);

                    if (strFile.IsEmpty())
                    {
                        continue;
                    }

                    strPath = /*MASTER_PATH +*/ strFile;

                    if (PathFileExists(strPath))
                    {
                        folderArray.Add(strFile);
                    }
                    else
                    {
                        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("File Copy,folder %s doesn't exist."),strPath);
                    }

                }

                for (UINT i = 0;i < nNumOfFiles;i++)
                {
                    strKey.Format(_T("File_%d"),i);
                    strFile = m_Config.GetString(_T("FileCopy"),strKey);

                    if (strFile.IsEmpty())
                    {
                        continue;
                    }

                    strPath = /*MASTER_PATH +*/ strFile;

                    if (PathFileExists(strPath))
                    {
                        filesArray.Add(strPath);
                    }
                    else
                    {
                        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("File Copy,file %s doesn't exist."),strPath);
                    }
                }
            }


            // 设置端口状态

            if (!((m_WorkMode == WorkMode_FileCopy) && bFileCopyType))
            {
                m_MasterPort.SetHashMethod(hashMethod);
                m_MasterPort.SetWorkMode(m_WorkMode);
                m_MasterPort.SetStartTime(m_StartTime);
            }

            POSITION pos = m_TargetPorts.GetHeadPosition();
            while (pos)
            {
                CPort *port = m_TargetPorts.GetNext(pos);
                if (port->IsConnected())
                {
                    port->SetHashMethod(hashMethod);
                    port->SetWorkMode(m_WorkMode);
                    port->SetStartTime(m_StartTime);
                }
            }

            disk.SetMasterPort(&m_MasterPort);
            disk.SetTargetPorts(&m_TargetPorts);
            disk.SetHashMethod(bComputeHash,hashMethod);
            disk.SetCompareParm(bCompare,compareMethod);
            disk.SetFileAndFolder(bCleanupTargets,filesArray,folderArray);

            bResult = disk.Start();

            CString strHashValue;
            if (bResult)
            {
                if (bComputeHash)
                {
                    int len = m_MasterPort.GetHashLength();
                    BYTE *pHash = new BYTE[len];
                    ZeroMemory(pHash,len);
                    m_MasterPort.GetHash(pHash,len);
                    for (int i = 0;i < len;i++)
                    {
                        CString strHash;
                        strHash.Format(_T("%02X"),pHash[i]);

                        strHashValue += strHash;
                    }
                    delete []pHash;
                    strMsg.Format(_T("%s Completed !\r\nHashMethod=%s, HashValue=%s")
                        ,strWorkMode,m_MasterPort.GetHashMethodString(),strHashValue);
                }
                else
                {
                    strMsg.Format(_T("%s Completed !!!"),strWorkMode);
                }
            }
            else
            {
                ErrorType errType = ErrorType_System;
                DWORD dwErrorCode = 0;
                errType = m_MasterPort.GetErrorCode(&dwErrorCode);

                if (dwErrorCode == 0)
                {
                    // 任意取一个错误
                    pos = m_TargetPorts.GetHeadPosition();
                    while (pos)
                    {
                        CPort *port = m_TargetPorts.GetNext(pos);

                        if (port->IsConnected() && !port->GetResult())
                        {
                            errType = port->GetErrorCode(&dwErrorCode);
                            break;
                        }
                    }
                }

                if (errType == ErrorType_System)
                {
                    strMsg.Format(_T("%s Failed !\r\nSystem errorCode=%d,%s")
                        ,strWorkMode,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
                }
                else
                {
                    strMsg.Format(_T("%s Failed !\r\nCustom errorCode=0x%X,%s")
                        ,strWorkMode,dwErrorCode,GetCustomErrorMsg((CustomError)dwErrorCode));
                }

            }

            CUtils::WriteLogFile(m_hLogFile,TRUE,strMsg);
        }
        break;

    case WorkMode_DiskFormat:
        {
            CString strVolumeLable = m_Config.GetString(_T("DiskFormat"),_T("VolumeLabel"));
            FileSystem fileSystem = (FileSystem)m_Config.GetUInt(_T("DiskFormat"),_T("FileSystem"),FileSystem_FAT32);
            UINT nClusterSize = m_Config.GetUInt(_T("DiskFormat"),_T("ClusterSize"),0);
            BOOL bQuickFormat = m_Config.GetBool(_T("DiskFormat"),_T("QuickFormat"),TRUE);

            // 设置端口状态
            m_MasterPort.Initial();

            POSITION pos = m_TargetPorts.GetHeadPosition();
            while (pos)
            {
                CPort *port = m_TargetPorts.GetNext(pos);
                if (port->IsConnected())
                {
                    port->SetHashMethod(hashMethod);
                    port->SetWorkMode(m_WorkMode);
                    port->SetStartTime(m_StartTime);
                }
            }

            disk.SetTargetPorts(&m_TargetPorts);
            disk.SetFormatParm(strVolumeLable,fileSystem,nClusterSize,TRUE);

            bResult = disk.Start();

            if (bResult)
            {
                strMsg.Format(_T("%s Completed !!!"),strWorkMode);
            }
            else
            {
                // 任意取一个错误
                ErrorType errType = ErrorType_System;
                DWORD dwErrorCode = 0;
                pos = m_TargetPorts.GetHeadPosition();
                while (pos)
                {
                    CPort *port = m_TargetPorts.GetNext(pos);

                    if (port->IsConnected() && !port->GetResult())
                    {
                        errType = port->GetErrorCode(&dwErrorCode);
                        break;
                    }
                }

                if (errType == ErrorType_System)
                {
                    strMsg.Format(_T("%s Failed !\r\nSystem errorCode=%d,%s")
                        ,strWorkMode,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
                }
                else
                {
                    strMsg.Format(_T("%s Failed !\r\nCustom errorCode=0x%X,%s")
                        ,strWorkMode,dwErrorCode,GetCustomErrorMsg((CustomError)dwErrorCode));
                }
            }

            CUtils::WriteLogFile(m_hLogFile,TRUE,strMsg);
        }
        break;

    case WorkMode_DifferenceCopy:
        {
            BOOL bUploadUserLog = m_Config.GetBool(_T("DifferenceCopy"),_T("En_UserLog"),FALSE);
            BOOL bComputeHash = m_Config.GetBool(_T("DifferenceCopy"),_T("En_ComputeHash"),FALSE);
            BOOL bCompare = m_Config.GetBool(_T("DifferenceCopy"),_T("En_Compare"),FALSE);
            BOOL bIncludeSubDir = m_Config.GetBool(_T("DifferenceCopy"),_T("En_IncludeSub"),FALSE);
            UINT nSourceType = m_Config.GetUInt(_T("DifferenceCopy"),_T("SourceType"),0);
            UINT nCompareRule = m_Config.GetUInt(_T("DifferenceCopy"),_T("CompareRule"),0);
            UINT nPkgLocation = m_Config.GetUInt(_T("DifferenceCopy"),_T("PkgLocation"),0);
            CString strPackageName = m_Config.GetString(_T("DifferenceCopy"),_T("PkgName"));
            CStringArray strArrayLog;

            if (bUploadUserLog)
            {
                UINT nNumOfLogPath = m_Config.GetBool(_T("DifferenceCopy"),_T("NumOfLogPath"),FALSE);

                CString strKey,strValue;

                for (UINT i = 0;i < nNumOfLogPath;i++)
                {
                    strKey.Format(_T("LogPath_%d"),i);
                    strValue = m_Config.GetString(_T("DifferenceCopy"),strKey);

                    if (strValue.Find(_T(":")))
                    {
                        strArrayLog.Add(strValue);
                    }
                }
            }


            CPort *masterPort = NULL;
            if (nSourceType == SourceType_Package)
            {
                if (nPkgLocation == PathType_Local)
                {
                    masterPort = &m_MasterPort;

                    strPackageName = MASTER_PATH + strPackageName;
                }
                else
                {
                    masterPort = &m_FilePort;
                }
            }
            else
            {
                nPkgLocation = PathType_Local;
                masterPort = &m_MasterPort;
            }


            // 设置端口状态
            masterPort->SetFileName(strPackageName);
            masterPort->SetConnected(TRUE);
            masterPort->SetPortState(PortState_Online);
            masterPort->SetHashMethod(hashMethod);
            masterPort->SetWorkMode(m_WorkMode);
            masterPort->SetStartTime(m_StartTime);

            POSITION pos = m_TargetPorts.GetHeadPosition();
            while (pos)
            {
                CPort *port = m_TargetPorts.GetNext(pos);
                if (port->IsConnected())
                {
                    port->SetHashMethod(hashMethod);
                    port->SetWorkMode(m_WorkMode);
                    port->SetStartTime(m_StartTime);
                }
            }

            disk.SetMasterPort(masterPort);
            disk.SetTargetPorts(&m_TargetPorts);
            disk.SetHashMethod(bComputeHash,hashMethod);
            disk.SetCompareParm(bCompare,CompareMethod_Hash);
            disk.SetSocket(m_ClientSocket,nPkgLocation);
            disk.SetDiffCopyParm(nSourceType,nPkgLocation,nCompareRule,bUploadUserLog,strArrayLog,bIncludeSubDir);

            bResult = disk.Start();

            CString strHashValue;
            if (bResult)
            {
                if (bComputeHash)
                {
                    int len = masterPort->GetHashLength();
                    BYTE *pHash = new BYTE[len];
                    ZeroMemory(pHash,len);
                    masterPort->GetHash(pHash,len);
                    for (int i = 0;i < len;i++)
                    {
                        CString strHash;
                        strHash.Format(_T("%02X"),pHash[i]);

                        strHashValue += strHash;
                    }
                    delete []pHash;
                    strMsg.Format(_T("%s Completed !\r\nHashMethod=%s, HashValue=%s")
                        ,strWorkMode,masterPort->GetHashMethodString(),strHashValue);
                }
                else
                {
                    strMsg.Format(_T("%s Completed !!!"),strWorkMode);
                }
            }
            else
            {
                ErrorType errType = ErrorType_System;
                DWORD dwErrorCode = 0;
                errType = masterPort->GetErrorCode(&dwErrorCode);

                if (dwErrorCode == 0)
                {
                    // 任意取一个错误
                    pos = m_TargetPorts.GetHeadPosition();
                    while (pos)
                    {
                        CPort *port = m_TargetPorts.GetNext(pos);

                        if (port->IsConnected() && !port->GetResult())
                        {
                            errType = port->GetErrorCode(&dwErrorCode);
                            break;
                        }
                    }
                }

                if (errType == ErrorType_System)
                {
                    strMsg.Format(_T("%s Failed !\r\nSystem errorCode=%d,%s")
                        ,strWorkMode,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
                }
                else
                {
                    strMsg.Format(_T("%s Failed !\r\nCustom errorCode=0x%X,%s")
                        ,strWorkMode,dwErrorCode,GetCustomErrorMsg((CustomError)dwErrorCode));
                }

            }

            CUtils::WriteLogFile(m_hLogFile,TRUE,strMsg);

        }
        break;

    case WorkMode_MTPCopy:
        {
            BOOL bComputeHash = m_Config.GetBool(_T("MTPCopy"),_T("En_ComputeHash"),FALSE);
            BOOL bCompare = m_Config.GetBool(_T("MTPCopy"),_T("En_Compare"),FALSE);

            // 设置端口状态
            m_MasterPort.SetHashMethod(hashMethod);
            m_MasterPort.SetWorkMode(m_WorkMode);
            m_MasterPort.SetStartTime(m_StartTime);

            POSITION pos = m_TargetPorts.GetHeadPosition();
            while (pos)
            {
                CPort *port = m_TargetPorts.GetNext(pos);
                if (port->IsConnected())
                {
                    port->SetHashMethod(hashMethod);
                    port->SetWorkMode(m_WorkMode);
                    port->SetStartTime(m_StartTime);
                }
            }

            CWpdDevice wpdDevice;
            wpdDevice.Init(m_hWnd,&m_bCancel,m_hLogFile,&m_Command);
            wpdDevice.SetWorkMode(WorkMode_MTPCopy);

            wpdDevice.SetMasterPort(&m_MasterPort);
            wpdDevice.SetTargetPorts(&m_TargetPorts);
            wpdDevice.SetHashMethod(bComputeHash,hashMethod);
            wpdDevice.SetCompareParm(bCompare,CompareMethod_Hash);

            bResult = wpdDevice.Start();

            CString strHashValue;
            if (bResult)
            {
                if (bComputeHash)
                {
                    int len = m_MasterPort.GetHashLength();
                    BYTE *pHash = new BYTE[len];
                    ZeroMemory(pHash,len);
                    m_MasterPort.GetHash(pHash,len);
                    for (int i = 0;i < len;i++)
                    {
                        CString strHash;
                        strHash.Format(_T("%02X"),pHash[i]);

                        strHashValue += strHash;
                    }
                    delete []pHash;
                    strMsg.Format(_T("%s Completed !\r\nHashMethod=%s, HashValue=%s")
                        ,strWorkMode,m_MasterPort.GetHashMethodString(),strHashValue);
                }
                else
                {
                    strMsg.Format(_T("%s Completed !!!"),strWorkMode);
                }
            }
            else
            {
                ErrorType errType = ErrorType_System;
                DWORD dwErrorCode = 0;
                errType = m_MasterPort.GetErrorCode(&dwErrorCode);

                if (dwErrorCode == 0)
                {
                    // 任意取一个错误
                    pos = m_TargetPorts.GetHeadPosition();
                    while (pos)
                    {
                        CPort *port = m_TargetPorts.GetNext(pos);

                        if (port->IsConnected() && !port->GetResult())
                        {
                            errType = port->GetErrorCode(&dwErrorCode);
                            break;
                        }
                    }
                }

                if (errType == ErrorType_System)
                {
                    strMsg.Format(_T("%s Failed !\r\nSystem errorCode=%d,%s")
                        ,strWorkMode,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
                }
                else
                {
                    strMsg.Format(_T("%s Failed !\r\nCustom errorCode=0x%X,%s")
                        ,strWorkMode,dwErrorCode,GetCustomErrorMsg((CustomError)dwErrorCode));
                }

            }

            CUtils::WriteLogFile(m_hLogFile,TRUE,strMsg);
        }
        break;

    case WorkMode_Full_RW_Test:
        {
            BOOL bReadOnlyTest = m_Config.GetBool(_T("FullRWTest"),_T("En_ReadOnlyTest"),FALSE);
            BOOL bRetainOriginData = m_Config.GetBool(_T("FullRWTest"),_T("En_RetainOriginData"),FALSE);
            BOOL bFormatFinish = m_Config.GetBool(_T("FullRWTest"),_T("En_FormatFinished"),FALSE);
            BOOL bStopBad = m_Config.GetBool(_T("FullRWTest"),_T("En_StopBadBlock"),FALSE);

            // 设置端口状态
            m_MasterPort.Initial();

            POSITION pos = m_TargetPorts.GetHeadPosition();
            while (pos)
            {
                CPort *port = m_TargetPorts.GetNext(pos);
                if (port->IsConnected())
                {
                    port->SetHashMethod(hashMethod);
                    port->SetWorkMode(m_WorkMode);
                    port->SetStartTime(m_StartTime);
                }
            }

            if (bFormatFinish)
            {
                CString strVolumeLable = m_Config.GetString(_T("DiskFormat"),_T("VolumeLabel"));
                FileSystem fileSystem = (FileSystem)m_Config.GetUInt(_T("DiskFormat"),_T("FileSystem"),FileSystem_FAT32);
                UINT nClusterSize = m_Config.GetUInt(_T("DiskFormat"),_T("ClusterSize"),0);
                BOOL bQuickFormat = m_Config.GetBool(_T("DiskFormat"),_T("QuickFormat"),TRUE);

                disk.SetFormatParm(strVolumeLable,fileSystem,nClusterSize,TRUE);
            }

            disk.SetTargetPorts(&m_TargetPorts);
            disk.SetFullRWTestParm(bReadOnlyTest,bRetainOriginData,bFormatFinish,bStopBad);

            bResult = disk.Start();

            if (bResult)
            {
                strMsg.Format(_T("%s Completed !!!"),strWorkMode);
            }
            else
            {
                // 任意取一个错误
                ErrorType errType = ErrorType_System;
                DWORD dwErrorCode = 0;
                pos = m_TargetPorts.GetHeadPosition();
                while (pos)
                {
                    CPort *port = m_TargetPorts.GetNext(pos);

                    if (port->IsConnected() && !port->GetResult())
                    {
                        errType = port->GetErrorCode(&dwErrorCode);
                        break;
                    }
                }

                if (errType == ErrorType_System)
                {
                    strMsg.Format(_T("%s Failed !\r\nSystem errorCode=%d,%s")
                        ,strWorkMode,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
                }
                else
                {
                    strMsg.Format(_T("%s Failed !\r\nCustom errorCode=0x%X,%s")
                        ,strWorkMode,dwErrorCode,GetCustomErrorMsg((CustomError)dwErrorCode));
                }
            }

            CUtils::WriteLogFile(m_hLogFile,TRUE,strMsg);
        }
        break;

    case WorkMode_Fade_Picker:
        {
            BOOL bRetainOriginData = m_Config.GetBool(_T("FadePicker"),_T("En_RetainOriginData"),FALSE);
            BOOL bFormatFinish = m_Config.GetBool(_T("FadePicker"),_T("En_FormatFinished"),FALSE);

            // 设置端口状态
            m_MasterPort.Initial();

            POSITION pos = m_TargetPorts.GetHeadPosition();
            while (pos)
            {
                CPort *port = m_TargetPorts.GetNext(pos);
                if (port->IsConnected())
                {
                    port->SetHashMethod(hashMethod);
                    port->SetWorkMode(m_WorkMode);
                    port->SetStartTime(m_StartTime);
                }
            }

            if (bFormatFinish)
            {
                CString strVolumeLable = m_Config.GetString(_T("DiskFormat"),_T("VolumeLabel"));
                FileSystem fileSystem = (FileSystem)m_Config.GetUInt(_T("DiskFormat"),_T("FileSystem"),FileSystem_FAT32);
                UINT nClusterSize = m_Config.GetUInt(_T("DiskFormat"),_T("ClusterSize"),0);
                BOOL bQuickFormat = m_Config.GetBool(_T("DiskFormat"),_T("QuickFormat"),TRUE);

                disk.SetFormatParm(strVolumeLable,fileSystem,nClusterSize,TRUE);
            }

            disk.SetTargetPorts(&m_TargetPorts);
            disk.SetFadePickerParm(bRetainOriginData,bFormatFinish);

            bResult = disk.Start();

            if (bResult)
            {
                strMsg.Format(_T("%s Completed !!!"),strWorkMode);
            }
            else
            {
                // 任意取一个错误
                ErrorType errType = ErrorType_System;
                DWORD dwErrorCode = 0;
                pos = m_TargetPorts.GetHeadPosition();
                while (pos)
                {
                    CPort *port = m_TargetPorts.GetNext(pos);

                    if (port->IsConnected() && !port->GetResult())
                    {
                        errType = port->GetErrorCode(&dwErrorCode);
                        break;
                    }
                }

                if (errType == ErrorType_System)
                {
                    strMsg.Format(_T("%s Failed !\r\nSystem errorCode=%d,%s")
                        ,strWorkMode,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
                }
                else
                {
                    strMsg.Format(_T("%s Failed !\r\nCustom errorCode=0x%X,%s")
                        ,strWorkMode,dwErrorCode,GetCustomErrorMsg((CustomError)dwErrorCode));
                }
            }

            CUtils::WriteLogFile(m_hLogFile,TRUE,strMsg);
        }
        break;

    case WorkMode_Speed_Check:
        {
            BOOL bSpeedUpStandard = m_Config.GetBool(_T("SpeedCheck"),_T("En_SpeedUpStandard"),FALSE);
            double dbReadSpeed = m_Config.GetDouble(_T("SpeedCheck"),_T("ReadSpeed"),0.0);
            double dbWriteSpeed = m_Config.GetDouble(_T("SpeedCheck"),_T("WriteSpeed"),0.0);

            // 设置端口状态
            m_MasterPort.Initial();

            POSITION pos = m_TargetPorts.GetHeadPosition();
            while (pos)
            {
                CPort *port = m_TargetPorts.GetNext(pos);
                if (port->IsConnected())
                {
                    port->SetHashMethod(hashMethod);
                    port->SetWorkMode(m_WorkMode);
                    port->SetStartTime(m_StartTime);
                }
            }

            disk.SetTargetPorts(&m_TargetPorts);
            disk.SetSpeedCheckParm(bSpeedUpStandard,dbReadSpeed,dbWriteSpeed);

            bResult = disk.Start();

            if (bResult)
            {
                strMsg.Format(_T("%s Completed !!!"),strWorkMode);
            }
            else
            {
                // 任意取一个错误
                ErrorType errType = ErrorType_System;
                DWORD dwErrorCode = 0;
                pos = m_TargetPorts.GetHeadPosition();
                while (pos)
                {
                    CPort *port = m_TargetPorts.GetNext(pos);

                    if (port->IsConnected() && !port->GetResult())
                    {
                        errType = port->GetErrorCode(&dwErrorCode);
                        break;
                    }
                }

                if (errType == ErrorType_System)
                {
                    strMsg.Format(_T("%s Failed !\r\nSystem errorCode=%d,%s")
                        ,strWorkMode,dwErrorCode,CUtils::GetErrorMsg(dwErrorCode));
                }
                else
                {
                    strMsg.Format(_T("%s Failed !\r\nCustom errorCode=0x%X,%s")
                        ,strWorkMode,dwErrorCode,GetCustomErrorMsg((CustomError)dwErrorCode));
                }
            }

            CUtils::WriteLogFile(m_hLogFile,TRUE,strMsg);
        }
        break;

    }

    m_strMsg = strMsg;
    m_bResult = bResult;

    KillTimer(TIMER_UPDATE_STATISTIC);

    m_bRunning = FALSE;

    PostMessage(WM_UPDATE_STATISTIC);

    if (!m_bCancel)
    {
        PostMessage(WM_COMMAND, MAKEWPARAM(IDC_BTN_START, BN_CLICKED), (LPARAM)m_hWnd);
    }
}

void MainWindow::OnStartAsync()
{
    HashMethod hashMethod = (HashMethod)m_Config.GetInt(_T("Option"),_T("HashMethod"),0);
    BOOL bComputeHash = m_Config.GetBool(_T("FileCopy"),_T("En_ComputeHash"),FALSE);
    BOOL bCompare = m_Config.GetBool(_T("FileCopy"),_T("En_Compare"),FALSE);
    CompareMethod compareMethod = (CompareMethod)m_Config.GetInt(_T("FileCopy"),_T("CompareMethod"),0);
    BOOL bCopyAllFiles = m_Config.GetBool(_T("FileCopy"),_T("En_CopyAllFiles"),TRUE);
    BOOL bCleanupTargets = m_Config.GetBool(_T("FileCopy"),_T("En_CleanupTargets"),FALSE);
    UINT nBlockSectors = m_Config.GetUInt(_T("Option"),_T("BlockSectors"),256); // 默认128KB
    BOOL bMustSameCapacity = m_Config.GetBool(_T("Option"),_T("En_SameCapacity"),FALSE);

    UINT nNumOfFolders = m_Config.GetUInt(_T("FileCopy"),_T("NumOfFolders"),0);
    UINT nNumOfFiles = m_Config.GetUInt(_T("FileCopy"),_T("NumOfFiles"),0);

    CStringArray filesArray,folderArray;
    CString strKey,strFile,strPath;

    if (bCopyAllFiles)
    {
        strPath = MASTER_PATH;

        folderArray.push_back(strPath);
    }
    else
    {
        for (UINT i = 0;i < nNumOfFolders;i++)
        {
            strKey.Format(_T("Folder_%d"),i);
            strFile = m_Config.GetString(_T("FileCopy"),strKey);

            if (strFile.IsEmpty())
            {
                continue;
            }

            strPath = /*MASTER_PATH +*/ strFile;

            if (PathFileExists(strPath))
            {
                folderArray.push_back(strFile);
            }
            else
            {
                CString strTmp;
                strTmp.Format(_T("File Copy,folder %s doesn't exist."),strPath.c_str());
                CUtils::WriteLogFile(m_hLogFile,TRUE,strTmp);
            }

        }

        for (UINT i = 0;i < nNumOfFiles;i++)
        {
            strKey.Format(_T("File_%d"),i);
            strFile = m_Config.GetString(_T("FileCopy"),strKey);

            if (strFile.IsEmpty())
            {
                continue;
            }

            strPath = /*MASTER_PATH +*/ strFile;

            if (PathFileExists(strPath))
            {
                filesArray.push_back(strPath);
            }
            else
            {
                CString strTmp;
                strTmp.Format(_T("File Copy,file %s doesn't exist."),strPath.c_str());
                CUtils::WriteLogFile(m_hLogFile,TRUE,strTmp);
            }
        }
    }

    Disk disk;
    disk.SetMasterPort(&m_MasterPort);
    disk.SetSingleTargetPort(m_pSinglePort);
    disk.SetHashMethod(bComputeHash,hashMethod);
    disk.SetCompareParm(bCompare,compareMethod);
    disk.SetFileAndFolder(bCleanupTargets,filesArray,folderArray);
    disk.SetWorkMode(m_WorkMode);
    disk.Init(this,&m_bCancel,m_hLogFile,&m_Command,nBlockSectors);
    disk.SetGlobalParm(bMustSameCapacity);

    BOOL bResult = disk.Start();


//    KillTimer(TIMER_UPDATE_STATISTIC);

}

void MainWindow::BackupLogfile(HANDLE hFile, DWORD dwFileSize)
{
    CString strBackupFile = m_strAppPath + LOG_FILE_BAK;

    if (PathFileExists(strBackupFile))
    {
        struct stat status;
        stat(strBackupFile.c_str(),&status);

        if (status.st_size > 1 * 1024 * 1024 * 1024)
        {
            DeleteFile(strBackupFile);
        }
    }

    HANDLE hBackup = open(strBackupFile.c_str(),O_RDWR|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO);

    if (hBackup < 0)
    {
        DEBUG("delete backup file failed!");
        return;
    }

    DWORD ullSize;
    off_t size;
    if ((size = lseek(hBackup,0,SEEK_END))>=0)
    {
        ullSize = size;
        BYTE *pByte = new BYTE[dwFileSize];
        ZeroMemory(pByte,dwFileSize);

        if(Disk::ReadFileAsyn(hFile,0,dwFileSize,pByte,NULL))
        {
            if(!Disk::WriteFileAsyn(hBackup,ullSize,dwFileSize,pByte,NULL))
            {
                ERROR("write backup file error!");
            }
        }

        delete []pByte;
    }
    else
    {
        ERROR("lseek failed!");
    }

    ::close(hBackup);
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
    lpParm = lpParm;
    return NULL;
}

void *MainWindow::StopThreadProc(LPVOID lpParm)
{
    lpParm = lpParm;
    return NULL;
}

void *MainWindow::InitialMachineThreadProc(LPVOID lpParm)
{
    lpParm = lpParm;
    return NULL;
}

void *MainWindow::EnumDeviceThreadProc(LPVOID lpParm)
{
    lpParm = lpParm;
    return NULL;
}

void *MainWindow::BurnInTestThreadProc(LPVOID lpParm)
{
    lpParm = lpParm;
    return NULL;
}

void *MainWindow::ConnectSocketThreadProc(LPVOID lpParm)
{
    lpParm = lpParm;
    return NULL;
}

void *MainWindow::StartAsyncThreadProc(LPVOID lpParm)
{
    MainWindow *pDlg = (MainWindow *)lpParm;
    pDlg->OnStartAsync();
    return NULL;
}

CString MainWindow::GetUploadLogString()
{
    CString strLog;

    //PortNum;PortType;MachineSN;AliasName;SerialNumber;Model;DataSize;Capacity;Function;StartTime;EndTime;
    //1 - HashMethod;HashValue;Speed;Result;ErrorType;ErrorCode
    //2 - BadBlocks;ReadSpeed;WriteSpeed;Result;ErrorType;ErrorCode

    CString strItem;

    CString strAliasName,strMachineSN;
//    GetDlgItemText(IDC_TEXT_ALIAS,strAliasName);
//    GetDlgItemText(IDC_TEXT_SN,strMachineSN);


    if (m_MasterPort.IsConnected())
    {
        strItem.Format(_T("%s;"),m_MasterPort.GetPortName().c_str());
        strLog += strItem;

        strItem.Format(_T("%s;"),m_MasterPort.GetPortTypeName().c_str());
        strLog += strItem;

//        strItem.Format(_T("%s;"),strMachineSN.c_str());
//        strLog += strItem;

//        strItem.Format(_T("%s;"),strAliasName.c_str());
//        strLog += strItem;


        strItem.Format(_T("%s;"),m_MasterPort.GetSN().c_str());
        strLog += strItem;

        strItem.Format(_T("%s;"),m_MasterPort.GetModuleName().c_str());
        strLog += strItem;

        strItem.Format(_T("%I64d;"),m_MasterPort.GetValidSize());
        strLog += strItem;

        strItem.Format(_T("%I64d;"),m_MasterPort.GetTotalSize());
        strLog += strItem;

        strItem.Format(_T("%s;"),GetWorkModeString(m_WorkMode).c_str());
        strLog += strItem;

//        strItem.Format(_T("%s;"),m_MasterPort.GetStartTime().Format(_T("%Y-%m-%d %H:%M:%S")));
//        strLog += strItem;

//        strItem.Format(_T("%s;"),m_MasterPort.GetEndTime().Format(_T("%Y-%m-%d %H:%M:%S")));
//        strLog += strItem;

        strItem.Format(_T("%s;"),m_MasterPort.GetHashMethodString().c_str());
        strLog += strItem;

        strItem.Format(_T("%s;"),m_MasterPort.GetHashString().c_str());
        strLog += strItem;

        strItem.Format(_T("%d;"),(int)m_MasterPort.GetRealSpeed());
        strLog += strItem;

        strItem.Format(_T("%s;"),m_MasterPort.GetResultString().c_str());
        strLog += strItem;

        DWORD dwErrorCode = 0;
        ErrorType errType = m_MasterPort.GetErrorCode(&dwErrorCode);

        strItem.Format(_T("%d;"),errType);
        strLog += strItem;

        strItem.Format(_T("%d\r\n"),dwErrorCode);
        strLog += strItem;
    }


    PortList::const_iterator itPort = m_TargetPorts.begin();
    PortList::const_iterator itPort_end = m_TargetPorts.end();

    while (itPort != itPort_end)
    {
        CPort *port = (*itPort);
        itPort++;

        if (port->IsConnected())
        {
            strItem.Format(_T("%s;"),port->GetPortName().c_str());
            strLog += strItem;

            strItem.Format(_T("%s;"),port->GetPortTypeName().c_str());
            strLog += strItem;

//            strItem.Format(_T("%s;"),strMachineSN.c_str());
//            strLog += strItem;

//            strItem.Format(_T("%s;"),strAliasName.c_str());
//            strLog += strItem;

            strItem.Format(_T("%s;"),port->GetSN().c_str());
            strLog += strItem;

            strItem.Format(_T("%s;"),port->GetModuleName().c_str());
            strLog += strItem;

            // 如果是卡检测，这里填实际容量
            if (m_WorkMode == WorkMode_Full_RW_Test
                || m_WorkMode == WorkMode_Fade_Picker
                || m_WorkMode == WorkMode_Speed_Check
                )
            {
                if (port->GetBadBlockCount() > 0)
                {
                    CULLArray badBlocks;
                    port->GetBadBlockArray(badBlocks);

                    ULONGLONG ullSize = badBlocks.at(0) * port->GetBytesPerSector();

                    strItem.Format(_T("%I64d;"),ullSize);
                }
                else
                {
                    strItem.Format(_T("%I64d;"),port->GetTotalSize());
                }
            }
            else
            {
                strItem.Format(_T("%I64d;"),port->GetValidSize());
            }
            strLog += strItem;

            strItem.Format(_T("%I64d;"),port->GetTotalSize());
            strLog += strItem;

            strItem.Format(_T("%s;"),GetWorkModeString(m_WorkMode).c_str());
            strLog += strItem;

//            strItem.Format(_T("%s;"),port->GetStartTime().Format(_T("%Y-%m-%d %H:%M:%S")));
//            strLog += strItem;

//            strItem.Format(_T("%s;"),port->GetEndTime().Format(_T("%Y-%m-%d %H:%M:%S")));
//            strLog += strItem;

            // 如果是卡检测，填坏块数量、读速度、写速度
            if (m_WorkMode == WorkMode_Full_RW_Test
                || m_WorkMode == WorkMode_Fade_Picker
                || m_WorkMode == WorkMode_Speed_Check
                )
            {
                strItem.Format(_T("%d;"),port->GetBadBlockCount());
                strLog += strItem;

                strItem.Format(_T("%d;"),(int)port->GetRealSpeed(TRUE));
                strLog += strItem;

                strItem.Format(_T("%d;"),(int)port->GetRealSpeed(FALSE));
                strLog += strItem;
            }
            else
            {
                strItem.Format(_T("%s;"),port->GetHashMethodString().c_str());
                strLog += strItem;

                strItem.Format(_T("%s;"),port->GetHashString().c_str());
                strLog += strItem;

                strItem.Format(_T("%d;"),(int)port->GetRealSpeed());
                strLog += strItem;
            }

            strItem.Format(_T("%s;"),port->GetResultString().c_str());
            strLog += strItem;

            DWORD dwErrorCode = 0;
            ErrorType errType = port->GetErrorCode(&dwErrorCode);

            strItem.Format(_T("%d;"),errType);
            strLog += strItem;

            strItem.Format(_T("%d\r\n"),dwErrorCode);
            strLog += strItem;
        }
    }

    return strLog;
}



void MainWindow::SetAllFailed(DWORD dwErrorCode, ErrorType errType)
{
    if (m_MasterPort.IsConnected())
    {
        m_MasterPort.SetResult(FALSE);
        m_MasterPort.SetPortState(PortState_Fail);

        if (dwErrorCode != 0)
        {
            m_MasterPort.SetErrorCode(errType,dwErrorCode);
        }

    }

    PortList::const_iterator itPort = m_TargetPorts.begin();
    PortList::const_iterator itPort_end = m_TargetPorts.end();

    while (itPort != itPort_end)
    {
        CPort *port = (*itPort);
        itPort++;

        if (port->IsConnected())
        {
            port->SetResult(FALSE);
            port->SetPortState(PortState_Fail);

            if (dwErrorCode != 0)
            {
                port->SetErrorCode(errType,dwErrorCode);
            }

        }
    }
}

BOOL MainWindow::IsLisence()
{
    CString strListenFile = m_strAppPath + _T("\\lisence.dat");

    HANDLE hFile = open(strListenFile.c_str(),O_RDONLY);


    if (hFile < 0)
    {
        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("The lisence file doesn't exist."));
        return FALSE;
    }

    BYTE byFileKey[KEY_LEN] = {NULL};

    DWORD dwReadSize = 0;


    if (!ReadFile(hFile,byFileKey,KEY_LEN,&dwReadSize,NULL))
    {
        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Read lisence file failed."));
        CloseHandle(hFile);
        return FALSE;
    }

    if (dwReadSize != KEY_LEN)
    {
        CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Invalid lisence file."));
        CloseHandle(hFile);
        return FALSE;
    }

    CLisence lisence;

    lisence.GetLock();
    BYTE *key = lisence.GetKey();

    // 比较
    for (int i = 0; i < KEY_LEN - 8; i++)
    {
        if (key[i] != byFileKey[i])
        {
            CUtils::WriteLogFile(m_hLogFile,TRUE,_T("Invalid lisence."));
            CloseHandle(hFile);
            return FALSE;
        }
    }
    CloseHandle(hFile);
    return TRUE;
}

