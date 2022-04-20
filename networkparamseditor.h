#ifndef NETWORKPARAMSEDITOR_H
#define NETWORKPARAMSEDITOR_H

#include <QDialog>

namespace Ui {
class NetworkParamsEditor;
}

class NetworkParamsEditor : public QDialog
{
    Q_OBJECT

public:
    explicit NetworkParamsEditor(QWidget *parent = nullptr);
    ~NetworkParamsEditor();

    void SetParameters();

    QByteArray m_eepromImage;

private:
    Ui::NetworkParamsEditor *ui;

    // QDialog interface
public slots:
private slots:
    void on_buttonBox_accepted();
};

#endif // NETWORKPARAMSEDITOR_H
