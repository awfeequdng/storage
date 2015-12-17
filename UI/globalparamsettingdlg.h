#ifndef GLOBALPARAMSETTINGDLG_H
#define GLOBALPARAMSETTINGDLG_H

#include <QDialog>

namespace Ui {
class GlobalParamSettingDlg;
}

class GlobalParamSettingDlg : public QDialog
{
    Q_OBJECT

public:
    explicit GlobalParamSettingDlg(QWidget *parent = 0);
    ~GlobalParamSettingDlg();

private:
    Ui::GlobalParamSettingDlg *ui;
};

#endif // GLOBALPARAMSETTINGDLG_H
