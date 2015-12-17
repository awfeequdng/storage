#ifndef CAPCONVERTDUPITEMDLG_H
#define CAPCONVERTDUPITEMDLG_H

#include <QDialog>

namespace Ui {
class CapConvertDupItemDlg;
}

class CapConvertDupItemDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CapConvertDupItemDlg(QWidget *parent = 0);
    ~CapConvertDupItemDlg();

private slots:


    void on_btnOk_clicked();

    void on_btnCancel_clicked();

    void on_chkBoxReadBackCheck_toggled(bool checked);

    void on_chkBoxMultiMBR_toggled(bool checked);

    void on_cboBoxMultiMBR_currentIndexChanged(int index);

private:
    Ui::CapConvertDupItemDlg *ui;
};

#endif // CAPCONVERTDUPITEMDLG_H
