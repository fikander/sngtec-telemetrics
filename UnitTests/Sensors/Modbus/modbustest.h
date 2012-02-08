#ifndef MODBUSTEST_H
#define MODBUSTEST_H


#include "AutoTest.h"

class ModbusTest : public QObject
{
    Q_OBJECT

private slots:
    void f1_2_3_4_Snd();
    void f1_2_3_4_Snd_data();
    void errorRec();
    void errorRec_data();
    void f1_2_3_4_Rec();
    void f1_2_3_4_Rec_data();
    void f5_6_11_15_Rec();
    void f5_6_11_15_Rec_data();
    void f7_11_12_Snd();
    void f7_11_12_Snd_data();
    void f7_Rec();
    void f7_Rec_data();
    void f0C_Rec();
    void f0C_Rec_data();

private:
    void testPort();
    short countCRC(unsigned char* array, int array_length);
    int fd;
};

#ifdef MODBUS
DECLARE_TEST(ModbusTest)
#endif

#endif // MODBUSTEST_H
