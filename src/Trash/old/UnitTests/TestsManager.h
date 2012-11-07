#ifndef TESTMANAGER_H
#define TESTMANAGER_H

#include <QTest>
#include <QList>
#include <QString>
#include <QSharedPointer>

//#define SAMPLE
//#define MODBUS
#define SNG
#define COSM_XML
#define COSM_TIME

#define DECLARE_TEST(className) static TestCase<className> t(#className);


inline QList<QObject*>& getListOfTests()
{
    static QList<QObject*> listOfTests;
    return listOfTests;
}

inline void addTest(QObject* object)
{
    QList<QObject*>& listOfTests = getListOfTests();
    foreach (QObject* test, listOfTests)
    {
        if (test->objectName() == object->objectName())
        {
            return;
        }
    }

    listOfTests.append(object);
}


template <class T>
class TestCase
{
public:
    QSharedPointer<T> test;

    TestCase(const QString& name) : test(new T)
    {
        test->setObjectName(name);
        addTest(test.data());
    }
};


inline int runTests(int argc, char *argv[])
{
    int res = 0;

    foreach (QObject* test, getListOfTests())
    {
        res += QTest::qExec(test, argc, argv);
    }

    return res;
}

#endif
