#ifndef CREATEIMAGEITEMDLG_H
#define CREATEIMAGEITEMDLG_H

#include <QDialog>

namespace Ui {
class CreateImageItemDlg;
}

class CreateImageItemDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CreateImageItemDlg(QWidget *parent = 0);
    ~CreateImageItemDlg();

private slots:
    void on_btnOK_clicked();

    void on_btnCancel_clicked();

private:
    Ui::CreateImageItemDlg *ui;
};

#endif // CREATEIMAGEITEMDLG_H
