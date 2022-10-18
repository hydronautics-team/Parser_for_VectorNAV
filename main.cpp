#include <QCoreApplication>
#include <QDebug>
#include <QSerialPortInfo>
#include <QSerialPort>
#include "vectornavprotocol.h"
;
#pragma pack(push,1)
struct Test {
    char header =0xFA;
    float yaw;
    char crc;
};
#pragma pack(pop)

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    VectorNavProtocol protocol ("COM7");

    return a.exec();
}
