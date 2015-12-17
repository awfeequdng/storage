#ifndef CLEARDISKITEMDLG_H
#define CLEARDISKITEMDLG_H

#include <QDialog>

namespace Ui {
class ClearDiskItemDlg;
}

class ClearDiskItemDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ClearDiskItemDlg(QWidget *parent = 0);
    ~ClearDiskItemDlg();

private slots:


    void on_btnAdd_clicked();

    void on_btnDelete_clicked();

    void on_btnOk_clicked();

    void on_btnCancel_clicked();

private:
    Ui::ClearDiskItemDlg *ui;
};

#endif // CLEARDISKITEMDLG_H
