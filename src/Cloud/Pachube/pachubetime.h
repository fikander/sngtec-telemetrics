#ifndef PACHUBETIME_H
#define PACHUBETIME_H

#include <QDateTime>
#include <QString>

class PachubeTime {
    public:
        PachubeTime(const QString &);
        PachubeTime(const QDateTime &);
        static PachubeTime currentDataTime(); 
        QString toString();
        QString toPachubeFormat();
        QDateTime getDateTime();
        bool operator == (const PachubeTime &);
        bool operator != (const PachubeTime &);
        bool operator <= (const PachubeTime &);
        bool operator < (const PachubeTime &);
        bool operator > (const PachubeTime &);
        bool operator >= (const PachubeTime &);
    private:
        PachubeTime();
        QDateTime time;
};

inline
PachubeTime PachubeTime::currentDataTime() {
    return PachubeTime();
}

#endif
