#include "file_scanner.h"
#include <QDirIterator>
#include <QThread>
#include <QCoreApplication>


FileScanner::FileScanner(QObject *parent)
    : QObject(parent) {}

void FileScanner::reset() {
    m_shouldStop.storeRelease(0);
    {
        QMutexLocker lk(&m_pauseMutex);
        m_paused.storeRelease(0);
        m_pauseCond.wakeAll();
    }
    m_filesProcessed = 0;
    m_totalSize = 0;
    m_foundFiles.clear();
    m_largestFileName.clear();
    m_largestFileSizeMB = 0.0;
    m_extCount.clear();
}

void FileScanner::scanDirectory(const QString &path) {
    QDir dir(path);
    if (!dir.exists()) {
        emit error(QString("Directory does not exist: %1").arg(path));
        return;
    }
    m_timer.start();
    m_elapsedPaused = 0;
    m_estimatedTotalTime = 10000;
    reset();

    QList<FileItem> batchFiles;
    const int batchSize = 20;

    QDirIterator it(dir.absolutePath(),
                    QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);

    while (it.hasNext()) {
        if (m_shouldStop.loadAcquire()) {
            emit finished(false,{}, {});
            break;
        }


        QFileInfo info(it.next());
        if (!info.isFile()) continue;

        // --- Pause handling ---
        {
            QMutexLocker lk(&m_pauseMutex);
            while (m_paused.loadAcquire() && !m_shouldStop.loadAcquire()) {
                m_pauseCond.wait(&m_pauseMutex);
            }
        }

        FileItem item = createFileItem(info);
        m_foundFiles.push_back(item);
        batchFiles.push_back(item);

        // --- Update stats ---
        ++m_filesProcessed;
        m_totalSize += item.size();

        double sizeMB = item.size() / (1024.0 * 1024.0);
        if (sizeMB > m_largestFileSizeMB) {
            m_largestFileSizeMB = sizeMB;
            m_largestFileName = item.name();
        }

        QString ext = item.type().toLower();
        if (!ext.isEmpty() && ext != "(no ext)") m_extCount[ext]++;

        // --- Emit batch ---
        if (batchFiles.size() >= batchSize) {
            emitBatch(batchFiles);
        }

        // Give control back to the event loop
        if ((m_filesProcessed % 100) == 0) {
            QThread::msleep(1);
            QCoreApplication::processEvents();
        }
    }

    if (m_shouldStop) {
        return;
    }

    // --- Emit remaining batch ---
    if (!batchFiles.isEmpty()) emitBatch(batchFiles);

    // --- Final stats ---
    FileStats stats;
    stats.totalFiles = m_filesProcessed;
    stats.totalSizeMB = static_cast<double>(m_totalSize) / (1024.0 * 1024.0);
    stats.largestFileName = m_largestFileName;
    stats.largestFileSizeMB = m_largestFileSizeMB;

    int maxCount = 0;
    for (auto it = m_extCount.begin(); it != m_extCount.end(); ++it) {
        if (it.value() > maxCount) {
            maxCount = it.value();
            stats.mostCommonExt = it.key();
        }
    }

    emit finished(true, m_foundFiles, stats);
}

void FileScanner::stopScanning() {
    m_shouldStop.storeRelease(1);
}

void FileScanner::pauseScanning() {
    QMutexLocker lk(&m_pauseMutex);
    if (!m_paused) {
        m_paused = true;
        m_elapsedPaused += m_timer.elapsed(); // accumulating time since pause
    }
}

void FileScanner::resumeScanning() {
    QMutexLocker lk(&m_pauseMutex);
    m_paused = false;
    m_timer.restart(); // restart timer for next scan session
    m_pauseCond.wakeAll();
}

FileItem FileScanner::createFileItem(const QFileInfo &info) {
    const QString ext = info.suffix().toLower();
    const QString type = ext.isEmpty() ? "(no ext)" : "." + ext;
    return FileItem(info.fileName(), info.absolutePath(), info.size(), info.lastModified(), type);
}

void FileScanner::emitBatch(QList<FileItem> &batchFiles) {
    if (batchFiles.isEmpty()) return;
    if (m_shouldStop) {
        emit finished(false, {}, {});
    }
    FileStats stats;
    stats.totalFiles = m_filesProcessed;
    stats.totalSizeMB = static_cast<double>(m_totalSize) / (1024.0*1024.0);
    stats.largestFileName = m_largestFileName;
    stats.largestFileSizeMB = m_largestFileSizeMB;

    // Determine most common extension
    int maxCount = 0;
    for (auto it = m_extCount.begin(); it != m_extCount.end(); ++it) {
        if (it.value() > maxCount) {
            maxCount = it.value();
            stats.mostCommonExt = it.key();
        }
    }


    int percent = static_cast<int>(100.0 * (0.2 * calculateProgressByFiles() + 0.8 * calculateProgressByTime()));

    emit progress(percent, batchFiles.last().fullPath(), stats);

    for (const FileItem &f : batchFiles) emit fileFound(f);
    batchFiles.clear();
}

double FileScanner::calculateProgressByFiles() const {
    if (m_estimatedTotalFiles <= 0) return 0.0;
    double p = static_cast<double>(m_filesProcessed) / m_estimatedTotalFiles;
    return qBound(0.0, p, 1.0);
}

double FileScanner::calculateProgressByTime() const {
    qint64 elapsed = m_paused ? m_elapsedPaused : m_elapsedPaused + m_timer.elapsed();
    if (m_estimatedTotalTime <= 0) return 0.0;
    double p = static_cast<double>(elapsed) / m_estimatedTotalTime;
    return qBound(0.0, p, 1.0);
}

