#include "synchronizingdlg.h"
#include "ui_synchronizingdlg.h"

SynchronizingDlg::SynchronizingDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SynchronizingDlg)
{
    ui->setupUi(this);
}

SynchronizingDlg::~SynchronizingDlg()
{
    delete ui;
}
