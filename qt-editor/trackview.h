#ifndef TRACKVIEW_H
#define TRACKVIEW_H

#include <QWidget>
#include <QScrollArea>
#include <QFontInfo>
#include <QEvent>
#include <QPainter>
#include <QStylePainter>

class QHBoxLayout;
class QLabel;

class TrackView : public QWidget {
	Q_OBJECT
public:
	TrackView(QWidget *parent = 0) :
		QWidget(parent),
		rowHilight(-1)
	{
		setAttribute(Qt::WA_OpaquePaintEvent, true);
	}

	void setRowHilight(int row) {
		int oldRow = rowHilight;
		rowHilight = row;

		if (oldRow >= 0)
			updateRow(oldRow);
		if (row >= 0)
			updateRow(row);
	}

	void updateRow(int row)
	{
		update(0, row * fontMetrics().lineSpacing(), width(), fontMetrics().lineSpacing());
	}

protected:
	void paintEvent(QPaintEvent *event);
	void changeEvent(QEvent *event);

	int rowHilight;
};

#endif // TRACKVIEW_H
