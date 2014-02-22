/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#include "client.h"
#include "connection.h"
#include "settingsframe.h"
#include "headersframe.h"
#include "dataframe.h"
#include "stream.h"

namespace http2 {

Client::Client(QObject *parent) :
    QObject(parent),
    socket_(new QTcpSocket(this)),
    connection_(nullptr)
{

}

void Client::connectToHost(const QString& host, quint16 port)
{
    host_ = host;
    connect(socket_, &QTcpSocket::connected, this, &Client::startConnection);
    socket_->connectToHost(host, port);
    socket_->waitForConnected();
}

void Client::close()
{
    connection_->close(NO_ERROR);
}

void Client::startConnection()
{
    socket_->write("PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n");
    connection_ = new Connection(CONNECTION_CLIENT, socket_, this);
    emit connected();
}

void Client::get(const QString& path)
{
    HeadersFrame frame;
    frame.setEndStream(true);
    frame[":authority"] = QString("%1:%2").arg(host_).arg(socket_->peerPort());
    frame[":method"] = "GET";
    frame[":path"]   = path;
    frame[":scheme"] = "http";

    Stream *stream = connection_->getStream();
    connect(stream, &Stream::headerReceived, this, &Client::receiveHeader);
    connect(stream, &Stream::dataReceived, this, &Client::receiveData);

    stream->sendFrame(frame);
}

void Client::receiveHeader(Stream *stream, const HeadersFrame* frame)
{
    responses_[stream->identifier()].headers = frame->headers();
}

void Client::receiveData(Stream *stream, const DataFrame* frame)
{
    quint32 identifier = stream->identifier();
    if (responses_.contains(identifier)) {
        Response& res = responses_[identifier];
        res.body += frame->payload();
        if (res.body.size() == res.headers["content-length"].toInt()) {
            emit responceReceived(res);
        }
    }
}

}
