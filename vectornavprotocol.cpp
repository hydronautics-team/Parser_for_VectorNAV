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

    char cmd2[31] = "$VNWRG,75,2,4,01,0129*XX\r\n";//передаем какие данные будем принимать
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


//bool VectorNavProtocol::correctChecksum (QByteArray const &ba) {
//    if (calculateCRC((uchar*)ba.data(), 7) == ba.mid(16, 2)) {
//        qDebug() << "true ";
//        return true;
//    }
//    //   qDebug() << m_buffer;
//    qDebug() << ba.data()[8];
//    qDebug() << "false ";
//    return false;
//}
//bool VectorNavProtocol::correctChecksum (QByteArray const &ba) {
//    if (calculateCRC((uchar*)ba.data(), 8) == ba.mid(16, 2)) {
//        return true;
//    }
//    return false;
//}

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
    if ( m_buffer.size() <= index + 49 ) {
        return;
    }
    if (correctChecksum(m_buffer.mid(index+1, 49))) {
        qDebug()<<++count;

        DataFromVectorNav msg;
                auto tmp = m_buffer.mid(index, 49);
                QDataStream stream(&tmp, QIODevice::ReadOnly);
                stream.setByteOrder(QDataStream::LittleEndian);
                stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
                stream >> msg.header.sync;
                stream >> msg.header.group;
                stream >> msg.header.group_1_fields;
                stream >> msg.TimeStartup;
                qDebug() << "TimeStartup: " <<msg.TimeStartup;
                stream >> msg.yaw;
                qDebug() << "yaw: " <<msg.yaw;
                stream >>msg.pitch;
                qDebug() << "pitch: " <<msg.pitch;
                stream >> msg.roll;
                qDebug() << "roll: " <<msg.roll;
                stream >> msg.X_rate;
                qDebug() << "X_rate: " <<msg.X_rate;
                stream >>msg.Y_rate;
                qDebug() << "Y_rate: " <<msg.Y_rate;
                stream >> msg.Z_rate;
                qDebug() << "Z_rate: " <<msg.Z_rate;
                stream >> msg.X_accel;
                qDebug() << "X_accel: " <<msg.X_accel;
                stream >>msg.Y_accel;
                qDebug() << "Y_accel: " <<msg.Y_accel;
                stream >> msg.Z_accel;
                qDebug() << "Z_accel: " <<msg.Z_accel;
                stream >> msg.temp[1];
                stream >> msg.temp[0];
}





//        DataFromVectorNav* msg = reinterpret_cast<DataFromVectorNav*>(m_buffer.data()+index);

//        qDebug() << "X_accel: " << msg->yaw;
//        qDebug() << "Y_accel: " << msg->Y_accel;
//        qDebug() << "Z_accel: " << msg->Z_accel;
//        qDebug() << "yaw: " << msg->yaw;
//        qDebug() << "pitch: " << msg->pitch;
//        qDebug() << "roll: " << msg->roll;


        /*DataFromVectorNav msg;
        auto tmp = m_buffer.mid(index, 18);
        QDataStream stream(&tmp, QIODevice::ReadOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream.setFloatingPointPrecision(QDataStream::DoublePrecision);
        stream >> msg.header.sync;
        stream >> msg.header.group;
        stream >> msg.header.group_1_fields;
        stream >> msg.yaw;
        stream >>msg.pitch;
        qDebug() << "pitch: " <<msg.pitch;
        stream >> msg.roll;
        stream >> msg.temp[1];
        stream >> msg.temp[0];  */

//        DataFromVectorNav *msg = new DataFromVectorNav();
//        memcpy(msg,m_buffer.data()+index,sizeof(DataFromVectorNav));
//        qDebug() << "yaw: " << msg->yaw;
//        qDebug() << "pitch: " << msg->pitch;
//        qDebug() << "roll: " << msg->roll;
//        qDebug() << "crc " << msg->crc;
        m_buffer.remove(0, index+49);
    return;



//  emit newMessageDetected(*msg);

    //разбор m_buffer;
    //1. Если размер буфера меньше длины сообщения, то ничего не делаем,
    //выходим из метода по return и ждём, когда придёт достаточное количество байт

    //2. Если байт в буфере достаточное количество начинаем разбор
    // в QByteArray есть метод indexOf (QByteArray ba, int from=0) - который возвращает индекс,
    //где он первый раз встретил последовательность байт ba
    //QByteArray header((char*) &(data.header),sizeof(Header));// сделала QByteArray из структуры
    //int index = m_buffer.indexOf(header);//так можете получить индекс на первое нахождение заголовка в данных
    //возвращает -1, если ничего не нашёл

    //если найдено новое сообщение, то emit newMessageDetected()
}

