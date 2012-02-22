#include "pachubetime.h"
#include <QDate>
#include <QTime>


PachubeTime::PachubeTime() : time (QDateTime::currentDateTime()) {
}

PachubeTime::PachubeTime(const QString &timeString) {
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

PachubeTime::PachubeTime(const QDateTime &time): time(time) { }

QString PachubeTime::toString() {
    return time.toString("dd.MM.yyyy hh:mm:ss.zzz");
}

QString PachubeTime::toPachubeFormat() {
    return time.toString("yyyy-MM-ddThh:mm:ss.zzz000Z");
}

QDateTime PachubeTime::getDateTime() {
    return time;
}

bool PachubeTime::operator == (const PachubeTime &rtime) {
    return this->time == rtime.time;
}

bool PachubeTime::operator != (const PachubeTime & rtime){
    return this->time != rtime.time;
}

bool PachubeTime::operator <= (const PachubeTime & rtime) {
    return this->time <= rtime.time;
}

bool PachubeTime::operator < (const PachubeTime & rtime) {
    return this->time < rtime.time;
}

bool PachubeTime::operator > (const PachubeTime & rtime) {
    return this->time > rtime.time;
}

bool PachubeTime::operator >= (const PachubeTime & rtime) {
    return this->time >= rtime.time;
}
