#include "pachubetimetest.h"
#include "src/Cloud/Pachube/pachubetime.h"
#include <QDateTime>
#include <QDate>
#include <QTime>

void PachubetimeTest::stringconstructor_data(){
    QTest::addColumn<QString>("string");
    QTest::addColumn<QDateTime>("datetime");

    QTest::newRow("pachube format") << "2012-01-21T15:55:58.930000Z"
        << QDateTime( QDate(2012, 1, 21), QTime(15,55,58,930));
}

void PachubetimeTest::stringconstructor() {
    QFETCH(QString, string);
    QFETCH(QDateTime, datetime);
    PachubeTime time(string);
    QCOMPARE(time.getDateTime(), datetime);
}

void PachubetimeTest::toPachubeFormat_data() {
    QTest::addColumn<QString>("string");
    QTest::addColumn<QDateTime>("datetime");

    QTest::newRow("pachube format") << "2012-01-21T15:55:58.930000Z"
        << QDateTime( QDate(2012, 1, 21), QTime(15,55,58,930));
}

void PachubetimeTest::toPachubeFormat() {
    QFETCH(QString, string);
    QFETCH(QDateTime, datetime);
    PachubeTime time(datetime);
    QCOMPARE(time.toPachubeFormat(), string);

}


void PachubetimeTest::compare_data(){
    QTest::addColumn<QString>("string");
    QTest::addColumn<QDateTime>("datetime");

    QTest::newRow("pachube format") << "2012-01-21T15:55:58.930000Z"
        << QDateTime( QDate(2012, 1, 21), QTime(15,55,58,930));
}
void PachubetimeTest::compare() {
    QFETCH(QString, string);
    QFETCH(QDateTime, datetime);

    PachubeTime time1(string);
    PachubeTime time2(datetime);
    QVERIFY(time1 == time2);
}
