#ifndef MODBUS_H
#define MODBUS_H

#include "src/Message/message.h"
#include "src/Sensors/devconnection.h"
#include "modbusrtuframe.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string>
#include <QSocketNotifier>


/* TODO : map : function->rtuSize*/

class Modbus : public DevConnection {
   // Q_OBJECT - strange problem

  public:
        Modbus* create(Configurator *config);
        Modbus* clone(Configurator *config);
        virtual ~Modbus();

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

        int preparePort(std::string port);

        //vector of sensors for rs458
        unsigned char sensor_addr;
        int fd, fd2;
        QSocketNotifier* portNotifier;
        Configurator * config;

        QVector<Message> msgQue;

};

#endif // MODBUS_H
