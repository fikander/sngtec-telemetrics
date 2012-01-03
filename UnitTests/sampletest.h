#ifndef SAMPLETEST_H
#define SAMPLETEST_H

#include "AutoTest.h"

class SampleTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testMethod();
    void testMethodFail();
    void cleanupTestCase();
};

DECLARE_TEST(SampleTest)

#endif // SAMPLETEST_H
