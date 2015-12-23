#-------------------------------------------------
#
# Project created by QtCreator 2015-12-05T17:07:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

TARGET = SSDCopy
TEMPLATE = app

CONFIG += c++11

#LIBS +=-L/usr/local/lib/
LIBS +=/usr/local/lib/libz.a

SOURCES += main.cpp\
        mainwindow.cpp \
    SerialPortCommand/PortCommand.cpp \
    GlobalData/gcfgglobalparamsetting.cpp \
    GlobalData/gcfglocalimagemanage.cpp \
    GlobalData/gcfgsyncimage.cpp \
    GlobalData/gcfgrunbatchcmd.cpp \
    GlobalData/gcfgmakeimage.cpp \
    GlobalData/gcfgfulldiskdup.cpp \
    GlobalData/gcfgdupfromimage.cpp \
    GlobalData/gcfgcapconvertdup.cpp \
    GlobalData/gcfgcleandisk.cpp \
    GlobalData/gcfguserdefineddup.cpp \
    GlobalData/gcfgquickdup.cpp \
    GlobalData/gcfgmainwindow.cpp \
    net/netcommunication.cpp \
    Disk/disk.cpp \
    HashMethod/Checksum32.cpp \
    HashMethod/CRC32.cpp \
    HashMethod/HashMethod.cpp \
    HashMethod/MD5.cpp \
    common/cini.cpp \
    common/cstring.cpp \
    common/cutils.cpp \
    Disk/DataQueue.cpp \
    Disk/Port.cpp \
    UI/capconvertdupitemdlg.cpp \
    UI/cleardiskitemdlg.cpp \
    UI/createimageitemdlg.cpp \
    UI/dupfromimageitemdlg.cpp \
    UI/globaldupitemdlg.cpp \
    UI/globalparamsettingdlg.cpp \
    UI/localimagemanagedlg.cpp \
    UI/loginauthendlg.cpp \
    UI/runbatchcmddlg.cpp \
    UI/selectcurpatterndlg.cpp \
    UI/serialportsetting.cpp \
    UI/softwareupdateitemdlg.cpp \
    UI/synchronizingdlg.cpp \
    UI/syncimagedlg.cpp \
    UI/systemfuncdlg.cpp \
    UI/userdefineitemdlg.cpp \
    UI/varisizedupitemdlg.cpp \
    UI/viewimagedlg.cpp \
    Disk/makeimagedisk.cpp \
    Thread/mythread.cpp \
    common/PrintMacros.cpp

HEADERS  += mainwindow.h \
    SerialPortCommand/PortCommand.h \
    GlobalData/GlobalDef.h \
    GlobalData/gcfgglobalparamsetting.h \
    GlobalData/gcfglocalimagemanage.h \
    GlobalData/gcfgsyncimage.h \
    GlobalData/gcfgrunbatchcmd.h \
    GlobalData/gcfgmakeimage.h \
    GlobalData/gcfgfulldiskdup.h \
    GlobalData/gcfgdupfromimage.h \
    GlobalData/gcfgcapconvertdup.h \
    GlobalData/gcfgcleandisk.h \
    GlobalData/gcfguserdefineddup.h \
    GlobalData/gcfgquickdup.h \
    GlobalData/gcfgmainwindow.h \
    net/netcommunication.h \
    HashMethod/Checksum32.h \
    HashMethod/CRC32.h \
    HashMethod/HashMethod.h \
    HashMethod/MD5.h \
    common/cini.h \
    common/cstring.h \
    common/cutils.h \
    Disk/DataQueue.h \
    Disk/Port.h \
    UI/capconvertdupitemdlg.h \
    UI/cleardiskitemdlg.h \
    UI/createimageitemdlg.h \
    UI/dupfromimageitemdlg.h \
    UI/globaldupitemdlg.h \
    UI/globalparamsettingdlg.h \
    UI/localimagemanagedlg.h \
    UI/loginauthendlg.h \
    UI/runbatchcmddlg.h \
    UI/selectcurpatterndlg.h \
    UI/serialportsetting.h \
    UI/softwareupdateitemdlg.h \
    UI/synchronizingdlg.h \
    UI/syncimagedlg.h \
    UI/systemfuncdlg.h \
    UI/userdefineitemdlg.h \
    UI/varisizedupitemdlg.h \
    UI/viewimagedlg.h \
    config.h \
    Disk/makeimagedisk.h \
    Thread/mythread.h \
    Disk/disk.h \
    common/PrintMacros.h

FORMS    += mainwindow.ui \
    UI/capconvertdupitemdlg.ui \
    UI/capconvertdupitemdlg1.ui \
    UI/cleardiskitemdlg.ui \
    UI/ClearDiskItemDlg1.ui \
    UI/createimageitemdlg.ui \
    UI/createimageitemdlg1.ui \
    UI/dupfromimageitemdlg.ui \
    UI/DupFromImageItemDlg1.ui \
    UI/globaldupitemdlg.ui \
    UI/GlobalDupItemDlg1.ui \
    UI/globalparamsettingdlg.ui \
    UI/GlobalParamSettingDlg1.ui \
    UI/localimagemanagedlg.ui \
    UI/LocalImageManageDlg1.ui \
    UI/loginauthendlg.ui \
    UI/LoginAuthenDlg1.ui \
    UI/runbatchcmddlg.ui \
    UI/RunBatchCmdDlg1.ui \
    UI/selectcurpatterndlg.ui \
    UI/SelectCurPatternDlg1.ui \
    UI/softwareupdateitemdlg.ui \
    UI/SoftwareUpdateItemDlg1.ui \
    UI/synchronizingdlg.ui \
    UI/SynchronizingDlg1.ui \
    UI/syncimagedlg.ui \
    UI/SyncImageDlg1.ui \
    UI/systemfuncdlg.ui \
    UI/SystemFuncDlg1.ui \
    UI/UserDefinedItemDlg1.ui \
    UI/userdefineitemdlg.ui \
    UI/varisizedupitemdlg.ui \
    UI/VariSizeDupItemDlg1.ui \
    UI/viewimagedlg.ui \
    UI/ViewImageDlg1.ui

RESOURCES += \
    resource.qrc
