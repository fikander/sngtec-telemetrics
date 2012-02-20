#ifndef PACHUBETIMETEST_H
#define PACHUBETIMETEST_H

#include <QObject>
#include "AutoTest.h"

class PachubetimeTest : public QObject{
    Q_OBJECT

    private slots:
        void stringconstructor_data();
        void stringconstructor();
        void toPachubeFormat_data();
        void toPachubeFormat();
        void compare_data();
        void compare();
        
};

#ifdef PACHUBE_TIME
DECLARE_TEST(PachubetimeTest);
#endif

#endif
