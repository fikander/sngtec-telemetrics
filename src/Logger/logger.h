#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QPair>
#include <QQueue>
#include <QVector>
#include <QTimer>
#include <QFile>

#include "KeyValueMap.h"

class Logger : public QObject
{
    Q_OBJECT
public:
    static void loggingHandler(QtMsgType type, const char *msg);

    static Logger* initialise(KeyValueMap &config);
    static Logger* getInstance(); // has to be initialised first

    QVector<QString> giveLogs();

protected:
    Logger(KeyValueMap &config);

private:
    int linesWritten;
    static Logger* instance;
    int fileNo;
    QVector<QString> fileNames;
    QQueue<QString> queue;
    QFile *logFile;
    int askInterval;
    QTimer *timer;

    bool queueDirty; // flushing queue to disk clears this flag
    QtMsgType loggingLevel; // minimum logging level to put to log files

public slots:
    void pushMessage(QtMsgType type, QString msg);
    void processQueue();
};

#endif // LOGGER_H
