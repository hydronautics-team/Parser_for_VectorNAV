#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include "vectornavprotocol.h"

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject *parent = nullptr);

signals:
public slots:
    void logStart();
    void logTick(DataFromVectorNav data);
    void logStop();
protected:
    QTextStream stream;
    QFile file;

};

#endif // LOGGER_H


