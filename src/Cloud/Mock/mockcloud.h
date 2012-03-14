#ifndef MOCK_CLOUD_H
#define MOCK_CLOUD_H

#include "Cloud/cloconnection.h"

class MockCloud: public CloConnection{
public: 
    virtual CloConnection* create(Configurator*);
    virtual CloConnection* clone(Configurator*);
    virtual ~MockCloud();
    virtual void connect();
    virtual void write(QVector<Message>);
    virtual QVector<Message> readAll();
    virtual bool isBusy();
private:
};
    
#endif
