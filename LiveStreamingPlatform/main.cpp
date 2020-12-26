#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

#include "rtmpstreamer.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    QQuickStyle::setStyle("Material");

    qmlRegisterType<RTMPStreamer>("RTMPStreamer", 1, 0, "RTMPStreamer");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
