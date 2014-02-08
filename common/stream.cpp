/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#include "stream.h"
#include "hpack.h"
#include "connection.h"
#include "basicframe.h"
#include "headersframe.h"
#include "dataframe.h"
#include <QDebug>

namespace http2 {

Stream::Stream(Connection *connection, quint32 identifier, ConnectionType type) :
    QObject(connection),
    connection_(connection),
    context_({identifier, type})
{

}

void Stream::sendFrame(const IFrame& frame)
{
    qDebug() << "<<SENT>>\n" << frame.inspect();
    connection_->sendRawData(frame.dump(context_));
}

void Stream::receiveFrame(const BasicFrame& frame)
{
    switch (frame.type()) {
    case FRAME_HEADERS:
    {
        HeadersFrame* in = new HeadersFrame(frame, context_, this);
        qDebug() << "<<RECV>>\n" << in->inspect();
        emit headerReceived(context_.identifier, in);
    }
        break;
    case FRAME_DATA:
    {
        DataFrame* in = new DataFrame(frame, this);
        qDebug() << "<<RECV>>\n" << in->inspect();
        emit dataReceived(context_.identifier, in);
    }
        break;
    default:
        ;
    }
}

void Stream::setHeaderTableSize(int headerTableSize)
{
    context_.hpack.setHeaderTableSize(headerTableSize);
}

}
