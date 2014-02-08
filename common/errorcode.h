/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#pragma once

namespace http2 {

enum ErrorCode {
    NO_ERROR           = 0,
    PROTOCOL_ERROR     = 1,
    INTERNAL_ERROR     = 2,
    FLOW_CONTROL_ERROR = 3,
    SETTINGS_TIMEOUT   = 4,
    STREAM_CLOSED      = 5,
    FRAME_SIZE_ERROR   = 6,
    REFUSED_STREAM     = 7,
    CANCEL             = 8,
    COMPRESSION_ERROR  = 9,
    CONNECT_ERROR      = 10,
    ENHANCE_YOUR_CALM  = 420
};

}
