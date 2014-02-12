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
#include "windowupdateframe.h"
#include <QDebug>

namespace http2 {

Stream::Stream(Connection *connection, quint32 identifier, ConnectionType type) :
    QObject(connection),
    connection_(connection),
    context_({identifier, type}),
    windowSize_(66535),
    windowConsumed_(0)
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

        windowConsumed_ += in->payload().size();
        if (windowConsumed_ >= windowSize_) {
            windowConsumed_ = 0;
            WindowUpdateFrame frame;
            frame.setWindowSizeIncrement(windowSize_);
            sendFrame(frame);
        }
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

int Stream::windowSize() const
{
    return windowSize_;
}

void Stream::setWindowSize(int windowSize)
{
    windowSize_ = windowSize;
}

}
