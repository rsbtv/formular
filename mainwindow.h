#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QCryptographicHash>
#include <QTableWidgetItem>
#include <QTextStream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_setDrive_clicked();
    QString calculateChecksum(const QString &path);
    QMap<QString, int> readChecksumsFromFile(const QString &filePath);
    void saveDriveInfo(const QString &path, const QString &checksum, int id);
    void displayDriveInfo(const QString &path);

private:
    Ui::MainWindow *ui;
    int checksumCounter = 0;
};

#endif // MAINWINDOW_H
