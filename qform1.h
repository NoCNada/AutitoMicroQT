#ifndef QFORM1_H
#define QFORM1_H

#include <QMainWindow>
#include <QtNetwork/QUdpSocket>
#include <QtSerialPort/QSerialPort>
#include <QtNetwork/QHostAddress>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>

typedef union {
    uint8_t u8[4];
    int8_t i8[4];
    uint16_t u16[2];
    int16_t i16[2];
    int32_t i32;
    uint32_t u32;
    float f;
} _sWork;


QT_BEGIN_NAMESPACE
namespace Ui { class qform1; }
QT_END_NAMESPACE

class qform1 : public QMainWindow
{
    Q_OBJECT

public:
    qform1(QWidget *parent = nullptr);
    ~qform1();

private slots:

    void onQudpSocket1Rx();

    void on_pushButton_clicked();

    void OnQSerialPort1Rx();

    void on_pushButton_2_clicked();

    void Decodificar(int ind, quint8 *buf);

    void RecibirComando(uint8_t ind);

    void EnviarComando(uint8_t length, uint8_t cmd, uint8_t payload[]);

    void on_pushButton_3_clicked();


    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

private:
    Ui::qform1 *ui;

    QUdpSocket *QUdpSocket1;
    quint16 hostPort; //guarda puerto que transmite algo
    QHostAddress hostAdress; //guarda ip que transmite algo

    QSerialPort *QSerialPort1;

    uint8_t TX[256], payload[8],RX[256],indiceRX=0;
    QString mensaje="--> 0x";
    _sWork PWM1, PWM2, bufADC[9], kp,kd,ki;


};
#endif // QFORM1_H
