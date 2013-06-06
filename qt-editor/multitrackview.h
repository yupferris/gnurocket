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

	QRect getCurrentTrackRect() const;

protected:

	QList<TrackView *> trackViews;
	int currRow, currCol;
};

#endif // MULTITRACKVIEW_H
