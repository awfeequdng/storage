#include "portdlg.h"
#include "ui_portdlg.h"

#include "common/cutils.h"

#include <QString>
PortDlg::PortDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PortDlg)
{
    ui->setupUi(this);
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(slotOnTimer));

    m_Port = NULL;

    m_bOnlineCommand = FALSE;
    m_bStopCommand = FALSE;

    m_bEnableKickOff = FALSE;

    m_nMachineType = MT_TS;

    m_nKickOffCount = 0;
    m_bSize = FALSE;
    m_strModel = _T("");

    Initial();
}

PortDlg::~PortDlg()
{
    delete ui;
}

void PortDlg::slotOnTimer()
{
    UpdateState();
}

BOOL PortDlg::IsSlowest()
{
    // 排除母盘比较
    if (m_Port->GetPortNum() == 0 && m_Port->GetPortType() == PortType_MASTER_DISK)
    {
        return FALSE;
    }

    BOOL bCheckRelativeSpeed = m_pIni->GetBool(_T("Option"),_T("En_RelativeSpeed"),FALSE);
    UINT nRelativeSpeed = m_pIni->GetUInt(_T("Option"),_T("RelativeSpeed"),60);

    BOOL bCheckAbsoluteSpeed = m_pIni->GetBool(_T("Option"),_T("En_AbsoluteSpeed"),FALSE);
    UINT nAbsolteSpeed = m_pIni->GetUInt(_T("Option"),_T("AbsoluteSpeed"),5);

    UINT nKickOffTimes = m_pIni->GetUInt(_T("Option"),_T("KickOffTimeS"),5);

    BOOL bSlow = TRUE;
    double dbSpeed = m_Port->GetRealSpeed();
    PortList::const_iterator itPort = m_PortList->begin();
    PortList::const_iterator itPort_end = m_PortList->end();


    double dbTotolSpeed = dbSpeed;
    int nCount = 0;
    while(itPort != itPort_end)
    {
        CPort *port = (*itPort);
        itPort++;

        if (port->IsConnected() && port->GetPortState() == PortState_Active && port->GetPortNum() != m_Port->GetPortNum())
        {
            nCount++;
            dbTotolSpeed += port->GetRealSpeed();
            if (port->GetRealSpeed() < dbSpeed)
            {
                bSlow = FALSE;
            }
        }
    }

    // 只有一个没有可比性
    if (nCount == 0)
    {
        bSlow = FALSE;
    }

    if (bSlow && m_bEnableKickOff)
    {
        if (bCheckRelativeSpeed)
        {
            double dbAvgSpeed = dbTotolSpeed / (nCount + 1);
            if (dbSpeed < dbAvgSpeed * nRelativeSpeed / 100)
            {
                m_nKickOffCount++;

                if (m_nKickOffCount >= nKickOffTimes)
                {
                    m_Port->SetKickOff(TRUE);
                    m_nKickOffCount = 0;
                }
            }
            else
            {
                m_nKickOffCount = 0;
            }
        }

        if (bCheckAbsoluteSpeed)
        {
            if (dbSpeed < nAbsolteSpeed)
            {
                m_nKickOffCount++;

                if (m_nKickOffCount >= nKickOffTimes)
                {
                    m_Port->SetKickOff(TRUE);
                    m_nKickOffCount = 0;
                }

            }
            else
            {
                m_nKickOffCount = 0;
            }
        }
    }

    return bSlow;
}

