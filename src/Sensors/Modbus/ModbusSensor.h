#ifndef MODBUS_H
#define MODBUS_H

#include <QObject>
#include <QSocketNotifier>
#include <QTimer>
#include <QVector>
#include <QHash>

#include "Sensors/Sensor.h"
#include "KeyValueMap.h"

#include "modbus.h"
#include "converter.h"

class Modbus : public Sensor
{

    Q_OBJECT

public:
    Modbus(KeyValueMap &config, QObject *parent = 0);
    virtual ~Modbus();

    virtual int connect();

public slots:
    // send to the device
    virtual void send(QSharedPointer<Message> payload);

signals:
    // emitted for every sample/event received from the device
    void received(QSharedPointer<Message> payload);

private slots:
      void sendAndReceiveData();

private:
    void modbusDisconnect();
    QVector<quint16> modbusReadData(int functionCode, int startAddress, int noOfItems);
    bool modbusWriteData(int functionCode, int startAddress, QVector<quint16> vector);

    QString portName;
    char parity;
    int bandwidth;
    int dataBits, stopBits;
    int timeout;
    bool modbusDebug;
    bool bigEndian;
    int byte_timeout_ms;
    QTimer timer;

    modbus_t *m_modbus;


    class Query
    {
    public:
        Query();
        ~Query();
        Query(QString name, int address, int count, bool bigEndian = true);
        QString name;
        int slave;
        int address;
        int count;
        bool bigEndian;
        QString eventType;
        char read_function, write_function;

        bool queried;
        QVector<quint16> lastResult;
        Converter *converter;
        static QHash<int, Converter*> *_static_converters;

        QString toString()
        {
            return name + "[" + eventType +
                    "],slave:" + QString::number(slave) +
                    ",addr:" + QString::number(address) +
                    ",cnt:" + QString::number(count) +
                    ",read:0x" + QString::number(read_function, 16) +
                    ",write:0x" + QString::number(write_function, 16) +
                    "," + ((0 == converter) ? "missing_converter" : converter->description());
        }

        QVector<quint16> revConvert(const QString &sample) {
            if (0 != converter) {
                return converter->revConvert(sample, bigEndian, count);
            }
            else {
                qDebug() << "WARNING : converter not set (revConvert)!";
                return QVector<quint16>();
            }
        }

        QString convert(const QVector<quint16> &vector) {
            if (0 != converter) {
                return converter->convertToString(vector, bigEndian);
            }
            else {
                qDebug() << "WARNING : converter not set (convert)!";
                return "nan";
            }
        }

        static QHash<int, Converter*> *converters();

    private:
        static QHash<int, Converter*> *_converters;

    };
    QVector<Query> queries;
};

#endif // MODBUS_H
