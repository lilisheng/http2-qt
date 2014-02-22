#include "server.h"
#include "connection.h"
#include "stream.h"
#include "headersframe.h"
#include "dataframe.h"
#include <QTcpSocket>
#include <QMetaMethod>
#include <QFile>

namespace http2 {

Server::Server(QObject *parent) :
    QObject(parent),
    tcpServer_(new QTcpServer(this)),
    socketMapper_(new QSignalMapper(this))
{
    connect(tcpServer_, &QTcpServer::newConnection, this, &Server::newConnection);
    connect(socketMapper_, SIGNAL(mapped(QObject*)), this, SLOT(readyReadSocket(QObject*)));
}

void Server::start(const QString& documentDir, quint16 port)
{
    documentDir_.setPath(documentDir);
    tcpServer_->listen(QHostAddress::Any, port);
}

void Server::newConnection()
{
    if (QTcpSocket* socket = tcpServer_->nextPendingConnection()) {
        qDebug() << "new tcp connection";
        connect(socket, SIGNAL(readyRead()), socketMapper_, SLOT(map()));
        socketMapper_->setMapping(socket, socket);
    }
}

void Server::receiveHeader(Stream* stream, const HeadersFrame* frame)
{
    QString path = documentDir_.absoluteFilePath((*frame)[":path"].mid(1));
    qDebug() << path;

    HeadersFrame header;
    header.setEndHeaders(true);
    header.setEndStream(false);

    DataFrame data;
    data.setEndStream(false);

    if (QFileInfo(path).exists()) {
        QFile file(path);
        file.open(QFile::ReadOnly);
        header[":status"] = "200";
        data.setPayload(file.readAll());
    }
    else {
        header[":status"] = "404";
        data.setPayload("404 Not Found");
    }

    header["content-type"] = "text/html";
    header["content-length"] = QString("%1").arg(data.payload().size());

    stream->sendFrame(header);
    stream->sendFrame(data);
}

void Server::newStream(Stream* stream)
{
    qDebug() << "new http2 stream";
    connect(stream, &Stream::headerReceived, this, &Server::receiveHeader);
}

void Server::readyReadSocket(QObject* object)
{
    if (QTcpSocket* socket = dynamic_cast<QTcpSocket*>(object)) {
        // Direct negotiation
        static const QByteArray startData = "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n";
        if (socket->bytesAvailable() >= startData.size()) {
            if (socket->read(startData.size()) == startData) {
                qDebug() << "new http2 connection";
                disconnect(socket, SIGNAL(readyRead()), socketMapper_, SLOT(map()));
                Connection* conn = new Connection(CONNECTION_SERVER, socket, this);
                connect(conn, &Connection::streamCreated, this, &Server::newStream);
            }
            else {
                socket->close();
            }
        }
    }
}

}
