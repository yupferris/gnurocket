#ifndef SERVERWRAPPER_H
#define SERVERWRAPPER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "SyncTrack.h"
#include "clientsocket.h"
class MainWindow;

enum ClientState {
    WAITING,
    VERIFYING,
    READY,
    STATE_ERROR
};

class ServerWrapper : public QObject
{
    Q_OBJECT
public:
    explicit ServerWrapper(MainWindow *parent = 0);
    ~ServerWrapper();

signals:
    void rowChanged(int row);

public slots:
    void update();

    void acceptConnection();
    void startRead();
    void processCommands();
    //void SendKey(std::string name, SyncKey key);
    void ChangeRow(int row);
    void cellChanged(std::string track, SyncKey key);
    void interpolationTypeChanged(std::string track, SyncKey key2);
    void SendPause();
    void keyDeleted(std::string, SyncKey key);
    void sendExportCommand();

    //bool recv(char *buffer, size_t length, int flags);


private:
    MainWindow *mainWindow;
    QTcpServer *server;
//    QTcpSocket *client;
    int isClientPaused;
    ClientState client_state;
    //int socket;
    int clientIndex;
    ClientSocket clientSocket;
};

#endif // SERVERWRAPPER_H
