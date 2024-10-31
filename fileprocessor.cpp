#include "fileprocessor.h"

FileProcessor::FileProcessor(QObject *parent)
{
    Q_UNUSED(parent)

    // NB: QFutureWatcher::progressValueChanged возвращает значение от 0 до 1 без промежуточного результата,
    // поэтому используем собственный метод getLineCount()
    // QFutureWatcher::progressRangeChanged в нашем случае так же будет возвращать min 0 - max 0
    // так как для вычесления прогресса необходимо использовать метод QtConcurrent::map,
    // что в нашем случае является неудачным выбором.
//    connect(&m_futureWatcher, &QFutureWatcher<void>::progressValueChanged, this);
//    connect(&m_futureWatcher, &QFutureWatcher<void>::progressRangeChanged, this);
    connect(&m_futureWatcher, &QFutureWatcher<void>::started, this, &FileProcessor::readStarted);
    connect(&m_futureWatcher, &QFutureWatcher<void>::finished, this, &FileProcessor::readFinished);
    connect(&m_futureWatcher, &QFutureWatcher<void>::canceled, this,
            [this]() {
                         emit readingCanceled();
                         disconnect(this, &FileProcessor::fileReading, this, &FileProcessor::fileProcessed);
                         emit fileProcessed(QVariantMap());
                         emit readingCanceled();
                     });
}

void FileProcessor::start() {
    if (m_filePath.isEmpty()) {
        qWarning() << "Empty path!";
        return;
    }

    if (m_futureWatcher.isStarted())
        m_futureWatcher.waitForFinished();

    if (isPausedFlag)
        togglePaused();

    connect(this, &FileProcessor::fileReading, this, &FileProcessor::fileProcessed);

    qDebug() << "Start reading";
    m_future = QtConcurrent::run(this, &FileProcessor::readFile, m_filePath);
    m_futureWatcher.setFuture(m_future);
}

void FileProcessor::togglePaused() {
    qDebug() << "Pause reading";
    isPausedFlag = !isPausedFlag;
    m_futureWatcher.togglePaused();
    emit pauseChanged(isPausedFlag);
    m_futureWatcher.waitForFinished();
}

// NB: Текущая задача не может быть немедленно завершена, но пулл задач будет отменен
void FileProcessor::cancel() {
    qDebug() << "Cancel reading";
    if (isPausedFlag)
        togglePaused();

    m_futureWatcher.cancel();
}

int FileProcessor::getLineCount() {
    int lineCount = 0;
    QFile file(QUrl(m_filePath).toLocalFile());
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            in.readLine();
            ++lineCount;
        }
        file.close();
    }
    return lineCount;
}

QVariantMap FileProcessor::getTopWords(const QVariantMap &wordCountMap, int topCount) const {
    QList<QPair<QString, int>> wordList;
    for (auto it = wordCountMap.begin(); it != wordCountMap.end(); ++it) {
        wordList.append(qMakePair(it.key(), it.value().toInt()));
    }

    std::sort(wordList.begin(), wordList.end(), [](const auto &a, const auto &b) {
        return a.second > b.second;
    });

    if (wordList.size() > topCount) {
        wordList = wordList.mid(0, topCount);
    }

    QVariantMap topWords;
    for (const auto &pair : qAsConst(wordList)) {
        topWords[pair.first] = pair.second;
    }
    return topWords;
}

QVariantMap FileProcessor::readFile(const QString &filePath) {
    QThread::currentThread()->setObjectName("File reading thread");
    qInfo() << "File reading thread started: -->" << QThread::currentThread();

    QVariantMap wordCountMap;
    QFile file(QUrl(filePath).toLocalFile());

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        int totalLines = getLineCount();
        int currentLine = 0;
        QTextStream in(&file);

        while (!in.atEnd() && !isPausedFlag) {
            QString line = in.readLine().toLower();
            ++currentLine;
            // Вычисляем процент прочитанных строк
            int progress = (static_cast<double>(currentLine) / totalLines) * 100;
            emit progressUpdated(progress);

            // Обновляем счетчик для каждого слова
            for (const QString &word : line.split(QRegularExpression("\\W+"), Qt::SkipEmptyParts)) {
                wordCountMap[word] = wordCountMap.value(word, 0).toInt() + 1;
                emit fileReading(wordCountMap);
                QThread::currentThread()->msleep(100); // Небольшая задержка для наглядности
            }
            // Отправляем топ-15 слов в UI
            emit fileReading(getTopWords(wordCountMap, 15));
        }
        file.close();
    }
    return wordCountMap;
}

const QString &FileProcessor::filePath() const
{
    return m_filePath;
}

void FileProcessor::setFilePath(const QString &newFilePath)
{
    if (m_filePath == newFilePath)
        return;
    m_filePath = newFilePath;
    emit filePathChanged();
}
