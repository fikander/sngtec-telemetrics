#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QIODevice>
//#include "port.h"
#include <QString>

/* Test class! */
class Protocol  {

public:
    //virtual int connect();
    //virtual int send(QString mesg);
   // virtual QString getAns();
    Protocol();
    int readTest();
    QString read();
    int write(QString string);

private:
  //  port myPort;
    int test;
};

#endif // PROTOCOL_H
