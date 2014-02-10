/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#pragma once
#include <QObject>
#include <QSharedPointer>
#include "streamcontext.h"
#include "iframe.h"
#include "common.h"

namespace http2 {

class Connection;
class BasicFrame;
class HeadersFrame;
class DataFrame;

class Stream : public QObject
{
    Q_OBJECT
public:
    explicit Stream(Connection *connection, quint32 identifier, ConnectionType type);
    int windowSize() const;
    void setWindowSize(int windowSize);

signals:
    void headerReceived(quint32 identifier, const HeadersFrame* frame);
    void dataReceived(quint32 identifier, const DataFrame* frame);

public slots:
    void sendFrame(const IFrame& frame);
    void receiveFrame(const BasicFrame& frame);
    void setHeaderTableSize(int headerTableSize);

private:
    Connection *connection_;
    StreamContext context_;
    int windowSize_;
    int windowConsumed_;

};

}
