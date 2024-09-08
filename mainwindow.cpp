#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label->clear();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_setDrive_clicked() // выбор носителя
{
    QString drivePath = QFileDialog::getExistingDirectory(this, "Select Drive"); // получаем папку
    if (drivePath.isEmpty())
        return;

    QString checksum = calculateChecksum(drivePath); // хэш сумма
    QMap<QString, int> existingChecksums = readChecksumsFromFile("checksums.txt"); // смотрим имеющиеся суммы

    if (existingChecksums.contains(checksum))  // если совпало
    {
        int existingId = existingChecksums[checksum];
        ui->label->setText("№" + QString::number(existingId));
        displayDriveInfo(drivePath);
    } else
    {
        checksumCounter++;
        ui->label->setText("№" + QString::number(checksumCounter));
        saveDriveInfo(drivePath, checksum, checksumCounter);
        displayDriveInfo(drivePath);
    }
}

QString MainWindow::calculateChecksum(const QString &path) // вычисляем контрольную сумму (по заданному пути файла)
{
    QCryptographicHash hash(QCryptographicHash::Sha256); // алгоритм SHA-256 для вычисления хэш-суммы.
    QDir dir(path); // директория по указанному пути
    QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot); // список файлов без лишних точек

    for (const QFileInfo &fileInfo : files) // цикл по каждому объекту fileInfo в списке файлов
    {
        QFile file(fileInfo.absoluteFilePath());
        if (file.open(QIODevice::ReadOnly))
        {
            hash.addData(&file); // Если файл успешно открыт, его содержимое добавляется в объект hash для вычисления хэш-суммы.
            file.close();
        }
    }

    return hash.result().toHex(); // результат вычисления хэш суммы в 16-ричной системе
}

QMap<QString, int> MainWindow::readChecksumsFromFile(const QString &filePath) //  метод возвращает контрольные суммы как ключи
// и их идентификаторы как значения.
{
    QMap<QString, int> checksums;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QString line = file.readLine().trimmed(); // без пробелов по краям
            QStringList parts = line.split(", ");
            if (parts.size() == 2) // parts содержит ровно 2 элемента, что соответствует ожидаемому формату строки с идентификатором и контрольной суммой.
            {
                int id = parts[0].split(": ")[1].toInt();
                QString checksum = parts[1].split(": ")[1];
                checksums.insert(checksum, id);
            }
        }
        file.close();
    }
    return checksums;
}

void MainWindow::saveDriveInfo(const QString &path, const QString &checksum, int id) // сохранение информации в файл
{
    QFile file("checksums.txt");
    if (file.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&file);
        out << "ID: " << id << ", Checksum: " << checksum << "\n"; // Save ID and checksum
        out << "Drive: " << path << "\n";
        QFileInfoList files = QDir(path).entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
        for (const QFileInfo &fileInfo : files) {
            out << fileInfo.fileName() << " " << fileInfo.size() << "\n";
        }
        file.close();
    }
}

void MainWindow::displayDriveInfo(const QString &path) // вывод информации в интерфейс
{
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Название файла" << "Размер в байтах");

    QFileInfoList files = QDir(path).entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    ui->tableWidget->setRowCount(files.size());

    for (int i = 0; i < files.size(); ++i)
    {
        const QFileInfo &fileInfo = files[i];
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(fileInfo.fileName()));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(fileInfo.size())));
    }
}
