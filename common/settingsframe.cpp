/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#include "settingsframe.h"
#include "streamcontext.h"
#include <QtEndian>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

namespace http2 {

enum SettingsFlags {
    SETTINGS_FLAG_ACK = 0x01
};

SettingsFrame::SettingsFrame() :
    ack_(0)
{
}

SettingsFrame::SettingsFrame(const BasicFrame& frame)
{
    int length = frame.length();
    ack_ = frame.flags() & SETTINGS_FLAG_ACK;

    if (frame.identifier() != 0) {
        // TODO: ERR
    }

    if (ack_ && length > 0) {
        // TODO: ERR
    }

    const QByteArray& payload = frame.payload();
    const uchar* payloadPtr = reinterpret_cast<const uchar*>(payload.data());

    if (length % 8 == 0) {
        for (int i = 0; i < length / 8; ++i) {
            SettingsType type = static_cast<SettingsType>(qFromBigEndian<quint32>(payloadPtr));
            settings_[type] = qFromBigEndian<quint32>(payloadPtr + sizeof(qint32));
            payloadPtr += sizeof(quint32) * 2;
        }
    }
    else {
        qWarning() << "invalid SettingsFrame payload";
    }
}

QByteArray SettingsFrame::dump(StreamContext& context) const
{
    if (context.identifier != 0) {
        qWarning() << "SETTINGS Frame identifier must be 0";
    }

    QByteArray payload;

    if (!ack_) {
        payload.resize(settings_.size() * 8);
        uchar* payloadPtr = reinterpret_cast<uchar*>(payload.data());

        for(SettingsType key: settings_.keys()) {
            quint32 value = settings_[key];
            qToBigEndian(static_cast<quint32>(key), payloadPtr);
            payloadPtr += sizeof(quint32);
            qToBigEndian(value, payloadPtr);
            payloadPtr += sizeof(quint32);
        }
    }

    uchar flags = 0;
    if (ack_) flags |= SETTINGS_FLAG_ACK;

    BasicFrame frame;
    frame.setType(FRAME_SETTINGS);
    frame.setFlags(flags);
    frame.setPayload(payload);
    frame.setIdentifier(0);
    return frame.dump();
}

QString SettingsFrame::inspect() const
{
    QJsonObject settingsObj;
    for(SettingsType key: settings_.keys()) {
        QString keyStr;
        int value = settings_[key];
        switch (key) {
        case SETTINGS_HEADER_TABLE_SIZE:
            keyStr = "HEADER_TABLE_SIZE";
            break;
        case SETTINGS_ENABLE_PUSH:
            keyStr = "ENABLE_PUSH";
            break;
        case SETTINGS_MAX_CONCURRENT_STREAMS:
            keyStr = "MAX_CONCURRENT_STREAMS";
            break;
        case SETTINGS_INITIAL_WINDOW_SIZE:
            keyStr = "INITIAL_WINDOW_SIZE";
            break;
        case SETTINGS_FLOW_CONTROL_OPTIONS:
            keyStr = "FLOW_CONTROL_OPTIONS";
            break;
        }
        settingsObj[keyStr] = value;
    }
    QJsonObject obj;
    obj["ack"] = ack();
    obj["settings"] = settingsObj;

    return "SETTINGS " + QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Indented));
}

quint32& SettingsFrame::operator[](const SettingsType& key)
{
    return settings_[key];
}

quint32  SettingsFrame::operator[](const SettingsType& key) const
{
    return settings_[key];
}

bool SettingsFrame::contains(const SettingsType& key) const
{
    return settings_.contains(key);
}

bool SettingsFrame::ack() const
{
    return ack_;
}

void SettingsFrame::setAck(bool ack)
{
    ack_ = ack;
}

}
