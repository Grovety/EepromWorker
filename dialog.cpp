#include "dialog.h"
#include "ui_dialog.h"
#include <QMessageBox>
#include <QDebug>
#include "networkparamseditor.h"

bool Dialog::ReadEeprom(uint32_t addr, int size, QByteArray &data)
{

    QString ip = ui->m_clientIp->text();
    QHostAddress hostAddr (ip);

    QUdpSocket socket (this);

    int offset = 0;
    data.resize(size);
    while (size > 0)
    {
        uint32_t argAddr = addr;
        socket.writeDatagram((const char*)(&argAddr),4,hostAddr,0xeee0);

        QTimer timeOut;
        timeOut.start (1000);
        while (true)
        {
            if (timeOut.remainingTime()==0)
            {
                return false;
            }
            if (eepromReadSocket->hasPendingDatagrams())
            {
                char pData [256];
                QHostAddress    peerAddr;
                quint16 peerPort;
                qint64 lenForCpy = eepromReadSocket->readDatagram(pData,sizeof(pData),&peerAddr,&peerPort);
                memcpy (((void*)(data.constData()+offset)),(const char*)(pData),lenForCpy);
                break;
            }
        }
        addr += 0x80;
        offset += 0x80;
        size -= 0x80;

    }
    return true;
}

#pragma pack (push,1)
struct writeeEepromStruct
{
    unsigned  addr:24;
    unsigned  pwd1:8;
    uint16_t  pwd2;
    uint8_t   data[128];
};
#pragma pack (pop)
bool Dialog::WriteEeprom(uint32_t addr, int size, const char *data)
{
    QString ip = ui->m_clientIp->text();
    QHostAddress hostAddr (ip);

    QUdpSocket socket (this);

    int offset = 0;
    while (size > 0)
    {
        int lenForCopy = 0x80;
        if (size < lenForCopy)
        {
            lenForCopy = size;
        }
        writeeEepromStruct params;
        params.addr = addr;
        params.pwd1 = 0x45;
        params.pwd2 = 0x5750;
        memcpy (params.data,data+offset,lenForCopy);

        socket.writeDatagram((const char*)(&params),lenForCopy+6,hostAddr,0xeee2);

        addr += 0x80;
        offset += 0x80;
        size -= 0x80;
        QThread::msleep(10);
    }
    return true;

}

bool Dialog::EraseEeprom(uint32_t addr, int size)
{
    QString ip = ui->m_clientIp->text();
    QHostAddress hostAddr (ip);

    QUdpSocket socket (this);

    int offset = 0;
    while (size > 0)
    {
        writeeEepromStruct params;
        params.addr = addr;
        params.pwd1 = 0x45;
        params.pwd2 = 0x4350;

        socket.writeDatagram((const char*)(&params),6,hostAddr,0xeeee);

        addr += 0x8000;
        offset += 0x8000;
        size -= 0x8000;
        QThread::msleep(100);
    }
    return true;

}

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    eepromReadSocket = new QUdpSocket (this);

    if (!eepromReadSocket->bind(QHostAddress::Any,0xeee1))
    {
        QMessageBox::critical(this,"Error","Cannot bind");
        return;
    }

//    connect(eepromReadSocket,SIGNAL(readyRead()),SLOT(onReadyRead()));

}

Dialog::~Dialog()
{
    delete ui;
}



void Dialog::on_m_btnReadEeprom_clicked()
{
    int addr = ui->m_eepromAddr->text().toULong(nullptr,0);
    int size = ui->m_eepromLen->text().toULong(nullptr,0);
    QByteArray ar;
    if (ReadEeprom(addr,size,ar))
    {
        ui->m_eepromDump->setData(ar);
    } else
    {
        QMessageBox::critical(this,"Error","Cannot Read Eeprom");
    }
}

void Dialog::onReadyReadEeprom()
{
    qDebug("Nihao!");
}

void Dialog::on_m_btnWriteEeprom_clicked()
{
    int addr = ui->m_eepromAddr->text().toULong(nullptr,0);
    int size = ui->m_eepromLen->text().toULong(nullptr,0);
    QByteArray ar = ui->m_eepromDump->data();
    const char* data = ar.constData();
    if (!WriteEeprom(addr,size,data))
    {
        QMessageBox::critical(this,"Error","Cannot Write Eeprom");
    }
}

void Dialog::on_m_btnEraseEeprom_clicked()
{
    int addr = ui->m_eepromAddr->text().toULong(nullptr,0);
    int size = ui->m_eepromLen->text().toULong(nullptr,0);
    if (!EraseEeprom(addr,size))
    {
        QMessageBox::critical(this,"Error","Cannot Write Eeprom");
    }

}

void Dialog::on_m_btnEditNetworkSettings_clicked()
{
    NetworkParamsEditor params (this);
    if (!ReadEeprom(0x1ff000,0x100,params.m_eepromImage))
    {
        QMessageBox::critical(this,"Error","Cannot read poarameters from EEPROM, check connection");
        return;
    }
    params.SetParameters();
    if (params.exec()==QDialog::Accepted)
    {
        ui->m_eepromAddr->setText("0x1ff000");
        ui->m_eepromLen->setText("0x100");
        ui->m_eepromDump->setData(params.m_eepromImage);
    }
}
