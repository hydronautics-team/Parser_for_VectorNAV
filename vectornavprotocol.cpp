#include "vectornavprotocol.h"
#include <QDataStream>


VectorNavProtocol::VectorNavProtocol(QString portName, int baudRate, QObject *parent)
{
    m_port.setBaudRate(baudRate);
    m_port.setPortName(portName);
    m_port.open(QIODevice::ReadWrite);

    char cmd[20] = "$VNWRG,06,0*XX\r\n";//отключаем передачу данных ACSII
    m_port.write(cmd, 19);
    m_port.waitForBytesWritten();

    char cmd2[31] = "$VNWRG,75,2,10,01,0008*XX\r\n";//передаем какие данные будем принимать
    m_port.write(cmd2, 30);
    m_port.waitForBytesWritten();

    connect(&m_port, &QSerialPort::readyRead, this, &VectorNavProtocol::readData);
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
    if ( m_buffer.size() <= index + 17 ) {
        return;
    }
    if (correctChecksum(m_buffer.mid(index+1, 17))) {}
        qDebug()<<++count;

        /*DataFromVectorNav* msg = reinterpret_cast<DataFromVectorNav*>(m_buffer.data()+index);
        qDebug() << "yaw: " << msg->yaw;
        qDebug() << "pitch: " << msg->pitch;
        qDebug() << "roll: " << msg->roll; */

        DataFromVectorNav msg;
        auto tmp = m_buffer.mid(index, 18);
        QDataStream stream(&tmp, QIODevice::ReadOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
        stream >> msg.header.sync;
        stream >> msg.header.group;
        stream >> msg.header.group_1_fields;
        stream >> msg.yaw;
        qDebug() << "yaw: " <<msg.yaw;
        stream >>msg.pitch;
        qDebug() << "pitch: " <<msg.pitch;
        stream >> msg.roll;
        qDebug() << "roll: " <<msg.roll;
        stream >> msg.temp[1];
        stream >> msg.temp[0];

//        DataFromVectorNav *msg = new DataFromVectorNav();
//        memcpy(msg,m_buffer.data()+index,sizeof(DataFromVectorNav));
//        qDebug() << "yaw: " << msg->yaw;
//        qDebug() << "pitch: " << msg->pitch;
//        qDebug() << "roll: " << msg->roll;
//        qDebug() << "crc " << msg->crc;
        m_buffer.remove(0, index+17);
    return;

//  emit newMessageDetected(*msg);
}

