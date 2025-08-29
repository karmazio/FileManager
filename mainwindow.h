#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QDateTime>
#include "file_scanner.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


enum class ScanState { Idle, Running, Paused };

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateStats(int totalFiles,
                     double totalSizeMB,
                     const QString &largestFileName,
                     double largestFileSizeMB,
                     const QString &mostCommonExt);
    ScanState scanState = ScanState::Idle;

private slots:
    void onSelectFolder();
    void onStartScan();
    void onStopScan();
    void onPauseScan();
    void onResumeScan();
    void onExport();

    // Slots for updating UI (called from scanner signals)
    void onProgressUpdated(int percent, const QString &currentFile, const FileStats &stats);
    void onFileFound(const FileItem &file);
    void onScanFinished(bool completed, const QList<FileItem> &allFiles, const FileStats &stats);
    void onScanError(const QString &error);

private:
    Ui::MainWindow *ui;
    QString selectedFolder;

    FileScanner *scanner = nullptr;
    QThread *scannerThread = nullptr;

    int totalFiles = 0;
    double totalSizeMB = 0.0;
    QString largestFileName;
    double largestFileSizeMB = 0.0;
    QMap<QString,int> extCount;

};

#endif // MAINWINDOW_H
