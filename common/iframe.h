/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#pragma once
#include <QByteArray>

namespace http2 {

struct StreamContext;

class IFrame {
public:
    virtual QByteArray dump(StreamContext& context) const = 0;
    virtual QString inspect() const = 0;
    virtual ~IFrame() {}
};

}
