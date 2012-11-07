#ifndef MOCK_CLOUD_H
#define MOCK_CLOUD_H

#include "Cloud/cloconnection.h"
#include <QTimer>

class MockCloud: public CloConnection{
    Q_OBJECT

public: 
    MockCloud();
    virtual CloConnection* create(Configurator*);
    virtual CloConnection* clone(Configurator*);
    virtual ~MockCloud();
    virtual void connect();
    virtual void write(QVector<MessageSample>);
    virtual QVector<MessageSample> readAll();
    virtual bool isBusy();

private:
    QTimer receive;

public slots:
    void receiveFakeData();
};
    
#endif
