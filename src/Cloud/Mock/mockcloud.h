#ifndef MOCK_CLOUD_H
#define MOCK_CLOUD_H

#include "src/Cloud/cloconnection.h"

class MockCloud: CloConnection{
public: 
    virtual CloConnection* create(Configurator*);
    virtual CloConnection* clone(Configurator*);
    virtual ~MockCloud();
    virtual void write(QVector<Message>);
    virtual QVector<Message> readAll();
    virtual bool isBusy();
private:
};
    
#endif
