#include "trackgroupnameview.h"
#include "trackgroup.h"
#include "track.h"

#include <QPushButton>
#include <QHBoxLayout>

TrackGroupNameView::TrackGroupNameView(TrackGroup *trackGroup, QWidget *parent) :
	QWidget(parent)
{
	QHBoxLayout *layout = new QHBoxLayout();
	layout->setContentsMargins(QMargins());
	layout->setSpacing(1);
	setLayout(layout);

	connect(trackGroup, SIGNAL(trackAdded(Track *)), this, SLOT(trackAdded(Track *)));
	connect(trackGroup, SIGNAL(trackRemoved(int)), this, SLOT(trackRemoved(int)));
}

void TrackGroupNameView::trackAdded(Track *track)
{
	QPushButton *trackNameView = new QPushButton(track->getName());
	trackNameViews.append(trackNameView);
	layout()->addWidget(trackNameView);
	trackNameView->show();
	adjustSize();
}

void TrackGroupNameView::trackRemoved(int index)
{
	QPushButton *trackNameView = trackNameViews[index];
	layout()->removeWidget(trackNameView);
	trackNameViews.removeAt(index);
	delete trackNameView;
	adjustSize();
}
