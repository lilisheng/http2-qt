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

class WindowUpdateFrame : public IFrame
{
public:
    WindowUpdateFrame();
    explicit WindowUpdateFrame(const BasicFrame& frame);
    QByteArray dump(StreamContext& context) const;
    QString inspect() const;

    quint32 windowSizeIncrement() const;
    void setWindowSizeIncrement(quint32 windowSizeIncrement);

private:
    quint32 windowSizeIncrement_;

};

}
