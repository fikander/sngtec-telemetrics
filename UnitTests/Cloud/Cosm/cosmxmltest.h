#ifndef COSMXMLTEST_H
#define COSMXMLTEST_H

#include <QObject>
#include "AutoTest.h"

class CosmxmlTest : public QObject{
    Q_OBJECT

    private slots:
        void addingDataTest();
        void getMessagesTest();
        
};

#ifdef COSM_XML
DECLARE_TEST(CosmxmlTest);
#endif

#endif
