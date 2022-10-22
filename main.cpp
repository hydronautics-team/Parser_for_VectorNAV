#include <QCoreApplication>
#include <QDebug>
#include <QSerialPortInfo>
#include <QSerialPort>
#include "vectornavprotocol.h"
#include "logger.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    VectorNavProtocol protocol ("COM5");
    Logger logger;
    logger.logStart();
    QObject::connect(&protocol, &VectorNavProtocol::newMessageDetected,
                     &logger, &Logger::logTick);
    return a.exec();
}
