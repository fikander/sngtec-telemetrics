#include "sampletest.h"

void SampleTest::initTestCase()
{
}

void SampleTest::cleanupTestCase()
{
}

void SampleTest::testMethod()
{
    QCOMPARE(1, 1);
}

void SampleTest::testMethodFail()
{
    QCOMPARE(0, 1);
}