void PortDlg::UpdateState()
{
    CString strSpeed,strSize,strSN,strTips,strUSBType,strReadSpeed,strWriteSpeed,strBadBlock,strRealSize;
    CULLArray ullBadBlockArray;
    int iPercent = 0;
//    UINT nID  = IDB_GRAY;
    QString strBitmap = QString(":/icon/image/NGFF_GRAY.bmp");

    switch (m_Port->GetPortState())
    {
    case PortState_Offline:
        strBitmap = QString(":/icon/image/NGFF_GRAY.bmp");

        strSpeed = _T("");
        strSize = _T("");
        strSN = _T("");
        strTips = _T("Offline");
        strUSBType = _T("");
        iPercent = 0;

        if (m_bOnlineCommand)
        {
//            m_pCommand->RedGreenLight(m_Port->GetPortNum(),FALSE);
//            m_bOnlineCommand = FALSE;
        }

        break;

    case PortState_Online:
        strBitmap = QString(":/icon/image/NGFF_NORMAL.bmp");
        strSpeed = _T("");
        strSize = CUtils::AdjustFileSize(m_Port->GetTotalSize());
        strSN = m_Port->GetSN();
        strTips.Format(_T("Online\r\nModel:%s\r\nSN:%s\r\n"),m_Port->GetModuleName().c_str(),m_Port->GetSN().c_str());
        iPercent = 0;
//        strUSBType = m_Port->GetUsbType();

        if (!m_bOnlineCommand)
        {
            //  红绿灯亮之前，先清掉红和绿的命令···
//            m_pCommand->GreenLight(m_Port->GetPortNum(),FALSE);
//            m_pCommand->RedLight(m_Port->GetPortNum(), FALSE);

//            m_pCommand->RedGreenLight(m_Port->GetPortNum(),TRUE);
//            m_bOnlineCommand = TRUE;
        }
        ui->labRdSpdVal->setText(strSpeed.c_str());

        break;

    case PortState_Active:
    case PortState_Stop:
        if (IsSlowest())
        {
            strBitmap = QString(":/icon/image/NGFF_YELLOW.bmp");
        }
        else
        {
            strBitmap = QString(":/icon/image/NGFF_NORMAL.bmp");
        }

        strSpeed = m_Port->GetRealSpeedString();
        strSize = CUtils::AdjustFileSize(m_Port->GetTotalSize());
        strSN = m_Port->GetSN();
        iPercent = m_Port->GetPercent();
//        strUSBType = m_Port->GetUsbType();

        strTips.Format(_T("Running\r\nModel:%s\r\nSN:%s\r\n"),m_Port->GetModuleName().c_str(),m_Port->GetSN().c_str());
        if (m_Port->GetWorkMode() == WorkMode_Full_RW_Test
            || m_Port->GetWorkMode() == WorkMode_Fade_Picker
            || m_Port->GetWorkMode() == WorkMode_Speed_Check
            )
        {
            strReadSpeed.Format(_T("R:%s"),m_Port->GetRealSpeedString(TRUE).c_str());
            strWriteSpeed.Format(_T("W:%s"),m_Port->GetRealSpeedString(FALSE).c_str());

            ui->labRdSpdVal->setText(strReadSpeed.c_str());
            ui->labWrSpdVal->setText(strWriteSpeed.c_str());
//            SetDlgItemText(IDC_TEXT_SPEED_R,strReadSpeed);
//            SetDlgItemText(IDC_TEXT_SPEED_W,strWriteSpeed);
        }
        else
        {
//            SetDlgItemText(IDC_TEXT_SPEED_R,strSpeed);
            ui->labRdSpdVal->setText(strSpeed.c_str());
        }

        break;

    case PortState_Pass:

        strBitmap = QString(":/icon/image/NGFF_GREEN.bmp");
        strSpeed = m_Port->GetRealSpeedString();
        strSize = CUtils::AdjustFileSize(m_Port->GetTotalSize());
        strSN = m_Port->GetSN();
        iPercent = 100;
        strUSBType = m_Port->GetUsbType();

        strTips.Format(_T("PASS\r\nModel:%s\r\nSN:%s\r\n"),m_Port->GetModuleName().c_str(),m_Port->GetSN().c_str());

        if (!m_bStopCommand)
        {
//            m_pCommand->GreenLight(m_Port->GetPortNum(),TRUE);

//            // 如果是NGFF,红灯亮起的时候需要断电
//            if (m_nMachineType == MT_NGFF)
//            {
//                Sleep(100);
//                m_pCommand->Power(m_Port->GetPortNum(),FALSE);
//            }

//            m_bStopCommand = TRUE;
        }

        if (m_Port->GetWorkMode() == WorkMode_Full_RW_Test
            || m_Port->GetWorkMode() == WorkMode_Fade_Picker)
        {
            ui->labRdSpdVal->setText(strSize.c_str());
            ui->labWrSpdVal->setText("");
        }
        else if (m_Port->GetWorkMode() == WorkMode_Speed_Check)
        {
            strReadSpeed.Format(_T("R:%s"),m_Port->GetRealSpeedString(TRUE).c_str());
            strWriteSpeed.Format(_T("W:%s"),m_Port->GetRealSpeedString(FALSE).c_str());

            ui->labRdSpdVal->setText(strReadSpeed.c_str());
            ui->labWrSpdVal->setText(strWriteSpeed.c_str());
        }
        else
        {
            ui->labRdSpdVal->setText(strSpeed.c_str());
        }

        break;

    case PortState_Fail:
        strBitmap = QString(":/icon/image/NGFF_RED.bmp");
        strSpeed = m_Port->GetRealSpeedString();
        strSize = CUtils::AdjustFileSize(m_Port->GetTotalSize());
        strSN = m_Port->GetSN();
        iPercent = m_Port->GetPercent();
        strTips.Format(_T("FAIL\r\nModel:%s\r\nSN:%s\r\n"),m_Port->GetModuleName().c_str(),m_Port->GetSN().c_str());
        strUSBType = m_Port->GetUsbType();

        if (!m_bStopCommand)
        {
//            m_pCommand->RedLight(m_Port->GetPortNum(),TRUE);

//            // 如果是NGFF,红灯亮起的时候需要断电
//            if (m_nMachineType == MT_NGFF)
//            {
//                Sleep(100);
//                m_pCommand->Power(m_Port->GetPortNum(),FALSE);
//            }

//            m_bStopCommand = TRUE;
        }

        if (m_Port->GetWorkMode() == WorkMode_Full_RW_Test
            || m_Port->GetWorkMode() == WorkMode_Fade_Picker )
        {
            int nBadCount = m_Port->GetBadBlockCount();

            if (nBadCount > 0)
            {
                m_Port->GetBadBlockArray(ullBadBlockArray);

                strBadBlock.Format(_T("B:%d"),nBadCount);
                strRealSize = CUtils::AdjustFileSize(ullBadBlockArray.at(0) * m_Port->GetBytesPerSector());

                ui->labRdSpdVal->setText(strRealSize.c_str());
                ui->labWrSpdVal->setText(strBadBlock.c_str());
            }
            else
            {
                strReadSpeed.Format(_T("R:%s"),m_Port->GetRealSpeedString(TRUE).c_str());
                strWriteSpeed.Format(_T("W:%s"),m_Port->GetRealSpeedString(FALSE).c_str());


                ui->labRdSpdVal->setText(strReadSpeed.c_str());
                ui->labWrSpdVal->setText(strWriteSpeed.c_str());
            }

        }
        else if (m_Port->GetWorkMode() == WorkMode_Speed_Check)
        {
            strReadSpeed.Format(_T("R:%s"),m_Port->GetRealSpeedString(TRUE).c_str());
            strWriteSpeed.Format(_T("W:%s"),m_Port->GetRealSpeedString(FALSE).c_str());

            ui->labRdSpdVal->setText(strReadSpeed.c_str());
            ui->labWrSpdVal->setText(strWriteSpeed.c_str());

            iPercent = 100;
        }
        else
        {
            ui->labRdSpdVal->setText(strSpeed.c_str());
        }

        break;
    default:
        break;
    }

    ui->labPort->setPixmap(QPixmap(strBitmap));
    ui->labCapVal->setText(strSize.c_str());
    ui->labSnVal->setText(strSN.c_str());

    ui->progressBar->setValue(iPercent);

    ui->labPortTagVal->setText(m_Port->GetPortName().c_str());
//    SetDlgItemText(IDC_GROUP_PORT,m_Port->GetPortName());

//    m_Tooltips.UpdateTipText(strTips,&m_PictureCtrol);
}

