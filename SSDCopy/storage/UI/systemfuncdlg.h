#ifndef SYSTEMFUNCDLG_H
#define SYSTEMFUNCDLG_H

#include <QDialog>

namespace Ui {
class SystemFuncDlg;
}

class SystemFuncDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SystemFuncDlg(QWidget *parent = 0);
    ~SystemFuncDlg();

private slots:
    void on_btnSoftwareUpdate_clicked();

    void on_btnSyncImage_clicked();

    void on_btnRecoverSetting_clicked();

    void on_btnExportLog_clicked();

    void on_btnGlobalParamSetting_clicked();

    void on_btnRunBatchCmd_clicked();

    void on_btnLocalImageManage_clicked();

    void on_btnReturn_clicked();

private:
    Ui::SystemFuncDlg *ui;
};

#endif // SYSTEMFUNCDLG_H
