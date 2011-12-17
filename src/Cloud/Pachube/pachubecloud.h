#ifndef MOCK_CLOUD_H
#define MOCK_CLOUD_H

#include "src/Cloud/cloconnection.h"
#include "pachubexml.h"
#include <QHttp>

class Configurator;

//TODO: timeout
//TODO: retry time
class PachubeCloud: public CloConnection{
    Q_OBJECT
    
public:
    PachubeCloud(Configurator &);
    virtual CloConnection* create(Configurator &);
    virtual CloConnection* clone(Configurator &);
    virtual ~PachubeCloud();
    virtual void write(QVector<Message>);
    virtual QVector<Message> readAll();
    virtual bool isBusy();

private:
    void send();

    PachubeXml currentPachubeXml;
    QString feed;
    QString apiKey;
    bool busy;
    QHttp http;

public slots:
    void done(bool);
    void retry();

};
    
#endif
