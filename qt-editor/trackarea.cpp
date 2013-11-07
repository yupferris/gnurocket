#include "trackarea.h"
#include "trackgroupview.h"
#include "trackview.h"
#include "rownumberview.h"

#include <QScrollBar>
#include <QKeyEvent>
#include <QApplication>

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
	trackGroupView = new TrackGroupView(this);
	setWidget(trackGroupView);

	setAlignment(Qt::AlignCenter);

	rowNumberView = new RowNumberView(this);
	setViewportMargins(fontMetrics().width(' ') * 8, 0, 0, 0);

	setFrameShape(QFrame::NoFrame);
	setBackgroundRole(QPalette::Dark);
	setAutoFillBackground(true);
}

void TrackArea::setRow(int row)
{
	row = qMin(qMax(row, 0), getRowCount() - 1);
	if (trackGroupView->getRow() != row) {
		trackGroupView->setRow(row);
		rowNumberView->setRowHilight(row);
	} else
		QApplication::beep();

	updateVScrollbar();
}

void TrackArea::setCol(int col)
{
	trackGroupView->setCol(col);
	updateHScrollbar();
}

int TrackArea::getRowCount() const
{
	return trackGroupView->getRowCount();
}

void TrackArea::setRowCount(int rows)
{
	// propagate row-count to both widgets
	rowNumberView->setRowCount(rows);
	trackGroupView->setRowCount(rows);
}

void TrackArea::updateHScrollbar()
{
	// make sure current track is visible
	const QRect trackRect = trackGroupView->getCurrentTrackRect();
	int x = trackRect.x() + trackRect.width() / 2;
	int y = verticalScrollBar()->value() + viewport()->height() / 2;
	ensureVisible(x, y, trackRect.width() / 2, 0);
}

void TrackArea::updateVScrollbar()
{
	// vertically center current row
	QFontMetrics fm(font());
	int y = trackGroupView->getRow() * fm.lineSpacing() + fm.lineSpacing() / 2;
	verticalScrollBar()->setValue(y - viewport()->height() / 2);
}

void TrackArea::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Up:
		setRow(trackGroupView->getRow() - 1);
		break;

	case Qt::Key_Down:
		setRow(trackGroupView->getRow() + 1);
		break;

	case Qt::Key_PageUp:
		setRow(trackGroupView->getRow() - 16);
		break;

	case Qt::Key_PageDown:
		setRow(trackGroupView->getRow() + 16);
		break;

	case Qt::Key_Left:
		setCol(trackGroupView->getCol() - 1);
		break;

	case Qt::Key_Right:
		setCol(trackGroupView->getCol() + 1);
		break;

	case Qt::Key_Home:
		if (event->modifiers() & Qt::ControlModifier)
			setCol(0);
		else
			setRow(0);
		break;

	case Qt::Key_End:
		if (event->modifiers() & Qt::ControlModifier)
			setCol(trackGroupView->getColCount() - 1);
		else
			setRow(trackGroupView->getRowCount() - 1);
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
