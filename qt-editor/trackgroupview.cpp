#include "trackgroupview.h"
#include "trackview.h"

#include <QApplication>
#include <QHBoxLayout>

TrackGroupView::TrackGroupView(QWidget *parent) :
	QWidget(parent),
	currRow(-1),
	currCol(-1)
{
	QHBoxLayout *layout = new QHBoxLayout();
	layout->setContentsMargins(QMargins());
	layout->setSpacing(1);
	setLayout(layout);
	setRowCount(128);

	// HACK: add some data!
	for (int i = 0; i < 10; ++i)
		createTrackView();
	setCol(0);
	setRow(0);
}

TrackView *TrackGroupView::createTrackView()
{
	TrackView *trackView = new TrackView;
	trackViews.append(trackView);
	layout()->addWidget(trackView);
	return trackView;
}

QRect TrackGroupView::getCurrentTrackRect() const
{
	if (currCol < 0)
		return QRect(0, 0, 0, 0);

	Q_ASSERT(trackViews.size());
	return trackViews[currCol]->geometry();
}

void TrackGroupView::setRow(int row)
{
	Q_ASSERT(row >= 0 && row < rowCount);

	if (!trackViews.size())
		return;

	Q_ASSERT(currCol >= 0 && currCol < trackViews.size());
	trackViews[currCol]->setRowHilight(row);
	currRow = row;
}

void TrackGroupView::setCol(int col)
{
	if (!trackViews.size())
		return;

	col = qMin(qMax(col, 0), trackViews.size() - 1);
	if (currCol != col) {
		if (currCol >= 0)
			trackViews[currCol]->setRowHilight(-1);

		trackViews[col]->setRowHilight(currRow);
		currCol = col;
	} else
		QApplication::beep();
}

void TrackGroupView::setRowCount(int rows)
{
	setFixedHeight(fontMetrics().lineSpacing() * rows);
	rowCount = rows;
}

void TrackGroupView::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::FontChange) {
		Q_ASSERT(fontInfo().fixedPitch());
		setFixedHeight(fontMetrics().lineSpacing() * rowCount);
	}
}
