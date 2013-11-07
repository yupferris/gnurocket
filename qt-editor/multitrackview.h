#ifndef MULTITRACKVIEW_H
#define MULTITRACKVIEW_H

#include <QWidget>

class TrackView;
class QHBoxLayout;

class MultiTrackView : public QWidget {
	Q_OBJECT

public:
	MultiTrackView(QWidget *parent = 0);

	int getRow() { return currRow; }
	void setRow(int row);
	int getCol() { return currCol; }
	void setCol(int col);

	void setRowCount(int rows);
	int getRowCount() const { return rowCount; };
	int getColCount() const { return trackViews.size(); };

	QRect getCurrentTrackRect() const;

protected:
	void changeEvent(QEvent *event);

	QList<TrackView *> trackViews;
	int currRow, currCol;
	int rowCount;
};

#endif // MULTITRACKVIEW_H
