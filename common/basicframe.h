/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#pragma once
#include "iframe.h"

namespace http2 {

enum FrameType {
  FRAME_DATA          = 0x0,
  FRAME_HEADERS       = 0x1,
  FRAME_PRIORITY      = 0x2,
  FRAME_RST_STREAM    = 0x3,
  FRAME_SETTINGS      = 0x4,
  FRAME_PUSH_PROMISE  = 0x5,
  FRAME_PING          = 0x6,
  FRAME_GOAWAY        = 0x7,
  FRAME_WINDOW_UPDATE = 0x9
};

class BasicFrame
{
public:
    BasicFrame();
    QByteArray dump() const;
    quint16 length() const;

    FrameType type() const;
    void setType(FrameType type);

    uchar flags() const;
    void setFlags(uchar flags);

    quint32 identifier() const;
    void setIdentifier(quint32 identifier);

    QByteArray payload() const;
    void setPayload(const QByteArray &payload);

private:
    FrameType type_;
    uchar flags_;
    quint32 identifier_;
    QByteArray payload_;

};

}
