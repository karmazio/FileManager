#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStyledItemDelegate>
#include <QThread>
#include <QMetaType>
#include <QFileIconProvider>

class FileSizeItem : public QTableWidgetItem {
public:
    FileSizeItem(const QString &text, qulonglong sizeBytes)
        : QTableWidgetItem(text), m_size(sizeBytes) {}
    bool operator<(const QTableWidgetItem &other) const override {
        const auto *o = dynamic_cast<const FileSizeItem*>(&other);
        return o ? (m_size < o->m_size) : QTableWidgetItem::operator<(other);
    }
private:
    qulonglong m_size;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qRegisterMetaType<FileItem>("FileItem");
    qRegisterMetaType<QList<FileItem>>("QList<FileItem>");
    qRegisterMetaType<FileStats>("FileStats");

    ui->tableFiles->setColumnCount(4);
    ui->tableFiles->setHorizontalHeaderLabels({"Name", "Size", "Type", "Date"});
    ui->tableFiles->horizontalHeader()->setStretchLastSection(true);
    ui->tableFiles->setSortingEnabled(true);

    connect(ui->btnSelectFolder, &QPushButton::clicked, this, &MainWindow::onSelectFolder);
    connect(ui->btnScan, &QPushButton::clicked, this, &MainWindow::onStartScan);
    connect(ui->btnStop, &QPushButton::clicked, this, &MainWindow::onStopScan);
    connect(ui->btnExport, &QPushButton::clicked, this, &MainWindow::onExport);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::onSelectFolder() {
    selectedFolder = QFileDialog::getExistingDirectory(this, "Select Folder");
    if (!selectedFolder.isEmpty())
        ui->lblStatus->setText("Selected: " + selectedFolder);
}

void MainWindow::onStartScan() {
    if (selectedFolder.isEmpty()) {
        QMessageBox::warning(this, "No folder", "Please select a folder first!");
        return;
    }


    if(scanState == ScanState::Running) {
        onPauseScan();
        return;
    }

    if(scanState == ScanState::Paused) {
        onResumeScan();
        return;
    }

    // new launch
    ui->btnStop->setEnabled(true);

    ui->lblStatus->setText("Scanning...");
    ui->progressBar->setValue(0);
    ui->tableFiles->setRowCount(0);


    scanState = ScanState::Running;
    ui->btnScan->setText("Pause");
    ui->btnScan->setIcon(QIcon(":/icons/pause.png"));

    // -- Preventing forced spread of the window on long file paths --
    ui->lblProgress->setMinimumWidth(300);
    ui->lblProgress->setMaximumWidth(this->width());
    ui->lblProgress->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->lblProgress->setTextInteractionFlags(Qt::TextSelectableByMouse);

    ui->lblTotalFiles->setMinimumWidth(300);
    ui->lblTotalFiles->setMaximumWidth(this->width());
    ui->lblTotalFiles->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->lblTotalFiles->setTextInteractionFlags(Qt::TextSelectableByMouse);

    ui->lblTotalSize->setMinimumWidth(300);
    ui->lblTotalSize->setMaximumWidth(this->width());
    ui->lblTotalSize->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->lblTotalSize->setTextInteractionFlags(Qt::TextSelectableByMouse);

    ui->lblLargestFile->setMinimumWidth(300);
    ui->lblLargestFile->setMaximumWidth(this->width());
    ui->lblLargestFile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->lblLargestFile->setTextInteractionFlags(Qt::TextSelectableByMouse);

    ui->lblMostCommon->setMinimumWidth(300);
    ui->lblMostCommon->setMaximumWidth(this->width());
    ui->lblMostCommon->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->lblMostCommon->setTextInteractionFlags(Qt::TextSelectableByMouse);

    // ----

    scanner = new FileScanner();
    scannerThread = new QThread();
    scanner->moveToThread(scannerThread);

    connect(scannerThread, &QThread::started, [=]() {
        scanner->scanDirectory(selectedFolder);
    });

    connect(scanner, &FileScanner::progress, this, &MainWindow::onProgressUpdated);
    connect(scanner, &FileScanner::fileFound, this, &MainWindow::onFileFound);
    connect(scanner, &FileScanner::finished, this, &MainWindow::onScanFinished);
    connect(scanner, &FileScanner::error, this, &MainWindow::onScanError);

    connect(scanner, &FileScanner::finished, scannerThread, &QThread::quit);
    connect(scannerThread, &QThread::finished, scanner, &QObject::deleteLater);
    connect(scannerThread, &QThread::finished, scannerThread, &QObject::deleteLater);
    connect(scannerThread, &QThread::finished, this, [=](){
        scanner = nullptr;
        scannerThread = nullptr;
    });

    scannerThread->start();
}

void MainWindow::onStopScan() {
    if (!scanner) return;

    if (scanState == ScanState::Running || scanState == ScanState::Paused) {
        scanner->stopScanning();
        ui->btnScan->setText("Scan");
        ui->btnScan->setIcon(QIcon(":/icons/search.png"));

        ui->btnStop->setEnabled(false);
        ui->lblStatus->setText("Scan stopped.");

        ui->progressBar->setValue(0);
        ui->tableFiles->setRowCount(0);

        ui->lblProgress->setText("Please select a new folder");

        updateStats(0, 0.0, "", 0.0, "");

    }
}

void MainWindow::onPauseScan() {
    if (scanner && scanState == ScanState::Running) {
        scanner->pauseScanning();
        scanState = ScanState::Paused;
        ui->btnScan->setText("Resume");
        ui->btnScan->setIcon(QIcon(":/icons/resume.png"));

        ui->lblStatus->setText("Scan paused");
    }
}

void MainWindow::onResumeScan() {
    if (scanner && scanState == ScanState::Paused) {
        scanner->resumeScanning();
        scanState = ScanState::Running;
        ui->btnScan->setText("Pause");
        ui->btnScan->setIcon(QIcon(":/icons/pause.png"));

        ui->lblStatus->setText("Scan resumed");
    }
}

void MainWindow::onScanFinished(bool completed, const QList<FileItem> &allFiles, const FileStats &stats) {
    // Completed or terminated by the user
    scanState = ScanState::Idle;

    ui->btnScan->setText("Scan");
    ui->btnScan->setIcon(QIcon(":/icons/search.png"));
    ui->btnStop->setEnabled(false);
    ui->lblProgress->setText("Please select a new folder");

    if (completed) {
        ui->lblStatus->setText("✅ Scan finished");
        ui->progressBar->setValue(100);
        updateStats(stats.totalFiles, stats.totalSizeMB, stats.largestFileName, stats.largestFileSizeMB, stats.mostCommonExt);
    } else {
        ui->lblStatus->setText("❌ Scan cancelled");
        ui->progressBar->setValue(0);
        ui->tableFiles->setRowCount(0);
        updateStats(0, 0.0, "", 0.0, "");
    }

    //
    scanner = nullptr;
    scannerThread = nullptr;

}

void MainWindow::onScanError(const QString &error) {
    QMessageBox::critical(this, "Scan Error", error);

    // Update scan state
    scanState = ScanState::Idle;
    ui->lblStatus->setText("Scan failed");
    ui->btnScan->setText("Scan");
    ui->btnScan->setIcon(QIcon(":/icons/search.png"));

    ui->progressBar->setValue(0);
    ui->tableFiles->setRowCount(0);

    ui->btnStop->setEnabled(false);
}

void MainWindow::onExport() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export CSV", "", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Error", "Cannot write file");
        return;
    }

    QTextStream out(&file);
    for (int row = 0; row < ui->tableFiles->rowCount(); ++row) {
        QStringList line;
        for (int col = 0; col < ui->tableFiles->columnCount(); ++col) {
            line << ui->tableFiles->item(row, col)->text();
        }
        out << line.join(",") << "\n";
    }
    file.close();
    ui->lblStatus->setText("Exported: " + fileName);
}

