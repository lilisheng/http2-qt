#pragma once
#include <QObject>
#include <QTcpServer>
#include <QSignalMapper>
#include <QDir>

namespace http2 {

class Stream;
class HeadersFrame;

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    void start(const QString& documentDir, quint16 port = 80);

signals:

private slots:
    void newConnection();
    void readyReadSocket(QObject* object);
    void newStream(Stream* stream);
    void receiveHeader(Stream* stream, const HeadersFrame* frame);

private:
    QDir documentDir_;
    QTcpServer *tcpServer_;
    QSignalMapper *socketMapper_;

};

}
