#include "cosmtime.h"
#include <QDate>
#include <QTime>


CosmTime::CosmTime() : time (QDateTime::currentDateTime()) {
}

CosmTime::CosmTime(const QString &timeString) {
    int year = timeString.mid(0, 4).toInt();
    int month = timeString.mid(5, 2).toInt();
    int day = timeString.mid(8, 2).toInt();
    int hour = timeString.mid(11, 2).toInt();
    int minute = timeString.mid(14, 2).toInt();
    int second = timeString.mid(17, 2).toInt();
    int miliseconds = timeString.mid(20,3).toInt();

    QTime time(hour, minute, second, miliseconds);
    QDate date(year, month, day);

    this->time = QDateTime(date, time);
}

CosmTime::CosmTime(const QDateTime &time): time(time) { }

QString CosmTime::toString() {
    return time.toString("dd.MM.yyyy hh:mm:ss.zzz");
}

QString CosmTime::toCosmFormat() {
    return time.toString("yyyy-MM-ddThh:mm:ss.zzz000Z");
}

QDateTime CosmTime::getDateTime() {
    return time;
}

bool CosmTime::operator == (const CosmTime &rtime) {
    return this->time == rtime.time;
}

bool CosmTime::operator != (const CosmTime & rtime){
    return this->time != rtime.time;
}

bool CosmTime::operator <= (const CosmTime & rtime) {
    return this->time <= rtime.time;
}

bool CosmTime::operator < (const CosmTime & rtime) {
    return this->time < rtime.time;
}

bool CosmTime::operator > (const CosmTime & rtime) {
    return this->time > rtime.time;
}

bool CosmTime::operator >= (const CosmTime & rtime) {
    return this->time >= rtime.time;
}
