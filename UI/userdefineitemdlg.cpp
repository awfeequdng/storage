#include "userdefineitemdlg.h"
#include "ui_userdefineitemdlg.h"

UserDefineItemDlg::UserDefineItemDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserDefineItemDlg)
{
    ui->setupUi(this);
}

UserDefineItemDlg::~UserDefineItemDlg()
{
    delete ui;
}
