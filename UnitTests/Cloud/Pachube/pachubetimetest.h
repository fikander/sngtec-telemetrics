#ifndef PACHUBETIMETEST_H
#define PACHUBETIMETEST_H

#include <QObject>
#include "AutoTest.h"

class CosmtimeTest : public QObject{
    Q_OBJECT

    private slots:
        void stringconstructor_data();
        void stringconstructor();
        void toCosmFormat_data();
        void toCosmFormat();
        void compare_data();
        void compare();
        
};

#ifdef PACHUBE_TIME
DECLARE_TEST(CosmtimeTest);
#endif

#endif
