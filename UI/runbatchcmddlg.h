#ifndef RUNBATCHCMDDLG_H
#define RUNBATCHCMDDLG_H

#include <QDialog>

namespace Ui {
class RunBatchCmdDlg;
}

class RunBatchCmdDlg : public QDialog
{
    Q_OBJECT

public:
    explicit RunBatchCmdDlg(QWidget *parent = 0);
    ~RunBatchCmdDlg();

private:
    Ui::RunBatchCmdDlg *ui;
};

#endif // RUNBATCHCMDDLG_H
