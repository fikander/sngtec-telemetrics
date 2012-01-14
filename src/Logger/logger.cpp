#include "logger.h"

#include <QString>
#include <QDateTime>
#include <QTextStream>
#include <QTimer>
#include <cstdlib>

Logger::Logger() {
    fileNames.push_back("logs_1");
    fileNames.push_back("logs_2");
    fileNo = 0;

    timer = new QTimer(this);
    askInterval = 2 * 1000;
    connect(timer, SIGNAL(timeout()), this, SLOT(processQueue()));

    logFile = new QFile(this->fileNames[this->fileNo]);

    if (!logFile->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return;


}

void Logger::loggingHandler(QtMsgType type, const char *msg) {
    // Put on queue
    QString message(msg);

    if (type == QtDebugMsg) {
        QTextStream errStream(stderr);
        errStream << message;
    }

    if (type == QtFatalMsg) {
        QTextStream errStream(stderr);
        errStream << message;
        abort();
    }

    Logger::getInstance()->pushMessage(type, message);
}

Logger* Logger::instance = NULL;


Logger* Logger::getInstance() {
    if (!instance)
        instance = new Logger;

    return instance;
}

void Logger::processQueue() {
    int MAX_LINES = 10;

    QTextStream errStream(stderr);
    errStream << "processing queue" << endl;

    if (!this->queue.empty()) {

        QTextStream fileStream(logFile);

        while (!this->queue.empty()) {   
            QPair<QtMsgType, QString> top = this->queue.dequeue();
            QString message = this->processMessage(top.first, top.second);

            fileStream << message;
            this->linesWritten += 1;
        }
    }

    if (this->linesWritten > MAX_LINES) {
        this->linesWritten = 0;
        // Swap files

        this->fileNo = (this->fileNo + 1) % this->fileNames.size();
        this->logFile->close();
        this->logFile = new QFile(this->fileNames[this->fileNo]);

        if (!logFile->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
            return;

    }
}

QString Logger::processMessage(QtMsgType type, QString msg) {
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

    return message;
}

void Logger::pushMessage(QtMsgType type, QString msg) {
timer->start(askInterval);
    this->queue.append(QPair<QtMsgType, QString>(type, msg));
}
