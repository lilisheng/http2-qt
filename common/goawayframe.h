/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#pragma once
#include <QMap>
#include "basicframe.h"

namespace http2 {

class GoawayFrame : public IFrame
{
public:
    GoawayFrame();
    explicit GoawayFrame(const BasicFrame& frame);
    QByteArray dump(StreamContext& context) const;
    QString inspect() const;

    quint32 lastIdentifier() const;
    void setLastIdentifier(quint32 lastIdentifier);

    quint32 errorCode() const;
    void setErrorCode(quint32 errorCode);

    QByteArray debugData() const;
    void setDebugData(const QByteArray &debugData);

private:
    quint32 lastIdentifier_;
    quint32 errorCode_;
    QByteArray debugData_;

};

}
