#ifndef SERIALPORTSETTING_H
#define SERIALPORTSETTING_H
#include <QString>
#include <QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
//class QSerialPort;
//class QIntValidator;
//QT_USE_NAMESPACE

class SerialPortSetting
{

public:
    struct Settings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        bool localEchoEnabled;
    };
    SerialPortSetting();



    Settings settings() const;


//    void showPortInfo(int idx)
//    {
//        if (idx != -1) {
//            //        QStringList list = ui->serialPortInfoListBox->itemData(idx).toStringList();
//            //        ui->descriptionLabel->setText(tr("Description: %1").arg(list.at(1)));
//            //        ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.at(2)));
//            //        ui->locationLabel->setText(tr("Location: %1").arg(list.at(3)));
//            //        ui->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.at(4)));
//            //        ui->pidLabel->setText(tr("Product Identifier: %1").arg(list.at(5)));
//        }
//    }
    void apply();
    void checkCustomBaudRatePolicy(int idx);

private:
    void fillPortsParameters();
    void fillPortsInfo();
    void updateSettings();

private:
    Settings currentSettings;
//    QIntValidator *intValidator;
};

#endif // SERIALPORTSETTING_H
