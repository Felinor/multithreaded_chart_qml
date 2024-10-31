#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "fileprocessor.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    app.setApplicationName(QString("multithreaded_chart_qml"));
    app.setOrganizationName(QString("Felinor"));
    app.setOrganizationDomain(QString("https://github.com/Felinor"));

    QQmlApplicationEngine engine;

    FileProcessor *fileProcessor = new FileProcessor(&app);
    engine.rootContext()->setContextProperty("fileProcessor", fileProcessor);

    QThread::currentThread()->setObjectName("Main thread");
    qInfo() << "Main thread started: -->" << QThread::currentThread();

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
