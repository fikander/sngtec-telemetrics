#ifndef COSM_H
#define COSM_H
#include <QHttp>
#include <QtXml>
#include <QString>
#include <QDebug>
#include <QObject>

class Cosm: public QObject{
    Q_OBJECT
    private:
        const QString feed;
        const QDomDocument xml;
        QHttp http;


    private slots:
        void done(bool);

    public:
        Cosm(const QString &apikey, const QString &feed, const QDomDocument &);

    signals:
        void aborted();
        void send();
};

#endif
