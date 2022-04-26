#include "networkparamseditor.h"
#include "ui_networkparamseditor.h"

NetworkParamsEditor::NetworkParamsEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkParamsEditor)
{
    ui->setupUi(this);
}

NetworkParamsEditor::~NetworkParamsEditor()
{
    delete ui;
}

/*************************************************************************************
 ************************************************************************************
 * Base Address: 0x1ff000
 *
 * +0x000  (0x06 bytes) - MAC Address
 * +0x006  (0x04 bytes) - IP Address
 * +0x00A (0x04 bytes) - Subnet Mask
 * +0x00E (0x04 bytes) - Default Gateway
 *
 * +0x040 (0x40 bytes) - DHCP Suffix, included Network Name.
 *                            First byte is an actual length
 * +0x080 (0x60 bytes) - mDNS Ping Packet, included Resolved Device Name
 *
 ************************************************************************************
 ***********************************************************************************/

void NetworkParamsEditor::SetParameters()
{
    uint8_t* pData = (uint8_t*)m_eepromImage.constData();
    QString macAddress = QString ("%1:%2:%3:%4:%5:%6").arg(pData[0],2,16,QChar('0')).arg(pData[1],2,16,QChar('0')).arg
            (pData[2],2,16,QChar('0')).arg(pData[3],2,16,QChar('0')).arg(pData[4],2,16,QChar('0')).arg(pData[5],2,16,QChar('0'));
    ui->m_macAddress->setInputMask("HH:HH:HH:HH:HH:HH;_");
    ui->m_macAddress->setText(macAddress);

    QString ipAddr = QString ("%1.%2.%3.%4").arg(pData[6],3,10,QChar('0')).arg(pData[7],3,10,QChar('0')).arg(pData[8],3,10,QChar('0')).arg(pData[9],3,10,QChar('0'));
    ui->m_ipAddr->setInputMask("000.000.000.000;_");
    ui->m_ipAddr->setText(ipAddr);

    QString subNetMask = QString ("%1.%2.%3.%4").arg(pData[10],3,10,QChar('0')).arg(pData[11],3,10,QChar('0')).arg(pData[12],3,10,QChar('0')).arg(pData[13],3,10,QChar('0'));
    ui->m_subNetMAsk->setInputMask("000.000.000.000;_");
    ui->m_subNetMAsk->setText(subNetMask);

    QString gateway = QString ("%1.%2.%3.%4").arg(pData[14],3,10,QChar('0')).arg(pData[15],3,10,QChar('0')).arg(pData[16],3,10,QChar('0')).arg(pData[17],3,10,QChar('0'));
    ui->m_defaultGateway->setInputMask("000.000.000.000;_");
     ui->m_defaultGateway->setText(gateway);

     // Fill DHCP Name only if it is really present.
     if ((pData [0x40]<=0x40) &&  (pData [0x41]==0x0c) && (pData [0x42]<=0x40-2))
     {
        char txt [0x40];
        memset(txt,0,sizeof(txt));
        memcpy (txt,pData+0x43,pData [0x42]);
        ui->m_DHCPname->setText(txt);
     }
     // Fill mDNS Name only if it is really present.
     static const uint8_t mdnsBase[] = {0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00};
     static const int endOfData = 0x80 + 0x60;
     if (memcmp(pData+0x80,mdnsBase,sizeof(mdnsBase))==0)
     {
         char mDnsName [0x100];
         memset (mDnsName,0,sizeof(mDnsName));
         int posInmDnsName = 0;
         int ptr = 0x8b;
         bool bAllIsGood = true;
          while (true)
          {
             int curLen = pData [ptr];
             if (ptr + curLen > endOfData)
             {
                 bAllIsGood = false;
                 break;
             }
             ptr += 1;
             while (curLen-- > 0)
             {
                 if ((pData[ptr] < ' ')||(pData[ptr] == 0xff))
                 {
                     bAllIsGood = false;
                     break;
                 } else
                 {
                     mDnsName [posInmDnsName++] = pData[ptr++];
                 }
             }
             if (pData[ptr] == 0)
             {
                 break;
             } else
             {
                 mDnsName [posInmDnsName++] ='.';
             }
          }
          ui->m_mDNSName->setText(mDnsName);
     }


}

