#ifndef TOPOLOGYSENSOR_H
#define TOPOLOGYSENSOR_H

#include <QObject>

class TopologySensor : public QObject
{
    Q_OBJECT
public:
    explicit TopologySensor(QObject *parent = 0);

signals:

public slots:

};

#endif // TOPOLOGYSENSOR_H
