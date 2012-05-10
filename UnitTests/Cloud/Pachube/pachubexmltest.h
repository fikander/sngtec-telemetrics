#ifndef PACHUBEXMLTEST_H
#define PACHUBEXMLTEST_H

#include <QObject>
#include "AutoTest.h"

class CosmxmlTest : public QObject{
    Q_OBJECT

    private slots:
        void addingDataTest();
        void getMessagesTest();
        
};

#ifdef PACHUBE_XML
DECLARE_TEST(CosmxmlTest);
#endif

#endif
