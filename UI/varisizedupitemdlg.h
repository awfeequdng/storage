#ifndef VARISIZEDUPITEMDLG_H
#define VARISIZEDUPITEMDLG_H

#include <QDialog>

namespace Ui {
class VariSizeDupItemDlg;
}

class VariSizeDupItemDlg : public QDialog
{
    Q_OBJECT

public:
    explicit VariSizeDupItemDlg(QWidget *parent = 0);
    ~VariSizeDupItemDlg();

private:
    Ui::VariSizeDupItemDlg *ui;
};

#endif // VARISIZEDUPITEMDLG_H
