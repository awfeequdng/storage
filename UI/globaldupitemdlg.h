#ifndef GLOBALDUPITEMDLG_H
#define GLOBALDUPITEMDLG_H

#include <QDialog>

namespace Ui {
class GlobalDupItemDlg;
}

class GlobalDupItemDlg : public QDialog
{
    Q_OBJECT

public:
    explicit GlobalDupItemDlg(QWidget *parent = 0);
    ~GlobalDupItemDlg();

private:
    Ui::GlobalDupItemDlg *ui;
};

#endif // GLOBALDUPITEMDLG_H
