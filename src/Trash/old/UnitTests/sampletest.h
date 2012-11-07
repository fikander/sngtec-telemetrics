#ifndef SAMPLETEST_H
#define SAMPLETEST_H

#include "TestsManager.h"

class SampleTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testMethod();
    void testMethodFail();
    void cleanupTestCase();
};

#ifdef SAMPLE
DECLARE_TEST(SampleTest)
#endif

#endif // SAMPLETEST_H
