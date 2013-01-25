#include "linearconverter.h"

#include <limits>

#include "debug.h"
#include "staticassert.h"

const char SignedInt16[]    = "sint16";
const char SignedInt32[]    = "sint32";
const char UnsignedInt16[]  = "uint16";
const char UnsignedInt32[]  = "uint32";
const char Float4B[]        = "float";
const char Double[]         = "double";

const int DoubleWordCount   = 4;
const int FloatWordCount    = 2;

#define PRINT_MEM_DEBUG

//////////////////////////////////////////////
///fromString template and specializations////
//////////////////////////////////////////////

#define DEF_FROM_STRING(type, method) \
    template <> \
    type fromStringHelper<type>(const QString &value) {\
    return value.method(); \
    }

template <class Y>
Y fromStringHelper(const QString &value)
{
    qDebug() << "From string conversion not supported" << __PRETTY_FUNCTION__;
    return 0;
}

DEF_FROM_STRING(qint32, toInt)
DEF_FROM_STRING(qint16, toInt)
DEF_FROM_STRING(quint16, toUInt)
DEF_FROM_STRING(quint32, toUInt)
DEF_FROM_STRING(float, toFloat)
DEF_FROM_STRING(double, toDouble)

//end of fromString definitions

#define IF_CREATE_LIN_CONV(strTypeY, strTypeX, typeY, typeX) \
    if ( (serverType == strTypeY) && (hostType == strTypeX) ) {\
    c = new LinearConverterT<typeY, typeX>();\
    }
Converter *LinearConverter::createLinearConverter(const QString &serverType, const QString &hostType, const QString &aParam, const QString &bParam)
{
    //static assertions
    STATIC_ASSERT(sizeof(float) == FloatWordCount * sizeof(quint16), "The size of float is assumed to be 4 bytes.");
    STATIC_ASSERT(std::numeric_limits<float>::is_iec559, "The float is represented in IEEE 754 format");//the best check I know. This considers compiler. However the processor may store floats in different format
    STATIC_ASSERT(sizeof(double) == DoubleWordCount * sizeof(quint16), "The size of double is assumed to be 8 bytes.");
    STATIC_ASSERT(std::numeric_limits<double>::is_iec559, "The double is represented in IEEE 754 format");
    //end of assertions


    LinearConverter *c(0);

    //generated with generate-linear-converter-if-else.sh script
    IF_CREATE_LIN_CONV(SignedInt16, SignedInt16, qint16, qint16)
            else IF_CREATE_LIN_CONV(SignedInt16, SignedInt32, qint16, qint32)
            else IF_CREATE_LIN_CONV(SignedInt16, UnsignedInt16, qint16, quint16)
            else IF_CREATE_LIN_CONV(SignedInt16, UnsignedInt32, qint16, quint32)
            else IF_CREATE_LIN_CONV(SignedInt16, Float4B, qint16, float)
            else IF_CREATE_LIN_CONV(SignedInt16, Double, qint16, double)
            else IF_CREATE_LIN_CONV(SignedInt32, SignedInt16, qint32, qint16)
            else IF_CREATE_LIN_CONV(SignedInt32, SignedInt32, qint32, qint32)
            else IF_CREATE_LIN_CONV(SignedInt32, UnsignedInt16, qint32, quint16)
            else IF_CREATE_LIN_CONV(SignedInt32, UnsignedInt32, qint32, quint32)
            else IF_CREATE_LIN_CONV(SignedInt32, Float4B, qint32, float)
            else IF_CREATE_LIN_CONV(SignedInt32, Double, qint32, double)
            else IF_CREATE_LIN_CONV(UnsignedInt16, SignedInt16, quint16, qint16)
            else IF_CREATE_LIN_CONV(UnsignedInt16, SignedInt32, quint16, qint32)
            else IF_CREATE_LIN_CONV(UnsignedInt16, UnsignedInt16, quint16, quint16)
            else IF_CREATE_LIN_CONV(UnsignedInt16, UnsignedInt32, quint16, quint32)
            else IF_CREATE_LIN_CONV(UnsignedInt16, Float4B, quint16, float)
            else IF_CREATE_LIN_CONV(UnsignedInt16, Double, quint16, double)
            else IF_CREATE_LIN_CONV(UnsignedInt32, SignedInt16, quint32, qint16)
            else IF_CREATE_LIN_CONV(UnsignedInt32, SignedInt32, quint32, qint32)
            else IF_CREATE_LIN_CONV(UnsignedInt32, UnsignedInt16, quint32, quint16)
            else IF_CREATE_LIN_CONV(UnsignedInt32, UnsignedInt32, quint32, quint32)
            else IF_CREATE_LIN_CONV(UnsignedInt32, Float4B, quint32, float)
            else IF_CREATE_LIN_CONV(UnsignedInt32, Double, quint32, double)
            else IF_CREATE_LIN_CONV(Float4B, SignedInt16, float, qint16)
            else IF_CREATE_LIN_CONV(Float4B, SignedInt32, float, qint32)
            else IF_CREATE_LIN_CONV(Float4B, UnsignedInt16, float, quint16)
            else IF_CREATE_LIN_CONV(Float4B, UnsignedInt32, float, quint32)
            else IF_CREATE_LIN_CONV(Float4B, Float4B, float, float)
            else IF_CREATE_LIN_CONV(Float4B, Double, float, double)
            else IF_CREATE_LIN_CONV(Double, SignedInt16, double, qint16)
            else IF_CREATE_LIN_CONV(Double, SignedInt32, double, qint32)
            else IF_CREATE_LIN_CONV(Double, UnsignedInt16, double, quint16)
            else IF_CREATE_LIN_CONV(Double, UnsignedInt32, double, quint32)
            else IF_CREATE_LIN_CONV(Double, Float4B, double, float)
            else IF_CREATE_LIN_CONV(Double, Double, double, double)
            ;

    if (0 != c) {
        c->setParams(aParam, bParam);
    }

    return c;
}

