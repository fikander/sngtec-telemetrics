#ifndef RESPONDER_H
#define RESPONDER_H

#include <QObject>

class QHttpRequest;
class QHttpResponse;

class Responder : public QObject
{
    Q_OBJECT
public:
    Responder(QHttpRequest *req, QHttpResponse *resp);
    virtual ~Responder();
signals:
    void done();
private slots:
    void accumulate(const QByteArray&);
    virtual void reply();
protected:
    QHttpRequest *m_req;
    QHttpResponse *m_resp;
    QByteArray data;
};

#endif // RESPONDER_H

