#ifndef SELECTCURPATTERNDLG_H
#define SELECTCURPATTERNDLG_H

#include <QDialog>

namespace Ui {
class SelectCurPatternDlg;
}

class SelectCurPatternDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SelectCurPatternDlg(QWidget *parent = 0);
    ~SelectCurPatternDlg();
    int getSelPattern();
private slots:
    void on_btnQuickDup_clicked();

    void on_btnCreateImage_clicked();

    void on_btnGlobalDup_clicked();

    void on_btnDupFromImage_clicked();

    void on_btnCapConvertDup_clicked();

    void on_btnClearDisk_clicked();

    void on_btnUserDefine_clicked();

    void on_btnReturn_clicked();

private:
    Ui::SelectCurPatternDlg *ui;
//    int m_selPattern;
};

#endif // SELECTCURPATTERNDLG_H