//not a member method since those may not be partially specialized
template <class X>
void convertFromVector(X *value, const QVector<quint16> &inVector, bool isBigEndian)
{
#ifdef PRINT_MEM_DEBUG
    printf("LinearConverter : (is big endian %d) vector ", isBigEndian);
    for (int i = 0; i < inVector.count(); ++i) {
        LinearConverter::printBuff((unsigned char*)(&inVector.at(i)), sizeof(quint16), false);
    }
#endif

    *value = 0;
    const int word_count = inVector.count();
    switch (word_count) {
    case 8:
    case 4: {
        X multiplier = 0x01;;
        for (int i = 0; i < word_count; ++i) {
            if (isBigEndian) {
                (*value) += (multiplier * inVector[word_count - i - 1]);
            }
            else {
                (*value) += (multiplier * inVector[i]);
            }
            multiplier <<= 16;
        }
    }
        break;
    case 2:
        if (isBigEndian)
            *value = inVector[0] * 0x10000 + inVector[1];
        else
            *value = inVector[1] * 0x10000 + inVector[0];
        break;
    case 1:
    default:
        *value = inVector[0];
    }

#ifdef PRINT_MEM_DEBUG
    printf(" result value: ");
    LinearConverter::printBuff((unsigned char*)(value), sizeof(X), true);
#endif
}

union FloatUnion {
    quint32 ui32;
    float f;
};

union DoubleUnion {
    double d;
    quint64 u64;
};

template <>
void convertFromVector<double>(double *value, const QVector<quint16> &inVector, bool isBigEndian)
{
    DoubleUnion du;
    du.d = *value;

    if (DoubleWordCount == inVector.count()) {
        convertFromVector(&(du.u64), inVector, isBigEndian);
        *value = du.d;
    }
    else {
        qDebug() << __PRETTY_FUNCTION__ << "The size of the vector is not" << DoubleWordCount;
    }
}

template <>
void convertFromVector<float>(float *value, const QVector<quint16> &inVector, bool isBigEndian)
{
    FloatUnion du;
    du.f = *value;

    if (FloatWordCount == inVector.count()) {
        convertFromVector(&(du.ui32), inVector, isBigEndian);
        *value = du.f;
    }
    else {
        qDebug() << __PRETTY_FUNCTION__ << "The size of the vector is not" << FloatWordCount;
    }
}

template <class Y, class X>
Y LinearConverterT<Y, X>::convert(const QVector<quint16> &inVector, bool isBigEndian)
{
    X value;
    convertFromVector<X>(&value, inVector, isBigEndian);

    Y result = _aParam * value + _bParam;
    return result;
}

template <class Y, class X>
QString LinearConverterT<Y, X>::convertToString(const QVector<quint16> &inVector, bool isBigEndian)
{
    Y result = convert(inVector, isBigEndian);
    return QString::number(result);
}

