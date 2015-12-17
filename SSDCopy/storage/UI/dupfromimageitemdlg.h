#ifndef DUPFROMIMAGEITEMDLG_H
#define DUPFROMIMAGEITEMDLG_H

#include <QDialog>

namespace Ui {
class DupFromImageItemDlg;
}

class DupFromImageItemDlg : public QDialog
{
    Q_OBJECT

public:
    explicit DupFromImageItemDlg(QWidget *parent = 0);
    ~DupFromImageItemDlg();

private slots:
    void on_btnOk_clicked();

    void on_btnCancel_clicked();

private:
    Ui::DupFromImageItemDlg *ui;
};

#endif // DUPFROMIMAGEITEMDLG_H
