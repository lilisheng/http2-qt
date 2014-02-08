/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#include <QCoreApplication>
#include <QTimer>
#include "app.h"

int main(int argc, char *argv[])
{
    QCoreApplication coreapp(argc, argv);
    App app(argc, argv);
    QObject::connect(&app, &App::done, &coreapp, &QCoreApplication::quit);
    QTimer::singleShot(0, &app, SLOT(run()));
    return coreapp.exec();
}
