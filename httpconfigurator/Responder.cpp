#include "Responder.h"

#include <QRegExp>
#include <QStringList>
#include <QDebug>

#include <qhttprequest.h>
#include <qhttpresponse.h>

Responder::Responder(QHttpRequest *req, QHttpResponse *resp)
    : QObject(0)
    , m_req(req)
    , m_resp(resp)
{
    connect(m_req, SIGNAL(data(const QByteArray&)), this, SLOT(accumulate(const QByteArray&)));
    connect(m_req, SIGNAL(end()), this, SLOT(reply()));
    connect(m_resp, SIGNAL(done()), this, SLOT(deleteLater()));
}

Responder::~Responder()
{
    qDebug() << "DELETING" << m_req;
    delete m_req;
    m_req = 0;
}

void Responder::accumulate(const QByteArray &data)
{
    this->data += data;
}

void Responder::reply()
{
    m_resp->end(QString("dummy response").toAscii());
}

