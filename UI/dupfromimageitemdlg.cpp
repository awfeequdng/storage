#include "dupfromimageitemdlg.h"
#include "ui_dupfromimageitemdlg.h"

DupFromImageItemDlg::DupFromImageItemDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DupFromImageItemDlg)
{
    ui->setupUi(this);
}

DupFromImageItemDlg::~DupFromImageItemDlg()
{
    delete ui;
}

void DupFromImageItemDlg::on_btnOk_clicked()
{

}

void DupFromImageItemDlg::on_btnCancel_clicked()
{

}
