#include "trackarea.h"
#include "multitrackview.h"
#include "trackview.h"
#include "rownumberview.h"

#include <QScrollBar>
#include <QKeyEvent>

TrackArea::TrackArea(QWidget *parent) :
	QScrollArea(parent)
{
#ifdef Q_WS_WIN
	setFont(QFont("Fixedsys"));
#else
	QFont font("Monospace");
	font.setStyleHint(QFont::TypeWriter);
	setFont(font);
#endif
	multiTrackView = new MultiTrackView(this);
	setWidget(multiTrackView);

	setAlignment(Qt::AlignCenter);

	rowNumberView = new RowNumberView(this);

	setFrameShape(QFrame::Panel);
	setBackgroundRole(QPalette::Dark);
}

void TrackArea::updateScrollbars()
{
	// make sure current track is visible
	const TrackView *trackView = multiTrackView->getCurrentTrackView();
	int x = trackView->x() + trackView->width() / 2;
	ensureVisible(x, verticalScrollBar()->value() + viewport()->height() / 2, trackView->width() / 2, 0);

	// vertically center current row
	QFontMetrics fm(trackView->font());
	int y = multiTrackView->getRow() * fm.lineSpacing() + fm.lineSpacing() / 2;
	verticalScrollBar()->setValue(y - viewport()->height() / 2);
}

void TrackArea::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Up:
		multiTrackView->setRow(multiTrackView->getRow() - 1);
		rowNumberView->setRowHilight(multiTrackView->getRow());
		updateScrollbars();
		break;

	case Qt::Key_Down:
		multiTrackView->setRow(multiTrackView->getRow() + 1);
		rowNumberView->setRowHilight(multiTrackView->getRow());
		updateScrollbars();
		break;

	case Qt::Key_PageUp:
		multiTrackView->setRow(multiTrackView->getRow() - 16);
		rowNumberView->setRowHilight(multiTrackView->getRow());
		updateScrollbars();
		break;

	case Qt::Key_PageDown:
		multiTrackView->setRow(multiTrackView->getRow() + 16);
		rowNumberView->setRowHilight(multiTrackView->getRow());
		updateScrollbars();
		break;

	case Qt::Key_Left:
		multiTrackView->setCol(multiTrackView->getCol() - 1);
		updateScrollbars();
		break;

	case Qt::Key_Right:
		multiTrackView->setCol(multiTrackView->getCol() + 1);
		updateScrollbars();
		break;
	}
}

void TrackArea::resizeEvent(QResizeEvent *event)
{
	updateScrollbars();
	QScrollArea::resizeEvent(event);
	rowNumberView->move(0, widget()->y());
	rowNumberView->resize(rowNumberView->width(), widget()->height());
	setViewportMargins(rowNumberView->width(), 0, 0, 0);
}

void TrackArea::scrollContentsBy(int dx, int dy)
{
	// syncronize left margin
	if (dy)
		rowNumberView->move(rowNumberView->x(), rowNumberView->y() + dy);

	QScrollArea::scrollContentsBy(dx, dy);
}
