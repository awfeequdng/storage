#include "localimagemanagedlg.h"
#include "ui_localimagemanagedlg.h"

LocalImageManageDlg::LocalImageManageDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LocalImageManageDlg)
{
    ui->setupUi(this);
}

LocalImageManageDlg::~LocalImageManageDlg()
{
    delete ui;
}
