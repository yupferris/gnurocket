#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QTimer>
#include "TrackView.h"
#include <iostream>
#include "SyncTrack.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    trackView = new TrackView(this);
    setCentralWidget(trackView);

    SetStatusMessage("Not connected");

    serverWrapper = new ServerWrapper(this);

    connect(trackView, SIGNAL(rowChanged(int)), serverWrapper, SLOT(ChangeRow(int)));
    connect(serverWrapper, SIGNAL(rowChanged(int)), trackView, SLOT(ChangeRow(int)));
    connect(trackView, SIGNAL(cellChanged(std::string,SyncKey)), serverWrapper, SLOT(cellChanged(std::string,SyncKey)));
    connect(trackView, SIGNAL(interpolationTypeChanged(std::string,SyncKey)), serverWrapper, SLOT(interpolationTypeChanged(std::string,SyncKey)));
    connect(trackView, SIGNAL(pauseTriggered()), serverWrapper, SLOT(SendPause()));
    connect(trackView, SIGNAL(deleteKey(std::string,SyncKey)), serverWrapper, SLOT(keyDeleted(std::string,SyncKey)));

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), serverWrapper, SLOT(update()));
    timer->start(16);
}

void MainWindow::SetStatusMessage(QString msg) {
    ui->statusBar->showMessage(msg);
}

MainWindow::~MainWindow()
{
    delete serverWrapper;
    delete ui;
}

void MainWindow::OpenFile()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Directory"));
    if (path.isNull() == false)
    {
        std::cout << "Not implemented" << std::endl;
    }
}

void MainWindow::RunExport()
{
    serverWrapper->sendExportCommand();
}

