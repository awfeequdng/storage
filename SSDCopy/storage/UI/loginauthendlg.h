#ifndef LOGINAUTHENDLG_H
#define LOGINAUTHENDLG_H

#include <QDialog>

namespace Ui {
class LoginAuthenDlg;
}

class LoginAuthenDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LoginAuthenDlg(QWidget *parent = 0);
    ~LoginAuthenDlg();

private:
    Ui::LoginAuthenDlg *ui;
};

#endif // LOGINAUTHENDLG_H
