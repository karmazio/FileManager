#pragma once
#include <QObject>
#include <QDir>
#include <QAtomicInt>
#include <QList>
#include <QMap>
#include <QMutex>
#include <QWaitCondition>
#include "file_item.h"

#ifndef FILE_SCANNER_H
#define FILE_SCANNER_H

struct FileStats {
    int totalFiles = 0;
    double totalSizeMB = 0.0;
    QString largestFileName;
    double largestFileSizeMB = 0.0;
    QString mostCommonExt;
};

Q_DECLARE_METATYPE(FileStats)
Q_DECLARE_METATYPE(FileItem)
Q_DECLARE_METATYPE(QList<FileItem>)

class FileScanner : public QObject {
    Q_OBJECT
public:
    explicit FileScanner(QObject *parent = nullptr);

    void reset();
    FileStats scanDirectory(const QString &path);

public slots:
    void stopScanning();
    void pauseScanning();
    void resumeScanning();

signals:
    void fileFound(const FileItem &file);
    void progress(int percent, const QString &currentFile, const FileStats &stats);
    void finished(const QList<FileItem> &allFiles, const FileStats &stats);
    void error(const QString &message);

private:
    bool scanRecursive(const QDir &dir);
    FileItem createFileItem(const QFileInfo &info);
    qint64 countFiles(const QDir &dir);

    // handling
    QAtomicInt m_shouldStop;
    QAtomicInt m_paused = false;
    QMutex m_pauseMutex;
    QWaitCondition m_pauseCond;

    // accumulation
    QList<FileItem> m_foundFiles;

    // statistics
    int m_filesProcessed = 0;
    qint64 m_totalSize = 0;
    qint64 m_totalFiles = 0;
    QString m_largestFileName;
    double m_largestFileSizeMB = 0.0;
    QMap<QString,int> m_extCount;
};

#endif // FILE_SCANNER_H