void NetworkParamsEditor::on_buttonBox_accepted()
{
    uint8_t* pData = (uint8_t*)m_eepromImage.constData();
    char txt [128];
    uint32_t temp[6];

    strcpy (txt,ui->m_macAddress->text().toLatin1());
    sscanf(txt,"%X:%X:%X:%X:%X:%X",&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
    pData[0] = (uint8_t) temp[0];
    pData[1] = (uint8_t) temp[1];
    pData[2] = (uint8_t) temp[2];
    pData[3] = (uint8_t) temp[3];
    pData[4] = (uint8_t) temp[4];
    pData[5] = (uint8_t) temp[5];

    strcpy (txt,ui->m_ipAddr->text().toLatin1());
    sscanf(txt,"%d.%d.%d.%d",&temp[0],&temp[1],&temp[2],&temp[3]);
    pData[6] = (uint8_t) temp[0];
    pData[7] = (uint8_t) temp[1];
    pData[8] = (uint8_t) temp[2];
    pData[9] = (uint8_t) temp[3];

    strcpy (txt,ui->m_subNetMAsk->text().toLatin1());
    sscanf(txt,"%d.%d.%d.%d",&temp[0],&temp[1],&temp[2],&temp[3]);
    pData[10] = (uint8_t) temp[0];
    pData[11] = (uint8_t) temp[1];
    pData[12] = (uint8_t) temp[2];
    pData[13] = (uint8_t) temp[3];

    strcpy (txt,ui->m_defaultGateway->text().toLatin1());
    sscanf(txt,"%d.%d.%d.%d",&temp[0],&temp[1],&temp[2],&temp[3]);
    pData[14] = (uint8_t) temp[0];
    pData[15] = (uint8_t) temp[1];
    pData[16] = (uint8_t) temp[2];
    pData[17] = (uint8_t) temp[3];

    strcpy (txt,ui->m_DHCPname->text().toLatin1());
    int lineSize = strlen (txt);
    int totalSize = lineSize + 6;
    memset (pData+0x40,0xff,0x60);
    pData [0x40] = (uint8_t) totalSize;
    pData [0x41] = 0x0c;
    pData [0x42] = (uint8_t)lineSize;
    strcpy ((char*)pData + 0x43,txt);
    pData [0x43 + lineSize + 0] = 0xff;
    pData [0x43 + lineSize + 1] = 0x00;
    pData [0x43 + lineSize + 2] = 0x00;
    pData [0x43 + lineSize + 3] = 0x00;

    static const uint8_t mDNSBasis [] =
         {/*0x00,*/0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00};

    memcpy (pData + 0x80,mDNSBasis,sizeof(mDNSBasis));
    int dest = 0x80 + sizeof (mDNSBasis);

    for(auto& iter : ui->m_mDNSName->text().split('.'))
    {
        pData [dest++] = iter.length();
        strcpy (txt,iter.toLatin1());
        for (int i = 0;i<iter.length();i++)
        {
            pData [dest++] = txt[i];
        }
    }
    pData [dest++] = 0x00;
    pData [dest++] = 0x00;
    pData [dest++] = 0x01;
    pData [dest++] = 0x00;
    pData [dest++] = 0x01;

/*    static const uint8_t mDNSanswerBasis1[] =
    {
        0x00,0x00,0x84,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00
    }

    static const uint8_t mDNSanswerBasis2[] =
    {
        0x00,0x00,0x01,0x80,0x01,0x00,0x00,0x00,0x78,0x00,0x04
    }
*/
}
