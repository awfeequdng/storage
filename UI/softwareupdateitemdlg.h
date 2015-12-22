#ifndef SOFTWAREUPDATEITEMDLG_H
#define SOFTWAREUPDATEITEMDLG_H

#include <QDialog>

namespace Ui {
class SoftwareUpdateItemDlg;
}

class SoftwareUpdateItemDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SoftwareUpdateItemDlg(QWidget *parent = 0);
    ~SoftwareUpdateItemDlg();

private slots:
    void on_listView_clicked(const QModelIndex &index);

private:
    Ui::SoftwareUpdateItemDlg *ui;
};

#endif // SOFTWAREUPDATEITEMDLG_H
