#include "systemfuncdlg.h"
#include "ui_systemfuncdlg.h"
#include "globalparamsettingdlg.h"
#include "localimagemanagedlg.h"
#include "syncimagedlg.h"
#include "runbatchcmddlg.h"

SystemFuncDlg::SystemFuncDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SystemFuncDlg)
{
    ui->setupUi(this);
}

SystemFuncDlg::~SystemFuncDlg()
{
    delete ui;
}

void SystemFuncDlg::on_btnSoftwareUpdate_clicked()
{

}

void SystemFuncDlg::on_btnSyncImage_clicked()
{
    SyncImageDlg syncImage;
    syncImage.setModal(true);
//    syncImage.show();
    syncImage.exec();

}

void SystemFuncDlg::on_btnRecoverSetting_clicked()
{

}

void SystemFuncDlg::on_btnExportLog_clicked()
{

}

void SystemFuncDlg::on_btnGlobalParamSetting_clicked()
{
    GlobalParamSettingDlg globalParamSetting(this);
//    globalParamSetting.setModal(false);
    globalParamSetting.exec();

}

void SystemFuncDlg::on_btnRunBatchCmd_clicked()
{
    RunBatchCmdDlg runBatchCmd(this);
//    runBatchCmd.setModal(false);
    runBatchCmd.exec();
}

void SystemFuncDlg::on_btnLocalImageManage_clicked()
{
    LocalImageManageDlg localImageManage(this);
//    localImageManage.setModal(false);
    localImageManage.exec();
}

void SystemFuncDlg::on_btnReturn_clicked()
{

}
