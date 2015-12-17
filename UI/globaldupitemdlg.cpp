#include "globaldupitemdlg.h"
#include "ui_globaldupitemdlg.h"

GlobalDupItemDlg::GlobalDupItemDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GlobalDupItemDlg)
{
    ui->setupUi(this);
}

GlobalDupItemDlg::~GlobalDupItemDlg()
{
    delete ui;
}
