#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_setDrive_clicked()
{
    QString drivePath = QFileDialog::getExistingDirectory(this, "Select Drive");
        if (drivePath.isEmpty()) return;

        QString checksum = calculateChecksum(drivePath);

        // Display the checksum in a QLabel
        ui->label->setText("Checksum: " + checksum); // Update this line

        QStringList existingChecksums = readChecksumsFromFile("checksums.txt");

        if (existingChecksums.contains(checksum)) {
            displayDriveInfo(drivePath);
        } else {
            saveDriveInfo(drivePath, checksum);
            displayDriveInfo(drivePath);
        }
}

QString MainWindow::calculateChecksum(const QString &path)
{
    QCryptographicHash hash(QCryptographicHash::Sha256);
    QDir dir(path);
    QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    for (const QFileInfo &fileInfo : files)
    {
        QFile file(fileInfo.absoluteFilePath());
        if (file.open(QIODevice::ReadOnly))
        {
            hash.addData(&file);
            file.close();
        }
    }

    return hash.result().toHex();
}

QStringList MainWindow::readChecksumsFromFile(const QString &filePath)
{
    QStringList checksums;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while (!file.atEnd())
        {
            checksums.append(file.readLine().trimmed());
        }
        file.close();
    }
    return checksums;
}

void MainWindow::saveDriveInfo(const QString &path, const QString &checksum)
{
    QFile file("C:\\Users\\79021\\Desktop\\formular_\\checksums.txt");
    if (file.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&file);
        out << checksum << "\n";
        out << "Drive: " << path << "\n";
        QFileInfoList files = QDir(path).entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
        for (const QFileInfo &fileInfo : files)
        {
            out << fileInfo.fileName() << " " << fileInfo.size() << "\n";
        }
        file.close();
    }
}

void MainWindow::displayDriveInfo(const QString &path)
{
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "File Name" << "Size (Bytes)");

    QFileInfoList files = QDir(path).entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    ui->tableWidget->setRowCount(files.size());

    for (int i = 0; i < files.size(); ++i)
    {
        const QFileInfo &fileInfo = files[i];
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(fileInfo.fileName()));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(fileInfo.size())));
    }
}
