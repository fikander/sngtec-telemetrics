#include "SetSettingsResponder.h"

#include <QUrl>

#include <qhttprequest.h>
#include <qhttpresponse.h>


SetSettingsResponder::SetSettingsResponder(QHttpRequest *req, QHttpResponse *resp, QSettings *settings):
    Responder(req, resp), settings(settings)
{
}

void SetSettingsResponder::reply()
{

    QList<QByteArray> parameters = data.split('&');

    foreach(QByteArray parameter, parameters)
    {
        QList<QByteArray> kv = parameter.split('=');

        QString key = QUrl::fromPercentEncoding(kv[0]);
        QString value = QUrl::fromPercentEncoding(kv[1]);

        settings->setValue(key, value);
        settings->sync();
    }

    // redirect to main page
    m_resp->setHeader("Location", "list");
    m_resp->writeHead(303);
}
