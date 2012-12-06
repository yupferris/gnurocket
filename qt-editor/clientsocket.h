#include "../sync/base.h"
#include <map>
#include <string>
#include <QTcpSocket>

class ClientSocket {
public:
	ClientSocket() : socket(NULL) {}
	explicit ClientSocket(QAbstractSocket *socket) : clientPaused(true), socket(socket) {}

	bool connected() const
	{
		return socket != NULL;
	}

	void disconnect()
	{
		socket = NULL;
		clientTracks.clear();
	}

	bool recv(char *buffer, size_t length)
	{
		if (!connected())
			return false;
		qint64 ret = socket->read(buffer, length);
		if (ret != length) {
			disconnect();
			return false;
		}
		return true;
	}

	bool send(const char *buffer, size_t length)
	{
		if (!connected())
			return false;
		qint64 ret = socket->write(buffer, length);
		if (ret != length) {
			disconnect();
			return false;
		}
		return true;
	}

	bool pollRead()
	{
		if (!connected())
			return false;
		return socket->bytesAvailable() > 0;
	}

	void sendSetKeyCommand(const std::string &trackName, const struct track_key &key);
	void sendDeleteKeyCommand(const std::string &trackName, int row);
	void sendSetRowCommand(int row);
	void sendPauseCommand(bool pause);
	void sendSaveCommand();

	bool clientPaused;
	std::map<const std::string, size_t> clientTracks;

private:
	QAbstractSocket *socket;
};
