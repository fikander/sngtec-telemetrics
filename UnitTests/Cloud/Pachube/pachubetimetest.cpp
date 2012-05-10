#include "cosmtimetest.h"
#include "src/Cloud/Cosm/cosmtime.h"
#include <QDateTime>
#include <QDate>
#include <QTime>

void CosmtimeTest::stringconstructor_data(){
    QTest::addColumn<QString>("string");
    QTest::addColumn<QDateTime>("datetime");

    QTest::newRow("cosm format") << "2012-01-21T15:55:58.930000Z"
        << QDateTime( QDate(2012, 1, 21), QTime(15,55,58,930));
}

void CosmtimeTest::stringconstructor() {
    QFETCH(QString, string);
    QFETCH(QDateTime, datetime);
    CosmTime time(string);
    QCOMPARE(time.getDateTime(), datetime);
}

void CosmtimeTest::toCosmFormat_data() {
    QTest::addColumn<QString>("string");
    QTest::addColumn<QDateTime>("datetime");

    QTest::newRow("cosm format") << "2012-01-21T15:55:58.930000Z"
        << QDateTime( QDate(2012, 1, 21), QTime(15,55,58,930));
}

void CosmtimeTest::toCosmFormat() {
    QFETCH(QString, string);
    QFETCH(QDateTime, datetime);
    CosmTime time(datetime);
    QCOMPARE(time.toCosmFormat(), string);

}


void CosmtimeTest::compare_data(){
    QTest::addColumn<QString>("string");
    QTest::addColumn<QDateTime>("datetime");

    QTest::newRow("cosm format") << "2012-01-21T15:55:58.930000Z"
        << QDateTime( QDate(2012, 1, 21), QTime(15,55,58,930));
}
void CosmtimeTest::compare() {
    QFETCH(QString, string);
    QFETCH(QDateTime, datetime);

    CosmTime time1(string);
    CosmTime time2(datetime);
    QVERIFY(time1 == time2);
}
