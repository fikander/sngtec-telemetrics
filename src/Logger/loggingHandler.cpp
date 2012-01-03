#include <QtGlobal>
#include <QIODevice>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <cstdlib>

void loggingHandler(QtMsgType type, const char *msg) {
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

    QFile logs("logs");
    if (!logs.open(QIODevice::Append | QIODevice::Text))
        return;
    QTextStream fileStream(&logs);
    QTextStream errStream(stderr);
    fileStream << message;
    errStream << message;

    if (type == QtFatalMsg) {
        abort();
    }
}
