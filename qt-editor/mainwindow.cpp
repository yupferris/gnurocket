#include "mainwindow.h"

#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>

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
