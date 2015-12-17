#ifndef USERDEFINEITEMDLG_H
#define USERDEFINEITEMDLG_H

#include <QDialog>

namespace Ui {
class UserDefineItemDlg;
}

class UserDefineItemDlg : public QDialog
{
    Q_OBJECT

public:
    explicit UserDefineItemDlg(QWidget *parent = 0);
    ~UserDefineItemDlg();

private:
    Ui::UserDefineItemDlg *ui;
};

#endif // USERDEFINEITEMDLG_H
