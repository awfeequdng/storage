#ifndef SYNCHRONIZINGDLG_H
#define SYNCHRONIZINGDLG_H

#include <QDialog>

namespace Ui {
class SynchronizingDlg;
}

class SynchronizingDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SynchronizingDlg(QWidget *parent = 0);
    ~SynchronizingDlg();

private:
    Ui::SynchronizingDlg *ui;
};

#endif // SYNCHRONIZINGDLG_H
