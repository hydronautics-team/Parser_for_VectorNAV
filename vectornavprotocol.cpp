#include "vectornavprotocol.h"
#include <QDataStream>
#include <QFile>
#include <QTime>


VectorNavProtocol::VectorNavProtocol(QString portName, int baudRate, QObject *parent)
{
    m_port.setBaudRate(baudRate);
    m_port.setPortName(portName);
    m_port.open(QIODevice::ReadWrite);

    if (m_port.open(QIODevice::ReadWrite) == false) {
        m_port.setBaudRate(baudRate);
        m_port.setPortName(portName);
        m_port.open(QIODevice::ReadWrite);
    }
    char cmd[20] = "$VNWRG,06,0*XX\r\n";//отключаем передачу данных ACSII
    m_port.write(cmd, 19);
    m_port.waitForBytesWritten();

    char cmd2[31] = "$VNWRG,75,2,4,01,0129*XX\r\n";//передаем какие данные будем принимать
    m_port.write(cmd2, 30);
    m_port.waitForBytesWritten();

    QTimer *timer = new QTimer(this);
    connect(&m_port, &QSerialPort::readyRead, this, &VectorNavProtocol::readData);
    connect(&m_port, &QSerialPort::readyRead, this, &VectorNavProtocol::readyReadForTimer);
    connect(timer, &QTimer::timeout, this, &VectorNavProtocol::timeoutSlot);
    timer->start(3000);

}

unsigned short VectorNavProtocol::calculateCRC(unsigned char data[], unsigned int length) {
     unsigned int i;
     unsigned short crc = 0;
     for(i=0; i<length; i++){
         char temp = data[i];
         crc = (unsigned char)(crc >> 8) | (crc << 8);
         crc ^= data[i];
         crc ^= (unsigned char)(crc & 0xff) >> 4;
         crc ^= crc << 12;
         crc ^= (crc & 0x00ff) << 5;
     }
    return crc;
}


bool VectorNavProtocol::correctChecksum (QByteArray const &ba) {
    if (calculateCRC((unsigned char*)ba.data(), ba.size()) == 0) {
        qDebug() << "true ";
        return true;
    }
    qDebug() << "false ";
    return false;
}

void VectorNavProtocol::readData() {
    m_buffer.append(m_port.readAll());
    parseBuffer();
}

void VectorNavProtocol::readyReadForTimer() {
    time.restart();

}

void VectorNavProtocol::timeoutSlot(){
 double deltaTMax = 3000;
    if (time.elapsed()>deltaTMax) {
        m_port.setBaudRate(baudRate);
        m_port.setPortName("COM7");
        m_port.open(QIODevice::ReadWrite);

        connect(&m_port, &QSerialPort::readyRead, this, &VectorNavProtocol::readData);
    }
}

void VectorNavProtocol::parseBuffer() {
    static int count;
    if ( m_buffer.size() <= 4 ) {
        return;
    }
    QByteArray header((char*) &(data.header),sizeof(Header));
    int index = m_buffer.indexOf(header);
    if (index == -1) {
        // Не найдено сообщение
        qDebug() << "нет сообщения в буфере ";
        return;
    }
    if ( m_buffer.size() <= index + 49 ) {
        return;
    }
    if (correctChecksum(m_buffer.mid(index+1, 49))) {
        //qDebug()<<++count;
        DataFromVectorNav msg;
        auto tmp = m_buffer.mid(index, 49);
        QDataStream stream(&tmp, QIODevice::ReadOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
        stream >> msg.header.sync;
        stream >> msg.header.group;
        stream >> msg.header.group_1_fields;
        stream >> msg.TimeStartup;
        //qDebug() << "TimeStartup: " <<msg.TimeStartup;
        stream >> msg.yaw;
        //qDebug() << "yaw: " <<msg.yaw;
        stream >>msg.pitch;
        //qDebug() << "pitch: " <<msg.pitch;
        stream >> msg.roll;
        //qDebug() << "roll: " <<msg.roll;
        stream >> msg.X_rate;
        //qDebug() << "X_rate: " <<msg.X_rate;
        stream >>msg.Y_rate;
        //qDebug() << "Y_rate: " <<msg.Y_rate;
        stream >> msg.Z_rate;
        //qDebug() << "Z_rate: " <<msg.Z_rate;
        stream >> msg.X_accel;
        //qDebug() << "X_accel: " <<msg.X_accel;
        stream >>msg.Y_accel;
        //qDebug() << "Y_accel: " <<msg.Y_accel;
        stream >> msg.Z_accel;
        //qDebug() << "Z_accel: " <<msg.Z_accel;
        stream >> msg.temp[1];
        stream >> msg.temp[0];
        emit newMessageDetected(msg);
        m_buffer.remove(0, index+49);
    }
    else {
        m_buffer.remove(0, index+1);
    }

    return;
}

