#include "selectcurpatterndlg.h"
#include "ui_selectcurpatterndlg.h"
#include "varisizedupitemdlg.h"
#include "globaldupitemdlg.h"
#include "capconvertdupitemdlg.h"
#include "userdefineitemdlg.h"
#include "createimageitemdlg.h"
#include "dupfromimageitemdlg.h"
#include "cleardiskitemdlg.h"
#include "GlobalData/gcfgmainwindow.h"

extern GcfgMainWindow *g_pGcfgMainWindow;

SelectCurPatternDlg::SelectCurPatternDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectCurPatternDlg)
{
    ui->setupUi(this);
}

SelectCurPatternDlg::~SelectCurPatternDlg()
{
    delete ui;
}


void SelectCurPatternDlg::on_btnQuickDup_clicked()
{
    g_pGcfgMainWindow->setWorkMode(WorkMode_QuickCopy);

    this->close();
}

void SelectCurPatternDlg::on_btnCreateImage_clicked()
{
    g_pGcfgMainWindow->setWorkMode(WorkMode_ImageMake);
    this->close();
}

void SelectCurPatternDlg::on_btnGlobalDup_clicked()
{
    g_pGcfgMainWindow->setWorkMode(WorkMode_FullCopy);
    this->close();
}

void SelectCurPatternDlg::on_btnDupFromImage_clicked()
{
    g_pGcfgMainWindow->setWorkMode(WorkMode_ImageCopy);
    this->close();
}

void SelectCurPatternDlg::on_btnCapConvertDup_clicked()
{
    g_pGcfgMainWindow->setWorkMode(WorkMode_DifferenceCopy);
    this->close();
}

void SelectCurPatternDlg::on_btnClearDisk_clicked()
{
    g_pGcfgMainWindow->setWorkMode(WorkMode_DiskClean);
    this->close();
}

void SelectCurPatternDlg::on_btnUserDefine_clicked()
{
    g_pGcfgMainWindow->setWorkMode(WorkMode_UserDefine);
    this->close();
}

void SelectCurPatternDlg::on_btnReturn_clicked()
{
    this->close();
}
