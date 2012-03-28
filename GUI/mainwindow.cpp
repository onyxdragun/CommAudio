#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QMessageBox>
#include "connectdlg.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Signals and Slots ------------------------------------------------------------------
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(openConnect()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    connect(ui->pushButton_play, SIGNAL(pressed()), this, SLOT(player_play()));
    connect(ui->pushButton_pause, SIGNAL(pressed()), this, SLOT(player_pause()));
    connect(ui->pushButton_stop, SIGNAL(pressed()), this, SLOT(player_stop()));

    // TreeWidget Setup (local) -----------------------------------------------------------
    initLocalDir(ui);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addChildren(QTreeWidgetItem* item,QString filePath)
{
    QDir* rootDir = new QDir(filePath);
    QFileInfoList filesList = rootDir->entryInfoList();

    foreach(QFileInfo fileInfo, filesList)
    {
        QTreeWidgetItem* child = new QTreeWidgetItem();
        child->setText(0,fileInfo.fileName());

        if(fileInfo.isFile())
        {
          child->setText(1,QString::number(fileInfo.size()));
        }

        if(fileInfo.isDir())
        {
          child->setIcon(0,*(new QIcon("folder.jpg")));
          child->setText(2,fileInfo.filePath());
        }

        item->addChild(child);
    }
}

void MainWindow::openConnect()
{
    ConnectDlg *cdlg = new ConnectDlg();

    cdlg->setModal(false);
    cdlg->exec();
}

void MainWindow::initLocalDir(Ui::MainWindow *ui)
{
    // TreeWidget Setup (local) -----------------------------------------------------------
    ui->treeWidget_Local->setColumnCount(4);
    ui->treeWidget_Local->setHeaderLabels(QStringList() << "Title" << "Artist" << "Size" << "Path");
    QDir *rootDir = new QDir(QApplication::applicationDirPath());
    rootDir->setFilter(QDir::Files | QDir::NoDotAndDotDot);
    QFileInfoList filelist = rootDir->entryInfoList();

    foreach(QFileInfo fileInfo, filelist)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem();

        item->setText(0,fileInfo.fileName());

        if(fileInfo.isFile())
        {
            item->setText(2,QString::number(fileInfo.size()));
            item->setIcon(0,*(new QIcon("file.jpg")));
        }

        ui->treeWidget_Local->addTopLevelItem(item);
    }
}

void MainWindow::player_play()
{
    QMessageBox qm;
    qm.setText("Playing...");
    qm.exec();
}
void MainWindow::player_pause()
{
    QMessageBox qm;
    qm.setText("Pausing...");
    qm.exec();
}

void MainWindow::player_stop()
{
    QMessageBox qm;
    qm.setText("Stopping...");
    qm.exec();
}