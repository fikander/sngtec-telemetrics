#ifndef DEBUG_H
#define DEBUG_H

#include <QDebug>

#define VERSION "0.1"

/*
 * Some useful debug defines
 */

#define QDEBUG qDebug() << "[" << this->metaObject()->className() << "]: "
#define QWARNING qWarning() << "[" << this->metaObject()->className() << "]: "
#define QERROR QDebug(QtCriticalMsg) << "[" << this->metaObject()->className() << "]: "

#endif // DEBUG_H
