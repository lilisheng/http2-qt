/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#include "connection.h"
#include "stream.h"
#include "settingsframe.h"
#include "goawayframe.h"
#include "pingframe.h"
#include <QTcpSocket>
#include <QtEndian>

namespace http2 {

Connection::Connection(ConnectionType type, QTcpSocket *socket, QObject *parent) :
    QObject(parent),
    type_(type),
    socket_(socket),
    maxIdentifier_(0)
{
    if (!socket_) qWarning() << "empty socket";
    connect(socket_, &QTcpSocket::readyRead, this, &Connection::receive);
    connectionStream_ = getStream(0);

    SettingsFrame settings;
    connectionStream_->sendFrame(settings);
}

void Connection::close(ErrorCode error)
{
    GoawayFrame frame;
    frame.setErrorCode(error);
    frame.setLastIdentifier(maxIdentifier_);
    connectionStream_->sendFrame(frame);
    socket_->waitForBytesWritten();
    socket_->close();
}

Stream* Connection::getStream(int identifier)
{
    Stream* stream = nullptr;
    if (streamMap_.contains(identifier)) {
        stream = streamMap_[identifier];
    }
    else {
        if (identifier < 0) {
            if (type_ == CONNECTION_CLIENT) {
                identifier = maxIdentifier_ + 1 + (maxIdentifier_ % 2);
            }
            else {
                identifier = maxIdentifier_ + 2 - (maxIdentifier_ % 2);
            }
        }
        stream = new Stream(this, identifier, type_);
        streamMap_[identifier] = stream;
        emit streamCreated(stream);
    }
    maxIdentifier_ = qMax(identifier, maxIdentifier_);
    return stream;
}

void Connection::sendRawData(const QByteArray& data)
{
    socket_->write(data);
}

void Connection::receive()
{
    qint64 length = socket_->bytesAvailable();
    if (currentFrame_) {
        int payloadLen = currentFrame_->length();
        if (length >= payloadLen) {
            currentFrame_->setPayload(socket_->read(payloadLen));
            receiveFrame(*currentFrame_);
            currentFrame_.reset();
        }
    }
    else {
        if (length >= 8) {
            QByteArray frameHeader = socket_->read(8);
            const uchar *headerPtr = reinterpret_cast<const uchar*>(frameHeader.data());

            currentFrame_.reset(new BasicFrame);
            currentFrame_->setPayload(QByteArray(qFromBigEndian<qint16>(headerPtr), 0));
            headerPtr += sizeof(qint16);

            currentFrame_->setType(static_cast<FrameType>(*headerPtr));
            headerPtr += sizeof(uchar);

            currentFrame_->setFlags(*headerPtr);
            headerPtr += sizeof(uchar);

            currentFrame_->setIdentifier(qFromBigEndian<qint32>(headerPtr));

            int payloadLen = currentFrame_->length();
            if (payloadLen <= length) {
                currentFrame_->setPayload(socket_->read(payloadLen));
                receiveFrame(*currentFrame_);
                currentFrame_.reset();
            }
        }
    }
}

Stream *Connection::connectionStream() const
{
    return connectionStream_;
}

void Connection::receiveFrame(const BasicFrame& frame)
{
    switch (frame.type()) {
    case FRAME_SETTINGS:
    {
        SettingsFrame in(frame);
        qDebug() << "<<RECV>>\n" << in.inspect();

        if (in.contains(SETTINGS_HEADER_TABLE_SIZE)) {
            for (Stream *stream : streamMap_) {
                stream->setHeaderTableSize(in[SETTINGS_HEADER_TABLE_SIZE]);
            }
        }

        if (!in.ack()) {
            SettingsFrame out;
            out.setAck(true);
            connectionStream_->sendFrame(out);
        }
    }
        return;
    case FRAME_PING:
    {
        PingFrame in(frame);
        qDebug() << "<<RECV>>\n" << in.inspect();

        if (!in.ack()) {
            PingFrame out;
            out.setAck(true);
            out.setPayload(in.payload());
            connectionStream_->sendFrame(out);
        }
    }
        return;
    case FRAME_GOAWAY:
    {
        GoawayFrame in(frame);
        qDebug() << "<<RECV>>\n" << in.inspect();
    }
        return;
    default:
        ;
    }

    Stream *stream = getStream(frame.identifier());
    stream->receiveFrame(frame);
}

}
