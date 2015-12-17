#include "softwareupdateitemdlg.h"
#include "ui_softwareupdateitemdlg.h"

SoftwareUpdateItemDlg::SoftwareUpdateItemDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SoftwareUpdateItemDlg)
{
    ui->setupUi(this);
}

SoftwareUpdateItemDlg::~SoftwareUpdateItemDlg()
{
    delete ui;
}
