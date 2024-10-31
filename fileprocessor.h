#ifndef FILEPROCESSOR_H
#define FILEPROCESSOR_H

#include <QObject>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QThreadPool>
#include <QThread>
#include <QFuture>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

class FileProcessor : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)

public:
    explicit FileProcessor(QObject *parent = nullptr);

    const QString &filePath() const;
    void setFilePath(const QString &newFilePath);

    QVariantMap readFile(const QString &filePath);

public slots:
    void start();
    void togglePaused();
    void cancel();

signals:
    void filePathChanged();
    void fileProcessed(const QVariantMap &wordCountMap);
    void fileReading(const QVariantMap &wordCountMap);
    void pauseChanged(bool isPause);
    void readStarted();
    void readFinished();
    void readingCanceled();
    void progressUpdated(int progress);

private:
    QVariantMap getTopWords(const QVariantMap &wordCountMap, int topCount) const;
    int getLineCount();

    QFuture<QVariantMap> m_future;
    QFutureWatcher<void> m_futureWatcher;
    QString m_filePath;
    bool isPausedFlag = false;
};

#endif // FILEPROCESSOR_H
