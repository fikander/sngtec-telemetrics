#ifndef MODBUS_H
#define MODBUS_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string>
#include <QSocketNotifier>
#include "src/Message/message.h"
#include "src/Sensors/devconnection.h"

struct ModbusRtuFrame
{
        unsigned char addr;
        unsigned char function;
        unsigned char* data;
        unsigned short int crc;
};


class Modbus : public DevConnection
{

  public:
        Modbus* create(Configurator *config);
        Modbus* clone(Configurator *config);
        Modbus(std::string port, std::string connection);
        virtual ~Modbus();
        void write(QVector<Message> messages);
        QVector<Message> readAll();
        int connect(unsigned char addr);
  private:
        void decodeMessage(Message msg);
        int preparePort(std::string port);

        // Add queue / vector of sensors for rs458
        unsigned char sensor_addr;
        int fd;
        int frame_length;
        QSocketNotifier* portNotifier;
};

#endif // MODBUS_H
