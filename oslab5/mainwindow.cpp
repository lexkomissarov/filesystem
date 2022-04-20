#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model = new QFileSystemModel(this);
    model->setFilter(QDir::QDir::AllEntries);
    model->setRootPath("");
    ui->lvBackup->setModel(model);
    ui->lvSource->setModel(model);
    connect(ui->lvBackup, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(on_lvSource_doubleClicked(QModelIndex)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void contentDifference(QDir &sDir, QDir &dDir, QFileInfoList &diffList)
{
    foreach(QFileInfo sInfo, sDir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot, QDir::Name|QDir::DirsFirst)){
        bool fileExists = false;
        foreach(QFileInfo dInfo, dDir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot, QDir::Name|QDir::DirsFirst)){
            if (sInfo.fileName() == dInfo.fileName()){
                if (sInfo.isDir() || sInfo.lastModified() <= dInfo.lastModified())
                    fileExists = true;
                break;
            }
        }
        if (!fileExists)
            diffList.append(sInfo);
        if (sInfo.isFile())
            continue;
        if (fileExists){
            sDir.cd(sInfo.fileName());
            dDir.cd(sInfo.fileName());
            contentDifference(sDir, dDir, diffList);
            sDir.cdUp();
            dDir.cdUp();
        }
        else {
            sDir.cd(sInfo.fileName());
            recursiveContentList(sDir, diffList);
            sDir.cdUp();
        }
    }
}

void recursiveContentList(QDir &dir, QFileInfoList &contentList)
{
    foreach(QFileInfo info, dir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot, QDir::Name|QDir::DirsFirst)){
        contentList.append(info);
        if (info.isDir() && dir.cd(info.fileName())){
            recursiveContentList(dir, contentList);
            dir.cdUp();
        }
    }
}

void MainWindow::on_lvSource_doubleClicked(const QModelIndex &index)
{
    QListView* listView = (QListView*)sender();
    QFileInfo fileInfo = model->fileInfo(index);
    if (fileInfo.fileName() == ".."){
        QDir dir = fileInfo.dir();
        dir.cdUp();
        listView->setRootIndex(model->index(dir.absolutePath()));
    }
    else if (fileInfo.fileName() == "."){
        listView->setRootIndex(model->index(""));
    }
    else if (fileInfo.isDir()){
        listView->setRootIndex(index);
    }
}


void MainWindow::on_btnBackup_clicked()
{
    QDir sDir = QDir(model->filePath(ui->lvSource->rootIndex()));
    QDir dDir = QDir(model->filePath(ui->lvBackup->rootIndex()));

    QFileInfoList diffList = QFileInfoList();
    contentDifference(sDir, dDir, diffList);

    foreach(QFileInfo diffInfo, diffList){
        QString backupPath = diffInfo.filePath().replace(sDir.absolutePath(), dDir.absolutePath());
        if (diffInfo.isFile()){
            QFile::remove(backupPath);
            QFile::copy(diffInfo.absoluteFilePath(), backupPath);
    }
        if (diffInfo.isDir()){
            dDir.mkdir(backupPath);
        }
    }
}

