#include "cleardiskitemdlg.h"
#include "ui_cleardiskitemdlg.h"

ClearDiskItemDlg::ClearDiskItemDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClearDiskItemDlg)
{
    ui->setupUi(this);
}

ClearDiskItemDlg::~ClearDiskItemDlg()
{
    delete ui;
}



void ClearDiskItemDlg::on_btnAdd_clicked()
{

}

void ClearDiskItemDlg::on_btnDelete_clicked()
{

}

void ClearDiskItemDlg::on_btnOk_clicked()
{

}

void ClearDiskItemDlg::on_btnCancel_clicked()
{

}
