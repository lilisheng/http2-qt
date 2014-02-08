#pragma once
#include <QMap>
#include <QSharedPointer>
#include <QObject>
/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#include "basicframe.h"

namespace http2 {

class HeadersFrame : public IFrame, public QObject
{
public:
    HeadersFrame(QObject *parent = 0);
    HeadersFrame(const BasicFrame& frame, StreamContext& context, QObject *parent = 0);
    QByteArray dump(StreamContext& context) const;
    QString inspect() const;

    const QMap<QString, QString>& headers() const;
    QString& operator[](const QString& key);
    QString  operator[](const QString& key) const;

    bool endStream() const;
    void setEndStream(bool endStream);

    bool endHeaders() const;
    void setEndHeaders(bool endHeaders);

    bool priority() const;
    void setPriority(bool priority);

private:
    bool endStream_;
    bool endHeaders_;
    bool priority_;
    QMap<QString, QString> headers_;

};

}
