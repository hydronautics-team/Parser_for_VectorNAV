#include "logger.h"
#include "vectornavprotocol.h"
#include <QObject>
#include <QDebug>
#include <QFile>

logger::logger(QObject *parent)
    : QObject{parent}
{
    connect ();

}


void logger::logStart() {
    QFile file("dataVectorNAV.txt");
    file.open(QIODevice::WriteOnly);
}

void logger::logTick(QFile file) {
    QTextStream stream(&file);
    stream << "TimeStartup: " << msg.TimeStartup << "yaw: " << msg.yaw << "pitch: " << msg.pitch << "roll: " << msg.roll << "X_rate: " << msg.X_rate << "Y_rate: " << msg.Y_rate << "Z_rate: " << msg.Z_rate << "X_accel: " << msg.X_accel << "Y_accel: "<< msg.Y_accel << "Z_accel: " << msg.Z_accel<<"\n";
}

void logger::logStop(QFile file) {
file.close();

}


