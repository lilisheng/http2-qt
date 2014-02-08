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

enum SettingsType {
    SETTINGS_HEADER_TABLE_SIZE      = 1,
    SETTINGS_ENABLE_PUSH            = 2,
    SETTINGS_MAX_CONCURRENT_STREAMS = 4,
    SETTINGS_INITIAL_WINDOW_SIZE    = 7,
    SETTINGS_FLOW_CONTROL_OPTIONS   = 10,
};

class SettingsFrame : public IFrame
{
public:
    SettingsFrame();
    explicit SettingsFrame(const BasicFrame& frame);
    QByteArray dump(StreamContext& context) const;
    QString inspect() const;

    quint32& operator[](const SettingsType& key);
    quint32  operator[](const SettingsType& key) const;
    bool contains(const SettingsType& key) const;

    bool ack() const;
    void setAck(bool ack);

private:
    bool ack_;
    QMap<SettingsType, quint32> settings_;

};

}
