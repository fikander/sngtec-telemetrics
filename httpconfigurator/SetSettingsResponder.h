#ifndef SETSETTINGSRESPONDER_H
#define SETSETTINGSRESPONDER_H

#include <QObject>
#include <QSettings>

#include "Responder.h"

class SetSettingsResponder : public Responder
{
    Q_OBJECT
public:
    SetSettingsResponder(QHttpRequest *req, QHttpResponse *resp, QSettings *setttings);

private slots:
    virtual void reply();

private:
    QSettings *settings;
};

#endif // SETSETTINGSRESPONDER_H
