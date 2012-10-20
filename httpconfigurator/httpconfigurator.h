#include <QObject>
#include <QSettings>

class QHttpRequest;
class QHttpResponse;

class HttpConfigurator : public QObject
{
    Q_OBJECT
public:
    HttpConfigurator(QSettings *settings);

private slots:
    void handle(QHttpRequest *req, QHttpResponse *resp);

private:
    QSettings *settings;

    void handleListSettings(QHttpRequest *req, QHttpResponse *resp);
    void handleSetSettings(QHttpRequest *req, QHttpResponse *resp);
};
