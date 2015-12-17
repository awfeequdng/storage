#ifndef VIEWIMAGEDLG_H
#define VIEWIMAGEDLG_H

#include <QDialog>

namespace Ui {
class ViewImageDlg;
}

class ViewImageDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ViewImageDlg(QWidget *parent = 0);
    ~ViewImageDlg();

private:
    Ui::ViewImageDlg *ui;
};

#endif // VIEWIMAGEDLG_H
