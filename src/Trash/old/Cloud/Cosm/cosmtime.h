#ifndef COSMTIME_H
#define COSMTIME_H

#include <QDateTime>
#include <QString>

class CosmTime {
    public:
        CosmTime(const QString &);
        CosmTime(const QDateTime &);
        static CosmTime currentDataTime(); 
        QString toString();
        QString toCosmFormat();
        QDateTime getDateTime();
        bool operator == (const CosmTime &);
        bool operator != (const CosmTime &);
        bool operator <= (const CosmTime &);
        bool operator < (const CosmTime &);
        bool operator > (const CosmTime &);
        bool operator >= (const CosmTime &);
    private:
        CosmTime();
        QDateTime time;
};

inline
CosmTime CosmTime::currentDataTime() {
    return CosmTime();
}

#endif
