/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#pragma once
#include <QObject>
#include <QScopedPointer>
#include "hpack.h"
#include "basicframe.h"
#include "common.h"
#include "errorcode.h"

class QTcpSocket;

namespace http2 {

class Stream;
class BasicFrame;

class Connection : public QObject
{
    Q_OBJECT
public:
    Connection(ConnectionType type, QTcpSocket *socket, QObject *parent = 0);
    void close(ErrorCode error);
    Stream* getStream(int identifier = -1);
    Stream *connectionStream() const;

public slots:
    void sendRawData(const QByteArray& data);

signals:
    void streamCreated(Stream*);

private:
    void receiveFrame(const BasicFrame& frame);

private slots:
    void receive();

private:
    ConnectionType type_;
    QTcpSocket *socket_;
    QScopedPointer<BasicFrame> currentFrame_;
    QMap<quint32, Stream*> streamMap_;
    Stream *connectionStream_;
    int maxIdentifier_;

};

}
