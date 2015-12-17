#include "globalparamsettingdlg.h"
#include "ui_globalparamsettingdlg.h"

GlobalParamSettingDlg::GlobalParamSettingDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GlobalParamSettingDlg)
{
    ui->setupUi(this);
}

GlobalParamSettingDlg::~GlobalParamSettingDlg()
{
    delete ui;
}
