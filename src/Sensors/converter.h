#ifndef CONVERTER_H
#define CONVERTER_H

#include <QString>
#include <QVector>

class Converter {
public:
    virtual QString convertToString(const QVector<quint16> &inVector, bool isBigEndian) = 0;
    virtual QVector<quint16> revConvert(const QString &sampleVaue, bool isBigEndian, int word_count) = 0;

    virtual QString description() {return "no description available";}
};

#endif // CONVERTER_H
