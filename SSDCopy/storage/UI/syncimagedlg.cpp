#include "syncimagedlg.h"
#include "ui_syncimagedlg.h"

SyncImageDlg::SyncImageDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SyncImageDlg)
{
    ui->setupUi(this);
}

SyncImageDlg::~SyncImageDlg()
{
    delete ui;
}
