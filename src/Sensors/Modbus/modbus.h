#ifndef MODBUS_H
#define MODBUS_H

#include "src/Message/message.h"
#include "src/Sensors/devconnection.h"
#include "modbusrtuframe.h"

#include <QObject>
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
        ~Modbus(); // was virtual

        Modbus(Configurator* config);

        void write(QVector<Message> messages);
        QVector<Message> readAll();

        int connect(unsigned char addr);

  signals: // Device should connect this
        void readyToRead();

  public slots:
        void readFromSensor();

  private:
        ModbusRtuFrame* decodeMessage(Message msg);
        int checkResponseCRC(unsigned char* answer, unsigned char* answer_data,
                             int answer_size, int take_byte_count, unsigned short crc);
        int preparePort(std::string port);

        std::set<unsigned char> sensor_addr;
        int fd;
        QSocketNotifier* portNotifier;
        Configurator * config;
        QVector<Message> msgQue;
};

#endif // MODBUS_H
