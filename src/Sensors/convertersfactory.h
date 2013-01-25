#ifndef CONVERTERSFACTORY_H
#define CONVERTERSFACTORY_H

#include <QString>

class Converter;
class ConvertersFactory
{
public:
    static Converter *create(const QString &type, const QString &params, bool *isNone = 0);
};

#endif // CONVERTERSFACTORY_H
