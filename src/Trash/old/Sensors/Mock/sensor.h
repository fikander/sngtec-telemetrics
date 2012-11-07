#ifndef CZUJNIK_H
#define CZUJNIK_H

#include <QObject>
#include <QString>
#include "protocol.h"
#include "devproxy.h"

class Sensor : public QIODevice {
    Q_OBJECT

 private:
    Protocol* myProtocol;
    DevProxy* myDevice;

public:
    Sensor();
    int connectDevice(DevProxy* device);
    // Qstrings?
    int sendMesg(QString mesg);
    QString readMesg();
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
};

#endif // CZUJNIK_H
