/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#pragma once
#include "basicframe.h"

namespace http2 {

class PingFrame : public IFrame
{
public:
    PingFrame();
    explicit PingFrame(const BasicFrame& frame);
    QByteArray dump(StreamContext& context) const;
    QString inspect() const;

    bool ack() const;
    void setAck(bool ack);

    quint64 payload() const;
    void setPayload(quint64 payload);

private:
    bool ack_;
    quint64 payload_;

};

}
