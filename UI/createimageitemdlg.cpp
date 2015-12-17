#include "createimageitemdlg.h"
#include "ui_createimageitemdlg.h"

CreateImageItemDlg::CreateImageItemDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateImageItemDlg)
{
    ui->setupUi(this);
}

CreateImageItemDlg::~CreateImageItemDlg()
{
    delete ui;
}

void CreateImageItemDlg::on_btnOK_clicked()
{

}

void CreateImageItemDlg::on_btnCancel_clicked()
{

}
