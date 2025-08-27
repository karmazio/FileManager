/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *buttonLayout;
    QPushButton *btnSelectFolder;
    QPushButton *btnScan;
    QPushButton *btnStop;
    QPushButton *btnExport;
    QProgressBar *progressBar;
    QLabel *lblProgress;
    QTableWidget *tableFiles;
    QGroupBox *groupStats;
    QVBoxLayout *verticalStats;
    QLabel *lblTotalFiles;
    QLabel *lblTotalSize;
    QLabel *lblLargestFile;
    QLabel *lblMostCommon;
    QLabel *lblStatus;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(900, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        buttonLayout = new QHBoxLayout();
        buttonLayout->setObjectName("buttonLayout");
        btnSelectFolder = new QPushButton(centralwidget);
        btnSelectFolder->setObjectName("btnSelectFolder");

        buttonLayout->addWidget(btnSelectFolder);

        btnScan = new QPushButton(centralwidget);
        btnScan->setObjectName("btnScan");

        buttonLayout->addWidget(btnScan);

        btnStop = new QPushButton(centralwidget);
        btnStop->setObjectName("btnStop");

        buttonLayout->addWidget(btnStop);

        btnExport = new QPushButton(centralwidget);
        btnExport->setObjectName("btnExport");

        buttonLayout->addWidget(btnExport);


        verticalLayout->addLayout(buttonLayout);

        progressBar = new QProgressBar(centralwidget);
        progressBar->setObjectName("progressBar");
        progressBar->setValue(0);

        verticalLayout->addWidget(progressBar);

        lblProgress = new QLabel(centralwidget);
        lblProgress->setObjectName("lblProgress");

        verticalLayout->addWidget(lblProgress);

        tableFiles = new QTableWidget(centralwidget);
        if (tableFiles->columnCount() < 4)
            tableFiles->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableFiles->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableFiles->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableFiles->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableFiles->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        tableFiles->setObjectName("tableFiles");

        verticalLayout->addWidget(tableFiles);

        groupStats = new QGroupBox(centralwidget);
        groupStats->setObjectName("groupStats");
        verticalStats = new QVBoxLayout(groupStats);
        verticalStats->setObjectName("verticalStats");
        lblTotalFiles = new QLabel(groupStats);
        lblTotalFiles->setObjectName("lblTotalFiles");

        verticalStats->addWidget(lblTotalFiles);

        lblTotalSize = new QLabel(groupStats);
        lblTotalSize->setObjectName("lblTotalSize");

        verticalStats->addWidget(lblTotalSize);

        lblLargestFile = new QLabel(groupStats);
        lblLargestFile->setObjectName("lblLargestFile");

        verticalStats->addWidget(lblLargestFile);

        lblMostCommon = new QLabel(groupStats);
        lblMostCommon->setObjectName("lblMostCommon");

        verticalStats->addWidget(lblMostCommon);


        verticalLayout->addWidget(groupStats);

        lblStatus = new QLabel(centralwidget);
        lblStatus->setObjectName("lblStatus");

        verticalLayout->addWidget(lblStatus);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        // Setting up the icons
        btnSelectFolder->setIcon(QIcon(":/icons/folder.png"));
        btnSelectFolder->setIconSize(QSize(16,16));

        btnScan->setIcon(QIcon(":/icons/search.png"));
        btnScan->setIconSize(QSize(16,16));

        btnStop->setIcon(QIcon(":/icons/stop.png"));
        btnStop->setIconSize(QSize(16,16));

        btnExport->setIcon(QIcon(":/icons/export.png"));
        btnExport->setIconSize(QSize(16,16));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "File scanner", nullptr));
        btnSelectFolder->setText(QCoreApplication::translate("MainWindow", "Select folder", nullptr));
        btnScan->setText(QCoreApplication::translate("MainWindow", "Scan", nullptr));
        btnStop->setText(QCoreApplication::translate("MainWindow", "Stop", nullptr));
        btnExport->setText(QCoreApplication::translate("MainWindow", "Export", nullptr));
        lblProgress->setText(QCoreApplication::translate("MainWindow", "Please select a folder", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableFiles->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "Name", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableFiles->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "Size", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableFiles->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "Type", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tableFiles->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "Date", nullptr));
        groupStats->setTitle(QCoreApplication::translate("MainWindow", "Basic statistic", nullptr));
        lblTotalFiles->setText(QCoreApplication::translate("MainWindow", "Total files: 0", nullptr));
        lblTotalSize->setText(QCoreApplication::translate("MainWindow", "Total size: 0 MB", nullptr));
        lblLargestFile->setText(QCoreApplication::translate("MainWindow", "Largest file: -", nullptr));
        lblMostCommon->setText(QCoreApplication::translate("MainWindow", "The most common type: -", nullptr));
        lblStatus->setText(QCoreApplication::translate("MainWindow", "Status: Waiting", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
