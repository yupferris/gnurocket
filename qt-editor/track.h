#ifndef TRACK_H
#define TRACK_H

#include <QMap>
#include <QString>

class Track {
public:
	struct KeyFrame {
		float value;
	};

	void setKeyFrame(int row, const KeyFrame &key) { keys.insert(row, key); }
	void removeKeyFrame(int row) { keys.remove(row); }

	bool isKeyFrame(int row) const { return keys.contains(row); }
	const KeyFrame &getKeyFrame(int row) const
	{
		Q_ASSERT(isKeyFrame(row));
		return keys.find(row).value();
	}

private:
	QString name;
	QMap<int, KeyFrame> keys;
};

#endif // TRACK_H
