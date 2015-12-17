#include "viewimagedlg.h"
#include "ui_viewimagedlg.h"

ViewImageDlg::ViewImageDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewImageDlg)
{
    ui->setupUi(this);
}

ViewImageDlg::~ViewImageDlg()
{
    delete ui;
}
