#ifndef GCFGMAINWINDOW_H
#define GCFGMAINWINDOW_H

#include <QSettings>
#define Setting_SelectPattern "GUI/MainWindow/SelectPattern"

#include "GlobalData/GlobalDef.h"

//typedef enum _ENUM_SELECT_PATTERN{
//    QUICK_DUP = 0,
//    GLOBAL_DUP,
//    CAP_CONVERT_DUP,
//    USER_DEFINE_DUP,
//    CREATE_IMAGE,
//    DUP_FROM_IMAGE,
//    CLEAR_DISK,
//    PATTERN_NUM
//}SelectPattern;


class GcfgMainWindow
{
public:
    GcfgMainWindow();

//    SelectPattern selectPattern() const;
//    void setSelectPattern(const SelectPattern &selectPattern);
    WorkMode workMode() const;
    void setWorkMode(const WorkMode &workMode);

private:
    void initial();
private:
//    SelectPattern m_selectPattern;
    WorkMode      m_workMode;
    QSettings     m_setting;
};

#endif // GCFGMAINWINDOW_H
