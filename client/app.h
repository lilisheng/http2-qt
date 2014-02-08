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
#include "client.h"

class App : public QObject {
    Q_OBJECT

public:
    App(int argc, char *argv[])
    {
        if (argc == 2) {
            url_ = QUrl(argv[1]);
        }
    }

    ~App()
    {
        client_.close();
    }

public slots:
    void run()
    {
        if (url_.isValid()) {
            connect(&client_, &http2::Client::responceReceived, this, &App::received);
            connect(&client_, &http2::Client::responceReceived, this, &App::done);
            client_.connectToHost(url_.host(), url_.port(80));
            client_.get("/");
        }
        else {
            QTextStream(stdout) << "please specify URL\n";
            emit done();
        };
    }

private slots:
    void received(const http2::Client::Response& response)
    {
        QTextStream(stdout) << response.body << "\n";
    }

signals:
    void done();

private:
    http2::Client client_;
    QUrl url_;

};
