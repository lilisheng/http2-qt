/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#pragma once
#include <QTextStream>
#include <QUrl>
#include "server.h"

class App : public QObject {
    Q_OBJECT

public:
    App(int argc, char *argv[])
    {
        if (argc >= 2) documentDir_ = argv[1];
        if (argc >= 3) port_ = QString(argv[2]).toInt();
    }

    ~App()
    {

    }

public slots:
    void run()
    {
        if (!documentDir_.isEmpty()) {
            server_.start(documentDir_, port_);
        }
        else {
            QTextStream(stdout) << "usage: http2s [documentDir] [port]\n";
            emit done();
        };
    }

signals:
    void done();

private:
    http2::Server server_;
    QString documentDir_;
    quint16 port_;
    
};
