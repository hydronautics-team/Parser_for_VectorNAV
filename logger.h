#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>

class logger : public QObject
{
    Q_OBJECT
public:
    explicit logger(QObject *parent = nullptr);

signals:

private slots:
    void logStart();
    void logTick(QFile file);
    void logStop(QFile file);
};

#endif // LOGGER_H


