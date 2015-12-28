#ifndef PORTDLG_H
#define PORTDLG_H

#include <QDialog>
#include "GlobalData/GlobalDef.h"

#include "Disk/Port.h"

#include "SerialPortCommand/PortCommand.h"

#include "common/cini.h"

#include <QTimer>

namespace Ui {
class PortDlg;
}

class PortDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PortDlg(QWidget *parent = 0);
    ~PortDlg();

private:
    Ui::PortDlg *ui;

    CPort   *m_Port;
    PortList *m_PortList;
    PortCommand *m_pCommand;

    int m_nMachineType;

    BOOL m_bSize;
    BOOL m_bOnlineCommand;
    BOOL m_bStopCommand;
    CIni *m_pIni;
    HANDLE m_hLogFile;
    volatile BOOL m_bEnableKickOff;
    UINT m_nKickOffCount;
    CString m_strModel;

    QTimer m_timer;

private slots:
    void slotOnTimer();
private:
//    CToolTipCtrl m_Tooltips;

//    CFont m_font;

//    void ChangeSize( CWnd *pWnd,int cx, int cy );

    BOOL IsSlowest();

    void UpdateState();

public:
    void SetPort(CIni *pIni,HANDLE hLogFile,PortCommand *pCommand,CPort *port,PortList *pPortList,int nMachineType);
    CPort *GetPort();
    void Update(BOOL bStart = TRUE);
    void EnableKickOff(BOOL bEnable);
    void Initial();


//    void SetBitmap(UINT nResource);
//    afx_msg void OnTimer(UINT_PTR nIDEvent);
//    afx_msg void OnNMCustomdrawProgress1(NMHDR *pNMHDR, LRESULT *pResult);
//    void MoveCtrl();
    void SetModel(CString &strModel);
};

#endif // PORTDLG_H
