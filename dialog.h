#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QUdpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    QUdpSocket* eepromReadSocket;
    virtual bool ReadEeprom(uint32_t addr, int size, QByteArray &data);
    virtual bool WriteEeprom(uint32_t addr, int size, const char* data);
    virtual bool EraseEeprom(uint32_t addr, int size);
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_m_btnReadEeprom_clicked();
    void onReadyReadEeprom();
    void on_m_btnWriteEeprom_clicked();

    void on_m_btnEraseEeprom_clicked();

    void on_m_btnEditNetworkSettings_clicked();

    void on_m_btnBuildImage_clicked();

private:
    Ui::Dialog *ui;
};
#endif // DIALOG_H
