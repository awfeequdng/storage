#include "loginauthendlg.h"
#include "ui_loginauthendlg.h"

LoginAuthenDlg::LoginAuthenDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginAuthenDlg)
{
    ui->setupUi(this);
}

LoginAuthenDlg::~LoginAuthenDlg()
{
    delete ui;
}
