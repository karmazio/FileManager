#include <QDirIterator>
#include <QFileInfo>
#include <QThread>
#include <QCoreApplication>
#include "file_scanner.h"

FileScanner::FileScanner(QObject *parent)
    : QObject(parent) {}

void FileScanner::reset() {
    m_shouldStop.storeRelease(0);
    {
        QMutexLocker lk(&m_pauseMutex);
        m_paused = false;
        m_pauseCond.wakeAll();
    }
    m_filesProcessed = 0;
    m_totalFiles = 0;
    m_totalSize = 0;
    m_foundFiles.clear();
    m_largestFileName.clear();
    m_largestFileSizeMB = 0.0;
    m_extCount.clear();
}

FileStats FileScanner::scanDirectory(const QString &path) {
    FileStats stats;

    QDir dir(path);
    if (!dir.exists()) {
        emit error(QString("Directory does not exist: %1").arg(path));
        return stats;
    }

    reset();
    m_totalFiles = countFiles(dir);

    scanRecursive(dir);

    // final statistics
    stats.totalFiles = m_filesProcessed;
    stats.totalSizeMB = static_cast<double>(m_totalSize) / (1024.0*1024.0);
    stats.largestFileName = m_largestFileName;
    stats.largestFileSizeMB = m_largestFileSizeMB;

    int maxCount = 0;
    for (auto it = m_extCount.begin(); it != m_extCount.end(); ++it) {
        if (it.value() > maxCount) {
            maxCount = it.value();
            stats.mostCommonExt = it.key();
        }
    }

    emit finished(m_foundFiles, stats);
    return stats;
}

void FileScanner::stopScanning() {
    m_shouldStop.storeRelease(1);
    resumeScanning();
}

void FileScanner::pauseScanning() {
    QMutexLocker lk(&m_pauseMutex);
    m_paused = true;
}

void FileScanner::resumeScanning() {
    QMutexLocker lk(&m_pauseMutex);
    m_paused = false;
    m_pauseCond.wakeAll();
}

bool FileScanner::scanRecursive(const QDir &dir) {
    QDirIterator it(dir.absolutePath(),
                    QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);

    while (it.hasNext()) {
        if (m_shouldStop.loadAcquire()) return false;

        QFileInfo info(it.next());
        if (!info.isFile()) continue;

        // пауза
        while (m_paused.loadAcquire() && !m_shouldStop.loadAcquire()) {
            QThread::msleep(50);
            QCoreApplication::processEvents();
        }

        if (m_shouldStop.loadAcquire()) return false;

        FileItem item = createFileItem(info);
        m_foundFiles.push_back(item);

        // update stats
        ++m_filesProcessed;
        m_totalSize += item.size();

        double sizeMB = item.size() / (1024.0 * 1024.0);
        if (sizeMB > m_largestFileSizeMB) {
            m_largestFileSizeMB = sizeMB;
            m_largestFileName = item.name();
        }

        QString ext = item.type().toLower();
        if (!ext.isEmpty() && ext != "(no ext)")
            m_extCount[ext]++;

        // intermediate stats
        FileStats stats;
        stats.totalFiles = m_filesProcessed;
        stats.totalSizeMB = static_cast<double>(m_totalSize) / (1024.0 * 1024.0);
        stats.largestFileName = m_largestFileName;
        stats.largestFileSizeMB = m_largestFileSizeMB;

        int maxCount = 0;
        for (QMap<QString,int>::const_iterator itExt = m_extCount.constBegin(); itExt != m_extCount.constEnd(); ++itExt) {
            if (itExt.value() > maxCount) {
                maxCount = itExt.value();
                stats.mostCommonExt = itExt.key();
            }
        }

        int percent = (m_totalFiles > 0) ? static_cast<int>((m_filesProcessed * 100) / m_totalFiles) : 0;

        emit progress(percent, item.fullPath(), stats);
        emit fileFound(item);

        if ((m_filesProcessed % 100) == 0)
            QThread::msleep(1);
    }
    return true;
}

FileItem FileScanner::createFileItem(const QFileInfo &info) {
    const QString ext = info.suffix().toLower();
    const QString type = ext.isEmpty() ? "(no ext)" : "." + ext;
    return FileItem(info.fileName(), info.absolutePath(), info.size(), info.lastModified(), type);
}

qint64 FileScanner::countFiles(const QDir &dir) {
    qint64 total = 0;
    const QFileInfoList entries =
        dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo &fi : entries) {
        if (fi.isDir()) total += countFiles(QDir(fi.absoluteFilePath()));
        else ++total;
    }
    return total;
}
