#include "varisizedupitemdlg.h"
#include "ui_varisizedupitemdlg.h"

VariSizeDupItemDlg::VariSizeDupItemDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VariSizeDupItemDlg)
{
    ui->setupUi(this);
}

VariSizeDupItemDlg::~VariSizeDupItemDlg()
{
    delete ui;
}
