#include "qform1.h"
#include "ui_qform1.h"

qform1::qform1(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::qform1)
{
    ui->setupUi(this);


    QUdpSocket1 = new QUdpSocket(this);

    connect(QUdpSocket1,&QUdpSocket::readyRead,this,&qform1::onQudpSocket1Rx);

    QSerialPort1 = new QSerialPort(this);
    //QSerialPort1->setPortName("ttyACM0");
    QSerialPort1->setPortName("COM6");
    QSerialPort1->setBaudRate(9600);
    QSerialPort1->open(QSerialPort::ReadWrite);
    QSerialPort1->setDataTerminalReady(true);
    connect(QSerialPort1, &QSerialPort::readyRead, this, &qform1::OnQSerialPort1Rx);

}

qform1::~qform1()
{
    if(QUdpSocket1->isOpen())
        QUdpSocket1->abort();
    delete QUdpSocket1;

    delete ui;
}

void qform1::OnQSerialPort1Rx()
{
    int count;
    quint8 *buf;
    QString strhex;

    count=QSerialPort1->bytesAvailable();
    if(count<=0){
        return;
    }
    buf =new quint8[count];
    QSerialPort1->read((char *)buf,count);

    Decodificar(count,buf);
    //for(int i=0; i<count; i++)
    // strhex = strhex+QString("%1").arg(buf[i], 2, 16, QChar('0')).toUpper();
    //mensaje+=strhex;
    //ui->lineEdit_2->setText(mensaje);
    delete[] buf;

}



void qform1::onQudpSocket1Rx() {
    int count;
    uint8_t *buf;
    QString mensajexxx;
    while (QUdpSocket1->hasPendingDatagrams()) {
        count = QUdpSocket1->pendingDatagramSize();
        buf = new uint8_t[count];
        QUdpSocket1->readDatagram((char *)buf,count,&hostAdress, &hostPort);
        //ui->lineEdit_2->setText((char *)buf);
        Decodificar(count,buf);
        for(int i=0; i<count; i++)
         mensajexxx = mensajexxx+QString("%1").arg(buf[i], 2, 16, QChar('0')).toUpper();
        mensaje+=mensajexxx;
        ui->lineEdit_2->setText(mensajexxx);

        delete[] buf;
    }

    ui->textEdit->setText(hostAdress.toString());
    //QUdpSocket1 = new QUdpSocket(this);

    //connect(QUdpSocket1,&QUdpSocket::readyRead,this,&MainWindow::onQudpSocket1Rx);
}




void qform1::on_pushButton_clicked()
{
    quint16 port;
    bool ok;

    if(QUdpSocket1->isOpen()){
        QUdpSocket1->abort();
        ui->pushButton->setText("OPEN");

    }
    else{
        port = ui->lineEdit->text().toInt(&ok);
        if(!ok)
            return;

        if(!QUdpSocket1->bind(port)){
            QMessageBox::warning(this, "UDP PORT", "CANT BLID PORT");
        }
        QUdpSocket1->open(QUdpSocket::ReadWrite);
        ui->pushButton->setText("CLOSE");
    }

}


void qform1::EnviarComando(uint8_t length, uint8_t cmd, uint8_t payload[]){
    uint8_t cks;
    QString strhex;
    QSerialPort1->read((char *)TX, 6 + TX[4]);

    TX[0] = 'U';
    TX[1] = 'N';
    TX[2] = 'E';
    TX[3] = 'R';
    TX[4] = length;
    TX[5] = 0x00;
    TX[6] = ':';
    switch (cmd) {
        case 0xD0: //PWM BASE
            TX[7] = cmd;
            TX[8] = PWM1.u8[0];
            TX[9] = PWM1.u8[1];
            TX[10] = PWM1.u8[2];
            TX[11] = PWM1.u8[3];
            TX[12] = PWM2.u8[0];
            TX[13] = PWM2.u8[1];
            TX[14] = PWM2.u8[2];
            TX[15] = PWM2.u8[3];
        break;
        case 0xD2: //TIEMPO MOTOR PRENDIDO
            TX[7] = cmd;//CMD
            TX[8] = ui->lineEdit_4->text().toInt();
            ui->label->setNum(ui->lineEdit_4->text().toInt()*100);
        break;
        case 0xF0: //Alive
            TX[7] = cmd;
        break;
        case 0xA5:
            TX[7] = cmd;
            break;
        case 0xB0:
            TX[7] = cmd;
            break;
        case 0xB3:
            TX[7] = cmd;
            break;
        case 0xD5:
            TX[7] = cmd;
            break;
        case 0xC7: //PID PARAMETROS
            TX[7] = cmd;
            TX[8] = kp.u8[0];
            TX[9] = kp.u8[1];
            TX[10] = kp.u8[2];
            TX[11] = kp.u8[3];
            TX[12] = kd.u8[0];
            TX[13] = kd.u8[1];
            TX[14] = kd.u8[2];
            TX[15] = kd.u8[3];
            TX[16] = ki.u8[0];
            TX[17] = ki.u8[1];
            TX[18] = ki.u8[2];
            TX[19] = ki.u8[3];
        break;
    }


    cks=0;

    for(int i=0; i<TX[4]+6; i++){
        cks ^= TX[i];
    }
    TX[TX[4]+6] = cks;

    if(QSerialPort1->isOpen()){
        QSerialPort1->write((char*)TX, 7 + TX[4]);
    }

    if(QUdpSocket1->isOpen()){
        QUdpSocket1->writeDatagram((char*)TX,7 + TX[4],hostAdress,hostPort);
    }

}



