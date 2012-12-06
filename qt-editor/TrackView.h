#ifndef TRACKVIEW_H
#define TRACKVIEW_H

#include <QtGui/QTableView>
#include <QMap>
#include "SyncTrack.h"

class SyncTrack;

class TrackModel;

class TrackView : public QTableView {
    Q_OBJECT

public:
    TrackView(QWidget *parent = 0);

    void scrollTo(const QModelIndex &index, ScrollHint hint)
    {
        (void)hint;
        QTableView::scrollTo(index, QAbstractItemView::PositionAtCenter);
    }

    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);

    void currentChanged(const QModelIndex &current, const QModelIndex &previous);

    SyncTrack* getTrack(std::string name);
    int getTrackIndex(std::string name);
    void createTrack(std::string name);

    int GetCurrentRow();
signals:
    void rowChanged(int row);
    void cellChanged(std::string column, SyncKey key);
    void interpolationTypeChanged(std::string, SyncKey key);
    void deleteKey(std::string, SyncKey key);
    void pauseTriggered();

public slots:
    void ChangeRow(int row);

protected:

    TrackModel *trackModel;
};

#endif // TRACKVIEW_H
