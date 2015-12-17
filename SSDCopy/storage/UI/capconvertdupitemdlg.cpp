#include "capconvertdupitemdlg.h"
#include "ui_capconvertdupitemdlg.h"

CapConvertDupItemDlg::CapConvertDupItemDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CapConvertDupItemDlg)
{
    ui->setupUi(this);
}

CapConvertDupItemDlg::~CapConvertDupItemDlg()
{
    delete ui;
}


void CapConvertDupItemDlg::on_btnOk_clicked()
{

}

void CapConvertDupItemDlg::on_btnCancel_clicked()
{

}

void CapConvertDupItemDlg::on_chkBoxReadBackCheck_toggled(bool checked)
{

}

void CapConvertDupItemDlg::on_chkBoxMultiMBR_toggled(bool checked)
{

}

void CapConvertDupItemDlg::on_cboBoxMultiMBR_currentIndexChanged(int index)
{

}
