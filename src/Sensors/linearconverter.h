#ifndef LINEARCONVERTER_H
#define LINEARCONVERTER_H

#include <QString>
#include <QVector>

#include "converter.h"

#include <cstdio>

class LinearConverter:
        public Converter
{
public:
    static Converter *createLinearConverter(const QString &serverType, const QString &hostType, const QString &aParam, const QString &bParam);
    virtual void setParams(const QString &a, const QString &b) = 0;

    static void printBuff(unsigned char *buff, int size, bool finishWithNewLine = true) {
        printf("0x");
        for (int i = 0; i < size; ++i) {
            printf("%.2x ", buff[i]);
        }
        if (finishWithNewLine)
            printf("\r\n");
        fflush(stdout);
    }
};

template <class Y, class X>
class LinearConverterT:
        public LinearConverter
{
public:
    virtual QString convertToString(const QVector<quint16> &inVector, bool isBigEndian);
    Y convert(const QVector<quint16> &inVector, bool isBigEndian);

    virtual QVector<quint16> revConvert(const QString &sampleVaue, bool isBigEndian, int word_count);
    Y fromString(const QString &value);

    void setParams(const QString &a, const QString &b);

    virtual QString description();

private:
    Y _aParam;
    Y _bParam;


};


#endif // LINEARCONVERTER_H
