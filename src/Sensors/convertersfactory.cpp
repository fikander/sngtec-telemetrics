#include "convertersfactory.h"

#include <QStringList>

#include "linearconverter.h"
#include "debug.h"

const char LinearType[] = "linear";
const char NoneType[]   = "none";

Converter *ConvertersFactory::create(const QString &type, const QString &params, bool *isNone)
{
    if (0 != isNone)
        *isNone = false;

    if (NoneType == type) {
        if (0 != isNone)
            *isNone = true;
    }
    else if (LinearType == type) {
        //params = serverType,modbusType,aParam,bParam;
        QStringList list = params.split('_');
        if (4 ==  list.count()) {
            return LinearConverter::createLinearConverter(list[0], list[1], list[2], list[3]);
        }
        else {
            qDebug() << __PRETTY_FUNCTION__ << "Incorrect number of parameters" << params;
        }
    }
    else {
        qDebug() << __PRETTY_FUNCTION__ << "Unknown type" << type;
    }

    return 0;
}

//#define TEST_LINEAR_CONVERTER
#ifdef TEST_LINEAR_CONVERTER

union FloatUniont {
    float f;
    quint32 v;
    unsigned char c[4];
};

union DoubleUniont {
    double d;
    quint64 v;
    unsigned char c[8];
};

//test
int main() {
    if (1) {
        Converter *fc = ConvertersFactory::create(LinearType, "float,uint32,0.1,0", true);

        QString value = "1.2";//cannot have more decimal places after the separator than the a parameter
        qDebug() << "The number is" << value;
        QVector<quint16> vBig = fc->revConvert(value, true, 2);
        QVector<quint16> vLittle = fc->revConvert(value, false, 2);

        QString strValue = fc->convertToString(vBig, true);
        qDebug() << strValue;
        Q_ASSERT(value == strValue);
        strValue = fc->convertToString(vLittle, false);
        qDebug() << strValue;
        Q_ASSERT(value == strValue);

        delete fc;
    }

    if (0) {
        Converter *fc = ConvertersFactory::create(LinearType, "float,float,1,0", true);
        FloatUniont fu;

        fu.c[0] = 0x01;
        fu.c[1] = 0x02;
        fu.c[2] = 0x03;
        fu.c[3] = 0x04;

        QString value = QString::number(fu.f);
        qDebug() << "The number is" << fu.f;
        QVector<quint16> vBig = fc->revConvert(QString::number(fu.f), true, 2);
        QVector<quint16> vLittle = fc->revConvert(QString::number(fu.f), false, 2);

        QString strValue = fc->convertToString(vBig, true);
        qDebug() << strValue;
        Q_ASSERT(value == strValue);
        strValue = fc->convertToString(vLittle, false);
        qDebug() << strValue;
        Q_ASSERT(value == strValue);

        delete fc;
    }

    if (0) {
        Converter *dc = ConvertersFactory::create(LinearType, "double,double,1,0", true);
        DoubleUniont du;

        du.d = 1.234;

        qDebug() << "The number is" << du.d;
        QVector<quint16> vBig1 = dc->revConvert(QString::number(du.d), true, 4);
        QVector<quint16> vLittle1 = dc->revConvert(QString::number(du.d), false, 4);

        QString strValue = dc->convertToString(vBig1, true);
        qDebug() << strValue;
        strValue = dc->convertToString(vLittle1, false);
        qDebug() << strValue;

        delete dc;
    }

    if (1) {//real data from a heat pump - size s116, factor 10 -> results in a Celsius degrees
        Converter *nibeConverter = ConvertersFactory::create(LinearType, "float,sint16,0.1,0", true);

        quint16 tempValue;
        char *tvPtr = (char*)&tempValue;
        tvPtr[0] = 0xf2;
        tvPtr[1] = 0xff;

        qint16 t = tempValue;

        qDebug() << tempValue << t;

        QVector<quint16> tempVector; tempVector.append(tempValue);

        qDebug() << "Converter temperature is" << nibeConverter->convertToString(tempVector, true);
        qDebug() << "Converter temperature is" << nibeConverter->convertToString(tempVector, false);
    }

    return 0;
}

#endif //TEST_LINEAR_CONVERTER
