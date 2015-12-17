#include "runbatchcmddlg.h"
#include "ui_runbatchcmddlg.h"

RunBatchCmdDlg::RunBatchCmdDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RunBatchCmdDlg)
{
    ui->setupUi(this);
}

RunBatchCmdDlg::~RunBatchCmdDlg()
{
    delete ui;
}