void PortDlg::SetPort(CIni *pIni, HANDLE hLogFile, PortCommand *pCommand, CPort *port, PortList *pPortList, int nMachineType)
{
    m_pCommand = pCommand;
    m_Port = port;
    m_PortList = pPortList;
    m_pIni = pIni;
    m_hLogFile = hLogFile;
    m_nMachineType = nMachineType;
}

CPort *PortDlg::GetPort()
{
    return m_Port;
}

void PortDlg::Update(BOOL bStart)
{
    if (bStart)
    {
//        SetTimer(TIMER_UPDATE,1000,NULL);
        if(!m_timer.isActive())
        {
            m_timer.start(1000);
        }
    }
    else
    {
//        KillTimer(TIMER_UPDATE);
        if(m_timer.isActive())
        {
            m_timer.stop();
        }
        UpdateState();
    }
}

void PortDlg::EnableKickOff(BOOL bEnable)
{
    m_bEnableKickOff = bEnable;
}

void PortDlg::Initial()
{
    switch (m_nMachineType)
    {
    case MT_TS:
//        nID = IDB_GRAY;
        break;

    case MT_USB:
//        nID = IDB_USB_GRAY;
        break;

    case MT_NGFF:
//        nID = IDB_NGFF_GRAY;
        break;
    case MT_CF:
//        nID = IDB_CF_GRAY;
        break;

    }

//    m_Bitmap.LoadBitmap(nID);

//    m_PictureCtrol.SetBitmap(m_Bitmap);

    ui->labPort->setPixmap(QPixmap(":/icon/image/NGFF_YELLOW.bmp"));
    ui->progressBar->setValue(0);

//    SetDlgItemText(IDC_TEXT_SPEED_R,_T(""));
//    SetDlgItemText(IDC_TEXT_SPEED_W,_T(""));
//    SetDlgItemText(IDC_TEXT_SIZE,_T(""));
//    SetDlgItemText(IDC_TEXT_SN,_T(""));

//    m_ProgressCtrl.SetPos(0);

    m_bOnlineCommand = FALSE;
    m_bStopCommand = FALSE;

    m_nKickOffCount = 0;

//    Invalidate(TRUE);
}

void PortDlg::SetModel(CString &strModel)
{
    m_strModel = strModel;
}
