#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/qglobal.h>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QMainWindow>
#else
#include <QtGui/QMainWindow>
#endif

#include <QWidget>
#include <QFileSystemModel>
#include <QDir>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_lvSource_doubleClicked(const QModelIndex &index);

    void on_btnBackup_clicked();

private:
    Ui::MainWindow *ui;
    QFileSystemModel *model;
};

// Ищет отсутствубщие или неактуальные элементы в резервной директории
void contentDifference(QDir &sDir, QDir &dDir, QFileInfoList &diffList);

// Наполняет список всех вложенных директорий и файлов
void recursiveContentList(QDir &dir, QFileInfoList &contentList);

#endif // MAINWINDOW_H
