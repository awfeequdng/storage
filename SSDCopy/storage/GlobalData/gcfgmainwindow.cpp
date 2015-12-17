#include "gcfgmainwindow.h"

GcfgMainWindow::GcfgMainWindow()
{
    initial();
}
SelectPattern GcfgMainWindow::selectPattern() const
{
    return m_selectPattern;
}

void GcfgMainWindow::setSelectPattern(const SelectPattern &selectPattern)
{
    m_selectPattern = selectPattern;
    m_setting.setValue(Setting_SelectPattern,m_selectPattern);
}

void GcfgMainWindow::initial()
{
    m_selectPattern = (SelectPattern)m_setting.value(Setting_SelectPattern,GLOBAL_DUP).toUInt();
}


