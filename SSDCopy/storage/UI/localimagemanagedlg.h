#ifndef LOCALIMAGEMANAGEDLG_H
#define LOCALIMAGEMANAGEDLG_H

#include <QDialog>

namespace Ui {
class LocalImageManageDlg;
}

class LocalImageManageDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LocalImageManageDlg(QWidget *parent = 0);
    ~LocalImageManageDlg();

private:
    Ui::LocalImageManageDlg *ui;
};

#endif // LOCALIMAGEMANAGEDLG_H
