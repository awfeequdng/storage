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
    g_pGcfgMainWindow->setSelectPattern(QUICK_DUP);
    this->close();
}

void SelectCurPatternDlg::on_btnCreateImage_clicked()
{
    g_pGcfgMainWindow->setSelectPattern(CREATE_IMAGE);
    this->close();
}

void SelectCurPatternDlg::on_btnGlobalDup_clicked()
{
    g_pGcfgMainWindow->setSelectPattern(GLOBAL_DUP);
    this->close();
}

void SelectCurPatternDlg::on_btnDupFromImage_clicked()
{
    g_pGcfgMainWindow->setSelectPattern(DUP_FROM_IMAGE);
    this->close();
}

void SelectCurPatternDlg::on_btnCapConvertDup_clicked()
{
    g_pGcfgMainWindow->setSelectPattern(CAP_CONVERT_DUP);
    this->close();
}

void SelectCurPatternDlg::on_btnClearDisk_clicked()
{
    g_pGcfgMainWindow->setSelectPattern(CLEAR_DISK);
    this->close();
}

void SelectCurPatternDlg::on_btnUserDefine_clicked()
{
    g_pGcfgMainWindow->setSelectPattern(USER_DEFINE_DUP);
    this->close();
}

void SelectCurPatternDlg::on_btnReturn_clicked()
{
    this->close();
}
