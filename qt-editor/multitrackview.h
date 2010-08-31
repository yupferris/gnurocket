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

	const TrackView *getCurrentTrackView() const
	{
		if (currCol < 0 || !trackViews.size())
			return NULL;
		return trackViews[currCol];
	}
protected:

	QHBoxLayout *horizontalLayout;
	QList<TrackView *> trackViews;
	int currRow, currCol;
};

#endif // MULTITRACKVIEW_H