void qform1::RecibirComando(uint8_t ind){
    static uint8_t asdasd = 0;
    QString ss;
    switch(RX[ind++]){
        case 0xF0:
            //ui->pushButton->setText("andaaa");
           // ui->lineEdit_2->setText("estoy Vivoo Funciona");
            asdasd++;
            ui->lineEdit_3->setText("Recibido ALIVE");
            ui->label->setNum(asdasd);
        break;
        case 0xD0:
            asdasd++;
            if(RX[ind++] == 0x0D){
                if(RX[ind++] == 0xDC){
                     ui->lineEdit_3->setText("MOTORES OK");
                }
            }
            ui->label->setNum(asdasd);
        break;
        case 0xD2:
        asdasd++;
        ui->lineEdit_3->setText("tiempo configurado");
        ui->label->setNum(asdasd);
        break;
        case 0xA1:

        for (uint8_t i = 0; i<9 ;i++) {
            for (uint8_t j = 0; j<4 ; j++) {
                bufADC[i].i8[j] = RX[ind++];
            }
        }
        //asdasd++;
        ui->lineEdit_3->setText("Recibido sensores");
        //ui->label->setNum(asdasd);
        ui->label_4->setNum(bufADC[0].i32);
        ui->label_5->setNum(bufADC[1].i32);
        ui->label_6->setNum(bufADC[2].i32);
        ui->label_7->setNum(bufADC[3].i32);
        ui->label_8->setNum(bufADC[4].i32);
        ui->label_9->setNum(bufADC[5].i32);
        ui->label_10->setNum(bufADC[6].i32);
        ui->label_11->setNum(bufADC[7].i32);
        ss = QString::number(bufADC[8].f,'f',3);
        ui->lineEdit_7->setText(ss);
      //  ui->label_12->setn
 //       ui->label_4->setNum();
        break;
//        case 0xD2:
//            ui->lineEdit_3->setText("Motores 0xD0");
//        break;
//        case 0xF1:
//            asdasd++;
//            ui->lineEdit_3->setText("0XF1");
//            ui->label->setNum(asdasd);
//        break;
    }
}


void qform1::Decodificar(int ind, quint8 *buf)
{
    static int caracter=0;
    static uint8_t nBytes=0, cks=0, indLectura=0;
    int indRecepcion=0;

    while (indRecepcion!=ind) {
        switch(caracter){
            case 0:
                if(buf[indRecepcion]=='U'){
                    caracter=1;
                }
            break;
            case 1:
                if(buf[indRecepcion]=='N')
                    caracter=2;
                else {
                    indRecepcion--;
                    caracter=0;
                }
           break;
           case 2:
               if(buf[indRecepcion]=='E')
                   caracter=3;
               else {
                   indRecepcion--;
                   caracter=0;
               }
          break;
          case 3:
            if(buf[indRecepcion]=='R')
                caracter=4;
            else {
                indRecepcion--;
                caracter=0;
            }
          break;
          case 4:
            nBytes=buf[indRecepcion];
            indiceRX=nBytes;
            caracter=5;
          break;
          case 5:
            if(buf[indRecepcion]==0x00)
                caracter=6;
            else {
                indRecepcion--;
                caracter=0;
            }
          break;
          case 6:
            if(buf[indRecepcion]==':'){
                cks= 'U'^'N'^'E'^'R'^nBytes^0x00^':';
                caracter=7;
            }
            else {
                indRecepcion--;
                caracter=0;
            }
          break;
          case 7:
            if(nBytes>1){
                cks^=buf[indRecepcion];
                RX[indLectura++]=buf[indRecepcion]; //guardo datos del buffer en RX para luego ver que llego
            }

            nBytes--;
            if(nBytes==0){
                caracter=0;
                if(cks==buf[indRecepcion]){
                    RecibirComando(indLectura+1-indiceRX);
                }
            }
          break;
        }
        indRecepcion++;
    }
}



void qform1::on_pushButton_2_clicked()
{
    EnviarComando(0x02,0xF0,payload);
}


void qform1::on_pushButton_3_clicked()
{
    PWM1.u32 = ui->lineEdit_6->text().toUInt();
    PWM2.u32 = ui->lineEdit_5->text().toUInt();
//TX[8] = ui->lineEdit_4->text().toInt();
    //EnviarComando(0x09,0xD0,payload);
    EnviarComando(10,0xD0,payload);
}


void qform1::on_pushButton_4_clicked()
{
    EnviarComando(0x03,0xD2,payload);
}


void qform1::on_pushButton_5_clicked()
{
     EnviarComando(0x02,0xA5,payload);
}


void qform1::on_pushButton_6_clicked()
{
    EnviarComando(0x02,0xB0,payload);
}


void qform1::on_pushButton_7_clicked()
{
    static uint8_t estadoo = 0;
    EnviarComando(2,0xD5,payload);
    // ui->pushButton->setText("OPEN");
    if(!estadoo){
        ui->pushButton_7->setText("STOP");
        estadoo = !estadoo;
    }
    else{
        ui->pushButton_7->setText("Start");
        estadoo = !estadoo;
    }

}


void qform1::on_pushButton_8_clicked()
{
    kp.i32 = ui->lineEdit_8->text().toInt();
    kd.i32 = ui->lineEdit_9->text().toInt();
    ki.i32 = ui->lineEdit_10->text().toInt();
    EnviarComando(14,0xC7,payload);
}


void qform1::on_pushButton_9_clicked()
{
    EnviarComando(2,0xB3,payload);
}

