#include "mainwindow.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QInputDialog>
#include <QString>

#include "trackarea.h"
#include "trackgroupview.h"
#include "track.h"

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	trackArea = new TrackArea(this);
	setCentralWidget(trackArea);

#if 1
	// HACK: add some data!
	for (int i = 0; i < 10; ++i) {
		Track *track = new Track;
		trackArea->getTrackGroupView()->createTrackView(track);
		for (int i = 0; i < 20; ++i) {
			Track::KeyFrame key;
			key.value = qrand() / (RAND_MAX * 0.5f);
			track->setKeyFrame(qrand() % 128, key);
		}
	}
	trackArea->getTrackGroupView()->setCol(0);
	trackArea->getTrackGroupView()->setRow(0);
#endif
}

MainWindow::~MainWindow()
{

}

void MainWindow::on_actionSet_rows_triggered()
{
	bool ok = false;
	int newRowCount = QInputDialog::getInt(this,
					       "Set rows", "",
					       trackArea->getRowCount(),
					       0, INT_MAX, 1, &ok);
	if (ok)
		trackArea->setRowCount(newRowCount);
	else
		QApplication::beep();
}
