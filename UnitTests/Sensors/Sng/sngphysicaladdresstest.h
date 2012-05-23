#ifndef SNGPHYSICALADDRESSTEST_H
#define SNGPHYSICALADDRESSTEST_H

#include "TestsManager.h"
#include "Sensors/SNG/sngphysicaladdress.h"

class SngPhysicalAddressTest : public QObject
{
    Q_OBJECT

private slots:
    void constructorTest();
    void constructorTest_data();
};

#ifdef SNG
DECLARE_TEST(SngPhysicalAddressTest);
#endif

#endif // SNGPHYSICALADDRESSTEST_H
