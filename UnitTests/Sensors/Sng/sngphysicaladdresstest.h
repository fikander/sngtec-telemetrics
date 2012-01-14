#ifndef SNGPHYSICALADDRESSTEST_H
#define SNGPHYSICALADDRESSTEST_H

#include "AutoTest.h"
#include "src/Sensors/SNG/sngphysicaladdress.h"

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
