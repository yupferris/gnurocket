#include "trackarea.h"
#include "multitrackview.h"
#include "trackview.h"
#include "rownumberview.h"

#include <QScrollBar>
#include <QKeyEvent>

TrackArea::TrackArea(QWidget *parent) :
	QScrollArea(parent)
{
#ifdef Q_OS_WIN
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
	setViewportMargins(fontMetrics().width(' ') * 8, 0, 0, 0);

	setFrameShape(QFrame::NoFrame);
	setBackgroundRole(QPalette::Dark);
	setAutoFillBackground(true);
}

void TrackArea::setRow(int row)
{
	multiTrackView->setRow(row);
	rowNumberView->setRowHilight(row);
	updateVScrollbar();
}

void TrackArea::updateHScrollbar()
{
	// make sure current track is visible
	const QRect trackRect = multiTrackView->getCurrentTrackRect();
	int x = trackRect.x() + trackRect.width() / 2;
	int y = verticalScrollBar()->value() + viewport()->height() / 2;
	ensureVisible(x, y, trackRect.width() / 2, 0);
}

void TrackArea::updateVScrollbar()
{
	// vertically center current row
	QFontMetrics fm(font());
	int y = multiTrackView->getRow() * fm.lineSpacing() + fm.lineSpacing() / 2;
	verticalScrollBar()->setValue(y - viewport()->height() / 2);
}

void TrackArea::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Up:
		setRow(multiTrackView->getRow() - 1);
		break;

	case Qt::Key_Down:
		setRow(multiTrackView->getRow() + 1);
		break;

	case Qt::Key_PageUp:
		setRow(multiTrackView->getRow() - 16);
		break;

	case Qt::Key_PageDown:
		setRow(multiTrackView->getRow() + 16);
		break;

	case Qt::Key_Left:
		multiTrackView->setCol(multiTrackView->getCol() - 1);
		updateHScrollbar();
		break;

	case Qt::Key_Right:
		multiTrackView->setCol(multiTrackView->getCol() + 1);
		updateHScrollbar();
		break;
	}
}

void TrackArea::resizeEvent(QResizeEvent *event)
{
	updateHScrollbar();
	updateVScrollbar();
	QScrollArea::resizeEvent(event);
	rowNumberView->move(0, widget()->y());
	rowNumberView->resize(viewport()->x(), widget()->height());
}

void TrackArea::scrollContentsBy(int dx, int dy)
{
	// syncronize left margin
	if (dy)
		rowNumberView->move(rowNumberView->x(), rowNumberView->y() + dy);

	QScrollArea::scrollContentsBy(dx, dy);
}
