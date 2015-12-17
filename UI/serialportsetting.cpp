#include "serialportsetting.h"

//#include <QtSerialPort/QSerialPortInfo>
#include <QIntValidator>
#include <QLineEdit>
//QT_USE_NAMESPACE
SerialPortSetting::SerialPortSetting()
{

//    intValidator = new QIntValidator(0, 4000000, this);

//    ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

//    connect(ui->applyButton, SIGNAL(clicked()),
//            this, SLOT(apply()));
//    connect(ui->serialPortInfoListBox, SIGNAL(currentIndexChanged(int)),
//            this, SLOT(showPortInfo(int)));
//    connect(ui->baudRateBox, SIGNAL(currentIndexChanged(int)),
//            this, SLOT(checkCustomBaudRatePolicy(int)));

    fillPortsParameters();
    fillPortsInfo();

    updateSettings();
}

SerialPortSetting::Settings SerialPortSetting::settings() const
{
    return currentSettings;
}





//void SerialPortSetting::apply()
//{
//    updateSettings();
//}

void SerialPortSetting::checkCustomBaudRatePolicy(int idx)
{
    //    bool isCustomBaudRate = !ui->baudRateBox->itemData(idx).isValid();
    //    ui->baudRateBox->setEditable(isCustomBaudRate);
    //    if (isCustomBaudRate) {
    //        ui->baudRateBox->clearEditText();
    //        QLineEdit *edit = ui->baudRateBox->lineEdit();
    //        edit->setValidator(intValidator);
    //    }
}

void SerialPortSetting::fillPortsParameters()
{

}

void SerialPortSetting::fillPortsInfo()
{
    //    ui->serialPortInfoListBox->clear();
    static const QString blankString = QObject::tr("N/A");
        QString description;
        QString manufacturer;
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
            QStringList list;
            description = info.description();
            manufacturer = info.manufacturer();
            list << info.portName()
                 << (!description.isEmpty() ? description : blankString)
                 << (!manufacturer.isEmpty() ? manufacturer : blankString)
                 << info.systemLocation()
                 << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
                 << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

    //        ui->serialPortInfoListBox->addItem(list.first(), list);
        }
}

void SerialPortSetting::updateSettings()
{

//    currentSettings.name = ui->serialPortInfoListBox->currentText();

//    // Baud Rate
//    if (ui->baudRateBox->currentIndex() == 4) {
//        // custom baud rate
//        currentSettings.baudRate = ui->baudRateBox->currentText().toInt();
//    } else {
//        // standard baud rate
//        currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(
//                    ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
//    }
//    currentSettings.stringBaudRate = QString::number(currentSettings.baudRate);

//    // Data bits
//    currentSettings.dataBits = static_cast<QSerialPort::DataBits>(
//                ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
//    currentSettings.stringDataBits = ui->dataBitsBox->currentText();

//    // Parity
//    currentSettings.parity = static_cast<QSerialPort::Parity>(
//                ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
//    currentSettings.stringParity = ui->parityBox->currentText();

//    // Stop bits
//    currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
//                ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
//    currentSettings.stringStopBits = ui->stopBitsBox->currentText();

//    // Flow control
//    currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
//                ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
//    currentSettings.stringFlowControl = ui->flowControlBox->currentText();

//    // Additional options
//    currentSettings.localEchoEnabled = ui->localEchoCheckBox->isChecked();
}
