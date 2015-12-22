#include "mainwindow.h"
#include <QApplication>
#include <QString>
#include "GlobalData/gcfgcapconvertdup.h"
#include "GlobalData/gcfgcleandisk.h"
#include "GlobalData/gcfgdupfromimage.h"
#include "GlobalData/gcfgfulldiskdup.h"
#include "GlobalData/gcfgglobalparamsetting.h"
#include "GlobalData/gcfglocalimagemanage.h"
#include "GlobalData/gcfgmainwindow.h"
#include "GlobalData/gcfgmakeimage.h"
#include "GlobalData/gcfgquickdup.h"
#include "GlobalData/gcfgrunbatchcmd.h"

#include "common/cini.h"
#include "common/cutils.h"
GcfgCapConvertDup   *g_pGcfgCapConvertDup;
GcfgCleanDisk       *g_pGcfgCleanDisk;
GcfgDupFromImage    *g_pGcfgDupFromImage;
GcfgFullDiskDup     *g_pGcfgFullDiskDup;
GcfgGlobalParamSetting  *g_pGcfgGlobalParamSetting;
GcfgLocalImageManage    *g_pGcfgLocalImageManage;
GcfgMainWindow      *g_pGcfgMainWindow;
GcfgMakeImage       *g_pGcfgMakeImage;
GcfgQuickDup        *g_pGcfgQuickDup;
GcfgRunBatchCmd     *g_pGcfgRunBatchCmd;

void initData(void);
void releaseData(void);


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    initData();

    MainWindow w;
    w.show();

    CIni config;
    config.SetPathName("/home/caipengxiang/test.ini");
    config.WriteDouble("test","cai",2.3);
    std::cout<<CUtils::GetAppPath()<<std::endl;
    std::cout<<CUtils::GetFilePathWithoutName(CUtils::GetAppPath())<<std::endl;
    int ret = a.exec();

    releaseData();

    return ret;
}


void initData(void)
{
    g_pGcfgCapConvertDup  = new GcfgCapConvertDup();
    g_pGcfgCleanDisk      = new GcfgCleanDisk()    ;
    g_pGcfgDupFromImage  = new GcfgDupFromImage() ;
    g_pGcfgFullDiskDup   = new GcfgFullDiskDup()  ;
    g_pGcfgGlobalParamSetting    = new GcfgGlobalParamSetting();
    g_pGcfgLocalImageManage      = new GcfgLocalImageManage()  ;
    g_pGcfgMainWindow    = new GcfgMainWindow()   ;
    g_pGcfgMakeImage     = new GcfgMakeImage()    ;
    g_pGcfgQuickDup      = new GcfgQuickDup()     ;
    g_pGcfgRunBatchCmd   = new GcfgRunBatchCmd()  ;
}

void releaseData(void)
{
    delete g_pGcfgCapConvertDup  ;
    delete g_pGcfgCleanDisk      ;
    delete g_pGcfgDupFromImage  ;
    delete g_pGcfgFullDiskDup   ;
    delete g_pGcfgGlobalParamSetting;
    delete g_pGcfgLocalImageManage  ;
    delete g_pGcfgMainWindow    ;
    delete g_pGcfgMakeImage     ;
    delete g_pGcfgQuickDup      ;
    delete g_pGcfgRunBatchCmd   ;
}
