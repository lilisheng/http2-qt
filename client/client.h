/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#pragma once
#include <QObject>
#include <QTcpSocket>
#include <QMap>
#include "basicframe.h"

namespace http2 {

class Connection;
class HeadersFrame;
class DataFrame;

class Client : public QObject
{
    Q_OBJECT
public:
    struct Response {
        QMap<QString, QString> headers;
        QByteArray body;
    };

public:
    explicit Client(QObject *parent = 0);
    void connectToHost(const QString& host, quint16 port = 80);
    void close();
    void get(const QString& path);

signals:
    void connected();
    void responceReceived(Response);

private slots:
    void startConnection();
    void receiveHeader(quint32 identifier, const HeadersFrame* frame);
    void receiveData(quint32 identifier, const DataFrame* frame);

private:
    QString host_;
    QTcpSocket *socket_;
    Connection *connection_;
    QMap<quint32, Response> responses_;

};

}
