#ifndef PACHUBETIME_H
#define PACHUBETIME_H

#include <QDateTime>
#include <QString>

class PachubeTime {
    public:
        PachubeTime(const QString &);
        PachubeTime(const QDateTime &);
        QString toString();
        QString toPachubeFormat();
        QDateTime getDateTime();
        bool operator == (const PachubeTime &);
    private:
        QDateTime time;
};

#endif
