#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QPair>
#include <QQueue>
#include <QVector>
#include <QTimer>

#include "Configurator/configurator.h"

class Logger : public QObject
{
    Q_OBJECT
public:
    static void loggingHandler(QtMsgType type, const char *msg);
    static Logger* getInstance();
    QVector<QString> giveLogs();
    // static Logger& getInstance(Configurator *config);

private:
    Logger();
    int linesWritten;
    static Logger* instance;
    int fileNo;
    QVector<QString> fileNames;
    QQueue<QPair<QtMsgType, QString> > queue;
    QFile *logFile;
    int askInterval;
    QTimer *timer;
    QString processMessage(QtMsgType type, QString msg);
public slots:
    void pushMessage(QtMsgType type, QString msg);
    void processQueue();

};

#endif // LOGGER_H
