#include "message.h"

Message::Message() {
}

Message::Message(QString k, QString v) {
    key = k;
    value = v;
    timestamp = QDateTime::currentDateTime();
}

Message::Message(QString k, QString v, QDateTime t) {
    key = k;
    value = v;
    timestamp = t; 
}
