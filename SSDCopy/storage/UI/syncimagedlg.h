#ifndef SYNCIMAGEDLG_H
#define SYNCIMAGEDLG_H

#include <QDialog>

namespace Ui {
class SyncImageDlg;
}

class SyncImageDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SyncImageDlg(QWidget *parent = 0);
    ~SyncImageDlg();

private:
    Ui::SyncImageDlg *ui;
};

#endif // SYNCIMAGEDLG_H
