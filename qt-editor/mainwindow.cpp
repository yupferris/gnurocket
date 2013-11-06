#include "mainwindow.h"

#include <QMenuBar>
#include <QStatusBar>

#include "trackarea.h"

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	trackArea = new TrackArea(this);
	setCentralWidget(trackArea);
}

MainWindow::~MainWindow()
{

}
