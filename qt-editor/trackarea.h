#ifndef TRACKAREA_H
#define TRACKAREA_H

#include <QScrollArea>

class MultiTrackView;
class RowNumberView;

class TrackArea : public QScrollArea {
	Q_OBJECT

public:
	TrackArea(QWidget *parent = 0);

protected:
	void scrollContentsBy(int dx, int dy);
	void updateScrollbars();
	void keyPressEvent(QKeyEvent *event);
	void resizeEvent(QResizeEvent *event);
	MultiTrackView *multiTrackView;
	RowNumberView *rowNumberView;
};

#endif // TRACKAREA_H
