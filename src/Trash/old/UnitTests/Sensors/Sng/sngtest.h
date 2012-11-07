#ifndef SNGTEST_H
#define SNGTEST_H

#include <QObject>
#include "TestsManager.h"
#include "Sensors/SNG/sngconnection.h"

class SngTest : public QObject
{
    Q_OBJECT

private slots:
    void prepareMsgTestSrc();
    void prepareMsgTestSrc_data();

public slots:

};

#ifdef SNG
DECLARE_TEST(SngTest);
#endif

#endif // SNGTEST_H
