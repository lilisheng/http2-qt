/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#pragma once
#include "hpack.h"

namespace http2 {

struct StreamContext {
    quint32 identifier;
    HPACK hpack;
};

}