void MainWindow::onProgressUpdated(int percent, const QString &currentFile, const FileStats &stats) {
    ui->progressBar->setValue(percent);
    ui->lblProgress->setText(QString("Progress: %1% (Current file: %2)").arg(percent).arg(currentFile));

    updateStats(stats.totalFiles,
                stats.totalSizeMB,
                stats.largestFileName,
                stats.largestFileSizeMB,
                stats.mostCommonExt);
}

void MainWindow::onFileFound(const FileItem &file) {
    const int row = ui->tableFiles->rowCount();
    ui->tableFiles->insertRow(row);

    QFileInfo fileInfo(file.fullPath());
    QFileIconProvider iconProvider;
    QIcon icon = iconProvider.icon(fileInfo);

    // Column 0: icon + filename
    QTableWidgetItem *nameItem = new QTableWidgetItem(file.name());
    nameItem->setIcon(icon);
    nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
    ui->tableFiles->setItem(row, 0, nameItem);
    ui->tableFiles->setItem(row, 1, new FileSizeItem(file.sizeString(), file.size()));
    ui->tableFiles->setItem(row, 2, new QTableWidgetItem(file.type()));

    auto *dateItem = new QTableWidgetItem(file.modifiedString());
    dateItem->setData(Qt::UserRole, file.modified().toMSecsSinceEpoch());
    ui->tableFiles->setItem(row, 3, dateItem);
}



void MainWindow::updateStats(int totalFiles,
                             double totalSizeMB,
                             const QString &largestFileName,
                             double largestFileSizeMB,
                             const QString &mostCommonExt)
{
    ui->lblTotalFiles->setText(QString("Files: %1").arg(totalFiles));
    ui->lblTotalSize->setText(QString("Total size: %1 MB").arg(totalSizeMB, 0, 'f', 2));
    ui->lblLargestFile->setText(QString("The largest file: %1 (%2 MB)")
                                    .arg(largestFileName)
                                    .arg(largestFileSizeMB, 0, 'f', 2));
    ui->lblMostCommon->setText(QString("The most common type: %1").arg(mostCommonExt));
}

