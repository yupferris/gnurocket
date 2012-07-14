#ifndef TRACKMODEL_H
#define TRACKMODEL_H

#include <QtGui/QTableView>
#include "SyncTrack.h"

class SyncTrack;

class TrackModel : public QAbstractTableModel {
    Q_OBJECT

public:
    TrackModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    void ChangeInterpolationType(const QModelIndex &index);

    void createTrack(std::string name);
    SyncTrack* getTrack(std::string name);
    SyncTrack* GetTrackByColumn(int column);
    int getTrackIndex(std::string name);

    bool HasTrack(std::string name);
    std::string GetTrackName(int column);
    SyncKey GetKey(const QModelIndex &index);
    void DeleteKey(const QModelIndex &index);

signals:
    void cellChanged(std::string column, SyncKey key);


private:
    std::vector<SyncTrack*> tracks;
    std::vector<int> columnOrder;
    int numRows;
};
#endif // TRACKMODEL_H
