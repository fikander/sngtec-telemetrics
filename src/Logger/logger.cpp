
#include <QString>
#include <QDateTime>
#include <QTextStream>
#include <QTimer>
#include <cstdlib>


#include "logger.h"
#include "debug.h"

Logger* Logger::instance = NULL;

Logger* Logger::initialise(KeyValueMap &config)
{
    if (!instance)
        instance = new Logger(config);
    return instance;
}

Logger* Logger::getInstance() {
    return instance;
}


Logger::Logger(KeyValueMap &config) {

    fileNames.push_back("logs_1.log");
    fileNames.push_back("logs_2.log");
    fileNo = 0;

    // timer

    timer = new QTimer(this);

    if (config.contains("log_flush_interval"))
        askInterval = config["log_flush_interval"].toInt() * 1000;
    else
        askInterval = 60*60*1000; // flush every hour

    if (config.contains("max_messages_in_file"))
        maxMessagesInFile = config["max_messages_in_file"].toInt();
    else
        maxMessagesInFile = 1000;

    connect(timer, SIGNAL(timeout()), this, SLOT(flushMessages()));
    timer->start(askInterval);

    // logging level

    loggingLevel = QtWarningMsg;
    if (config.contains("level"))
    {
        QString level = config["level"].toString();
        if (level == "debug")
            loggingLevel = QtDebugMsg;
        else if (level == "warning")
            loggingLevel = QtWarningMsg;
        else if (level == "error")
            loggingLevel = QtCriticalMsg;
    }

    queueDirty = false;

    logFile = new QFile(this->fileNames[this->fileNo]);

    if (!logFile->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return;
}


QVector<QString> Logger::giveLogs() {
    QVector<QString> res;
    int oldFileNo = (this->fileNo + 1) % this->fileNames.size();
    int newFileNo = this->fileNo;
    QFile oldFile(this->fileNames[oldFileNo]);
    QFile newFile(this->fileNames[newFileNo]);

    // Read old?
    oldFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream oldStream(&oldFile);

    while (!oldStream.atEnd()) {
        res.push_back(oldStream.readLine());
    }

    oldFile.close();
    // Read new
    newFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream newStream(&newFile);

    while (!newStream.atEnd()) {
        res.push_back(newStream.readLine());
    }

    newFile.close();

    // Read queue
    QListIterator<QString> it(queue);
    while(it.hasNext()) {
        res.push_back(it.next());
    }

    return res;
}


void Logger::pushMessage(QtMsgType type, QString msg) {
    if (type >= loggingLevel)
    {
        queue.append(msg);
        queueDirty = true;
    }
}


void Logger::flushMessages()
{
    if (!queueDirty)
        return;

    QDEBUG << "Flushing messages to " << logFile->fileName();

    if (!queue.isEmpty()) {
        QTextStream fileStream(logFile);

        while (!queue.isEmpty()) {
            fileStream << queue.dequeue();
            linesWritten += 1;
        }
    }

    if (linesWritten > maxMessagesInFile) {
        linesWritten = 0;
        // Swap files

        fileNo = (fileNo + 1) % fileNames.size();
        logFile->close();
        logFile = new QFile(fileNames[fileNo]);

        if (!logFile->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
            return;
    }
}

void Logger::loggingHandler(QtMsgType type, const char *msg)
{
    QString message = QDateTime::currentDateTime().toString();

    switch (type) {
    case QtDebugMsg:
        message.append(" Debug: ");
        break;
    case QtWarningMsg:
        message.append(" Warning: ");
        break;
    case QtCriticalMsg:
        message.append(" Critical: ");
        break;
    case QtFatalMsg:
        message.append(" Fatal: ");
        abort();
    }
    message.append(msg);
    message.append("\n");

    QTextStream errStream(stderr);
    errStream << message;

    Logger::getInstance()->pushMessage(type, message);
}

