#include "cloconnection.h"

CloConnection* CloConnection::create(Configurator &config) {};
CloConnection* CloConnection::clone(Configurator &config) {};
CloConnection::~CloConnection() {};

void CloConnection::write(QVector<Message>) {};
QVector<Message> CloConnection::readAll() {};

