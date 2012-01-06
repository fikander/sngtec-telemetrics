#ifndef MODBUSTEST_H
#define MODBUSTEST_H


#include "AutoTest.h"

class ModbusTest : public QObject
{
    Q_OBJECT

private slots:
    void testCase1();
    void testCase1_data();

};

DECLARE_TEST(ModbusTest)

#endif // MODBUSTEST_H
