#ifndef MODBUS_H
#define MODBUS_H

#include "Message/message.h"
#include "Sensors/devconnection.h"
#include "modbusrtuframe.h"

#include <QObject>
#include <QMap>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string>
#include <QSocketNotifier>
#include <set>



class Modbus : public DevConnection {
    Q_OBJECT

  public:
        Modbus* create(Configurator *config, int no);
        Modbus* clone(Configurator *config, int no);
        ~Modbus();
        Modbus();
        Modbus(Configurator* config, int no);

        void write(QVector<MessageSample> messages);
        QVector<MessageSample> readAll();

  signals: // Device should connect this
        void readyToRead();

  public slots:
        void readFromSensor();

  private:
        ModbusRtuFrame* decodeMessage(MessageSample msg);
        int checkResponseCRC(unsigned char* answer, unsigned char* answer_data,
                             int answer_size, int take_byte_count, unsigned short crc);
        int preparePort(char* port, QString bandwidth, QString parity);

        //Decodes pseudo-hexes from config
        MessageSample decodeByConfig(MessageSample msg);
        QString hexTranslate(QString toTranslate);

        //Protects from read problems & makes timeout
        int tryRead(unsigned char* buffer, int size);

        int fd;
        int number;
        QSocketNotifier* portNotifier;
        Configurator * config;
        QVector<MessageSample> msgQue;
        QMap<QString, QString> message_map;
};

#endif // MODBUS_H
