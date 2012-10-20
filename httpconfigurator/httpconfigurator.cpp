#include "httpconfigurator.h"

#include <QCoreApplication>
#include <QRegExp>
#include <QStringList>
#include <QSettings>
#include <QSize>
#include <QDebug>

#include <qhttpserver.h>
#include <qhttprequest.h>
#include <qhttpresponse.h>

#include "SetSettingsResponder.h"

HttpConfigurator::HttpConfigurator(QSettings *settings):
    settings(settings)
{
    QHttpServer *server = new QHttpServer;
    server->listen(QHostAddress::Any, 5000);
    connect(server, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
            this, SLOT(handle(QHttpRequest*, QHttpResponse*)));
}

void HttpConfigurator::handle(QHttpRequest *req, QHttpResponse *resp)
{
    QRegExp expListSettings("^/list$");
    QRegExp expSetSettings("^/set$");

    if (expListSettings.indexIn(req->path()) != -1)
    {
        handleListSettings(req, resp);
    }
    else if (expSetSettings.indexIn(req->path()) != -1)
    {
        SetSettingsResponder *r = new SetSettingsResponder(req, resp, settings);
    }
    else
    {
        resp->writeHead(404);
        resp->end("Page not found");
    }
}

void HttpConfigurator::handleListSettings(QHttpRequest *req, QHttpResponse *resp)
{
    resp->setHeader("Content-Type", "text/html");
    resp->writeHead(200);
    QString html ="<html><head><title>Http Configurator</title></head><body>";

    html += "<form name=\"input\" action=\"set\" method=\"post\"> <ul>";

    settings->sync();
    foreach(QString key, settings->allKeys()) {
        html += "<li>";
        html += key + " ";
        html += "<input type=\"text\" name=\"" + key + "\" value=\""+settings->value(key).toString()+"\">";
        html += "</li>";
    }
    html += "</ul><input type=\"submit\" value=\"Set\"></form>";
    html += "</body></html>";

    resp->end(html);
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QSettings settings("telemetron.ini", QSettings::IniFormat);
    qDebug() << settings.fileName();

    QStringList all = settings.allKeys();
    if (all.count() == 0)
    {
        // init config file
        settings.beginGroup("owner");
        settings.setValue("device/type", "<empty>");
        settings.setValue("device/description", "<empty>");
        settings.endGroup();

        settings.beginGroup("cloud");
        settings.setValue("type", "mock");
        settings.setValue("apikey", "n/a");
    }

    HttpConfigurator config(&settings);

    app.exec();
}
