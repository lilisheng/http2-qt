/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#include "windowupdateframe.h"
#include "streamcontext.h"
#include <QDebug>
#include <QtEndian>
#include <QJsonObject>
#include <QJsonDocument>

namespace http2 {

enum PingFlags {
    SETTINGS_FLAG_ACK = 0x01
};

WindowUpdateFrame::WindowUpdateFrame() :
    windowSizeIncrement_(0)
{
}

WindowUpdateFrame::WindowUpdateFrame(const BasicFrame& frame)
{
    if (frame.identifier() != 0) {
        // ERR
    }
    if (frame.length() == sizeof(windowSizeIncrement_)) {
        const QByteArray& payload = frame.payload();
        windowSizeIncrement_ = qFromBigEndian<quint32>(reinterpret_cast<const uchar*>(payload.data()));
    }
    else {
        // ERR
    }
}

QByteArray WindowUpdateFrame::dump(StreamContext& context) const
{
    QByteArray payload(sizeof(windowSizeIncrement_), 0);
    qToBigEndian(windowSizeIncrement_, reinterpret_cast<uchar*>(payload.data()));

    BasicFrame frame;
    frame.setType(FRAME_WINDOW_UPDATE);
    frame.setPayload(payload);
    frame.setIdentifier(context.identifier);
    return frame.dump();
}

QString WindowUpdateFrame::inspect() const
{
    QJsonObject obj;
    obj["windowSizeIncrement"] = static_cast<int>(windowSizeIncrement_);
    return "WINDOW_UPDATE " + QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Indented));
}

quint32 WindowUpdateFrame::windowSizeIncrement() const
{
    return windowSizeIncrement_;
}

void WindowUpdateFrame::setWindowSizeIncrement(quint32 windowSizeIncrement)
{
    windowSizeIncrement_ = windowSizeIncrement;
}


}