//not a member method since those may not be partially specialized
template <class X>
void fillVector(QVector<quint16> *vector, X &value, bool isBigEndian, int word_count) {
#ifdef PRINT_MEM_DEBUG
    printf("LinearConverter : (is big endian %d) value", isBigEndian);
    LinearConverter::printBuff((unsigned char*)(&value), sizeof(X), false);
#endif

    switch(word_count)
    {
    case 8:
    case 4: {
        X vCopy = value;
        for (int i = 0; i < word_count; ++i) {
            if (isBigEndian) {
                vector->prepend(vCopy & 0xFFFF);//NOTE : this may be slow operation for a longer vectors (memory shift)
            }
            else {
                vector->append(vCopy & 0xFFFF);
            }
            vCopy >>= 16;
        }
    }
        break;
    case 2:
        if (isBigEndian) {
            vector->append((value >> 16) & 0xFFFF); // high
            vector->append(value & 0xFFFF); // low
        }
        else {
            vector->append(value & 0xFFFF); // low
            vector->append((value >> 16) & 0xFFFF); // high
        }
        break;
    case 1:
    default:
        vector->append(value & 0xFFFF);
    }

#ifdef PRINT_MEM_DEBUG
    printf(" result vector ");
    for (int i = 0; i < vector->count(); ++i) {
        LinearConverter::printBuff((unsigned char*)(&vector->at(i)), sizeof(quint16), false);
    }
    printf("\r\n");
    fflush(stdout);
#endif
}

template <>
void fillVector<float>(QVector<quint16> *vector, float &value, bool isBigEndian, int word_count) {
    const int FloatWordSize = 2;
    FloatUnion v;
    v.f = value;

    if (FloatWordSize == word_count) {
        fillVector(vector, v.ui32, isBigEndian, word_count);
    }
    else {
        qDebug() << __PRETTY_FUNCTION__ << "count != FloatWordSize" << word_count << FloatWordSize;
    }

}

template <>
void fillVector<double>(QVector<quint16> *vector, double &value, bool isBigEndian, int word_count) {
    const int DoubleWordSize = 4;
    STATIC_ASSERT(sizeof(double) == DoubleWordSize * sizeof(quint16), "The size of float is assumed to be 8 bytes.");
    DoubleUnion v;
    v.d = value;

    if (DoubleWordSize == word_count) {
        fillVector(vector, v.u64, isBigEndian, word_count);
    }
    else {
        qDebug() << __PRETTY_FUNCTION__ << "count != DoubleWordSize" << word_count << DoubleWordSize;
    }
}

template <class Y, class X>
QVector<quint16> LinearConverterT<Y, X>::revConvert(const QString &sampleVaue, bool isBigEndian, int word_count)
{
    QVector<quint16> vector;

    Y origValue = fromString(sampleVaue);
    if (0 == _aParam) {
        qDebug() << __PRETTY_FUNCTION__ << "Parameter a is zero -> division by zero";
        return vector;
    }
    X value = (origValue - _bParam) / _aParam;

    fillVector<X>(&vector, value, isBigEndian, word_count);
    return vector;
}

template <class Y, class X>
Y LinearConverterT<Y, X>::fromString(const QString &value)
{
    return fromStringHelper<Y>(value);
}

template <class Y, class X>
void LinearConverterT<Y, X>::setParams(const QString &a, const QString &b)
{
    _aParam = fromStringHelper<Y>(a);
    _bParam = fromStringHelper<Y>(b);
}

#include <typeinfo>
#define IF_TYPE_TO_HUMAN_THEN_RETURN(type) \
    if (0 == strcmp(typeIdName, typeid(type).name())) {\
        return #type;\
    }
QString typeIdToHuman(const char* typeIdName) {
    IF_TYPE_TO_HUMAN_THEN_RETURN(qint16)
            else IF_TYPE_TO_HUMAN_THEN_RETURN(qint32)
            else IF_TYPE_TO_HUMAN_THEN_RETURN(quint16)
            else IF_TYPE_TO_HUMAN_THEN_RETURN(quint32)
            else IF_TYPE_TO_HUMAN_THEN_RETURN(float)
            else IF_TYPE_TO_HUMAN_THEN_RETURN(double)
            else return "unknown";
}

template <class Y, class X>
QString LinearConverterT<Y, X>::description()
{
    return QString("linear: (%1)y = (%1)%3 * (%2)x + (%1)%4").
            arg(typeIdToHuman(typeid(Y).name())).
            arg(typeIdToHuman(typeid(X).name())).
            arg(_aParam).
            arg(_bParam);
}


