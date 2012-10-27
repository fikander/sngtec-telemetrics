#include "debug.h"

#include "Cloud/Cloud.h"
#include "CommandProcessor.h"
#include "Logger/logger.h"

CommandProcessor::CommandProcessor(KeyValueMap &config, QObject *parent) :
    Sensor(parent)
{
}

CommandProcessor::~CommandProcessor()
{
}

void CommandProcessor::send(QSharedPointer<Message> payload)
{
    if (payload->getType() != Message::MsgRequest)
        return;

    QSharedPointer<MessageRequest> request = payload.staticCast<MessageRequest>();

    //TODO: process commands, e.g. send logs
    QObject *sender = this->sender();
    Cloud *cloud = qobject_cast<Cloud*>(sender);

    if (request->command == "send_log")
    {
        KeyValueMap arguments = request->arguments;
        arguments["log"] = Logger::getInstance()->giveLogsAsString();

        QSharedPointer<MessageResponse> response(new MessageResponse(request->command, arguments));

        // send response directly to the clousd which requested it
        cloud->send(response);
        //emit received(response);
    }

    QDEBUG << "Processed command: " + payload->toString();
    payload.clear();
}


void CommandProcessor::connect()
{
    QDEBUG << "Command Processor initialised";
}
