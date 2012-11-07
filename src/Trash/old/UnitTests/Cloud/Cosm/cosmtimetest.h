#ifndef COSMTIMETEST_H
#define COSMTIMETEST_H

#include <QObject>
#include "TestsManager.h"

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

#ifdef COSM_TIME
DECLARE_TEST(CosmtimeTest);
#endif

#endif
