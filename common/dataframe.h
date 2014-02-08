/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#pragma once
#include <QMap>
#include <QObject>
#include "basicframe.h"

namespace http2 {

class DataFrame : public IFrame, public QObject
{
public:
    DataFrame(QObject *parent = 0);
    explicit DataFrame(const BasicFrame& frame, QObject *parent = 0);
    QByteArray dump(StreamContext& context) const;
    QString inspect() const;

    QByteArray payload() const;
    void setPayload(const QByteArray &payload);

    bool endStream() const;
    void setEndStream(bool endStream);

private:
    bool endStream_;
    QByteArray payload_;

};

}
