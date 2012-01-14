#ifndef MODBUSTEST_H
#define MODBUSTEST_H


#include "AutoTest.h"

class ModbusTest : public QObject
{
    Q_OBJECT

private slots:
    void simpleTest();
    void simpleTest_data();

private:
    void testPort();
    int fd;
};

#ifdef MODBUS
DECLARE_TEST(ModbusTest)
#endif

#endif // MODBUSTEST_H
