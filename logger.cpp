#include "logger.h"
#include "vectornavprotocol.h"
#include <QObject>
#include <QDebug>
#include <QFile>
#include <QDateTime>

Logger::Logger(QObject *parent)
    : QObject{parent}{
}


void Logger::logStart() {
    QString fileName = "dataVectorNAV"+QString(" ")+QDate::currentDate().toString("yy-MM-dd")+QString(" ")+QTime::currentTime().toString("hh-mm-ss")+".txt";
    qDebug()<<fileName;
    file.setFileName(fileName);

    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug()<<"file is opened";
    }
    else {
        //qDebug()<<"file open error : "<<QDate::currentDate().toString()+QTime::currentTime().toString()+".txt";
        qDebug()<<file.errorString() <<" " << file.error();
    }
    QTextStream stream2(&file);
    stream2 << "TimeStartup,  msg.yaw, msg.pitch, msg.roll, msg.X_rate, msg.Y_rate,  msg.Z_rate,  msg.X_accel,  msg.Y_accel,  msg.Z_accel \n";
}

void Logger::logTick(DataFromVectorNav msg) {
    QTextStream stream2(&file);
    stream2 << msg.TimeStartup <<" , " <<  msg.yaw <<" , "<< msg.pitch<< " , " << msg.roll <<" , "<< msg.X_rate <<" , "<< msg.Y_rate << " , "<< msg.Z_rate << " , " << msg.X_accel << " , "<< msg.Y_accel << " , " << msg.Z_accel<<"\n";
}

void Logger::logStop() {
    file.close();
}
