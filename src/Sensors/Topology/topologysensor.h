#ifndef TOPOLOGYSENSOR_H
#define TOPOLOGYSENSOR_H

#include "Sensors/devconnection.h"
#include <QTcpServer>
#include <QTcpSocket>

class TopologySensor : public DevConnection
{
    Q_OBJECT

public:
    virtual DevConnection* create(Configurator *, int);
    virtual DevConnection* clone(Configurator *, int);
    virtual ~TopologySensor();

    virtual void write(QVector<Message>);
    virtual QVector<Message> readAll();
    TopologySensor(Configurator*, int);
    TopologySensor();
signals:
    void readyToRead();

private slots:
    void handleConnection();
    void readData();
    void deleteConnection();
    void handleError(QAbstractSocket::SocketError);

private:

    void sendMsg();

    QTcpServer *tcpServer;
    QTcpSocket *clientConnection;
    qint16 port;

    QVector<Message> msgQue;
    QVector<Message> msgToSend;
    enum ReadingState { READ_key = 0,  READ_value = 1,  READ_timestamp = 2,  READ_max = 3};
    static int max_messages;
    static int max_messages_size;

    int messages_toread;
    QString read_values[3];
    int read_state;
};

#endif // TOPOLOGYSENSOR_H
