#ifndef TRACKAREA_H
#define TRACKAREA_H

#include <QScrollArea>

class MultiTrackView;
class RowNumberView;

class TrackArea : public QScrollArea {
	Q_OBJECT

public:
	TrackArea(QWidget *parent = 0);

	void setRow(int row);
	int getRowCount() const;
	void setRowCount(int rows);
protected:
	void scrollContentsBy(int dx, int dy);
	void updateHScrollbar();
	void updateVScrollbar();
	void keyPressEvent(QKeyEvent *event);
	void resizeEvent(QResizeEvent *event);
	MultiTrackView *multiTrackView;
	RowNumberView *rowNumberView;
};

#endif // TRACKAREA_H
