#include <iostream>
#include "ServerWrapper.h"
#include "TrackView.h"
#include "MainWindow.h"
#include <cassert>
#include <fcntl.h>
#include <unistd.h>
#include "../sync/base.h"
#include <stdexcept>
#include "../sync/track.h"

const int SYNC_PORT = 1338;

SOCKET clientConnect(SOCKET serverSocket, sockaddr_in *host)
{
    sockaddr_in hostTemp;
    socklen_t hostSize = sizeof(sockaddr_in);
    SOCKET clientSocket = accept(serverSocket, (sockaddr*)&hostTemp, &hostSize);
    if (INVALID_SOCKET == clientSocket) return INVALID_SOCKET;

    const char *expectedGreeting = CLIENT_GREET;
    char recievedGreeting[128];

    recv(clientSocket, recievedGreeting, int(strlen(expectedGreeting)), 0);

    if (strncmp(expectedGreeting, recievedGreeting, strlen(expectedGreeting)) != 0)
    {
        closesocket(clientSocket);
        return INVALID_SOCKET;
    }
    std::cout << "Greeting was ok" << std::endl;

    const char *greeting = SERVER_GREET;
    send(clientSocket, greeting, int(strlen(greeting)), 0);

    if (NULL != host) *host = hostTemp;
    return clientSocket;
}

ServerWrapper::ServerWrapper(MainWindow *parent) :
    QObject((QObject*)parent), mainWindow(parent)
{
    isClientPaused = true;
#ifdef WIN32
    WSADATA wsa;
    if (0 != WSAStartup(MAKEWORD(2, 0), &wsa))
        assert(0);
#endif

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        assert(0);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(SYNC_PORT);

    if (bind(serverSocket, (struct sockaddr *)&addr, sizeof(addr)))
        throw std::runtime_error("Coult not bind server on port");

    if (listen(serverSocket, SOMAXCONN)) {
        throw std::runtime_error("Coult not listen on socket");
    }
}

ServerWrapper::~ServerWrapper() {
}

void ServerWrapper::update()
{
    if (clientSocket.connected()) {
        startRead();
    } else {
        acceptConnection();
    }
}

void ServerWrapper::acceptConnection() {

    SOCKET newSocket = INVALID_SOCKET;
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(serverSocket, &fds);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;

    // look for new clients
    if (select(serverSocket+1, &fds, NULL, NULL, &timeout) > 0)
    {
        sockaddr_in client;
        newSocket = clientConnect(serverSocket, &client);

        if (INVALID_SOCKET != newSocket)
        {

            char temp[256];
            snprintf(temp, 256, "Connected to %s", inet_ntoa(client.sin_addr));
            mainWindow->SetStatusMessage(temp);

            clientIndex = 0;

            this->clientSocket = ClientSocket(newSocket);
            this->clientSocket.sendPauseCommand(true);
            this->clientSocket.sendSetRowCommand(mainWindow->trackView->GetCurrentRow());
            isClientPaused = true;
        }
    }
}

void ServerWrapper::startRead() {

    //std::cout << "startRead ..." << std::endl;

    while (this->clientSocket.pollRead())
        processCommands();

    return;
}

void ServerWrapper::processCommands()
{
    int strLen, serverIndex, newRow;
    std::string trackName;
    unsigned char cmd = 0;
    if (clientSocket.recv((char*)&cmd, 1, 0)) {
        std::cout << "Cmd: " << (int)cmd << std::endl;
        switch (cmd) {
        case GET_TRACK:
            {
            // Get index
            uint32_t index;
            if (!clientSocket.recv((char*)&index, sizeof(index), 0))
                return;
            std::cout << "Index: " << index << std::endl;

            // Get track string length
            clientSocket.recv((char *)&strLen, sizeof(int), 0);
            strLen = ntohl(strLen);
            std::cout << "Strlen: " << strLen << std::endl;
            if (!clientSocket.connected())
                return;

            // Get track string
            trackName.resize(strLen);
            if (!clientSocket.recv(&trackName[0], strLen, 0))
                return;
            std::cout << "TrackName: " << trackName << std::endl;

            TrackView *trackView = mainWindow->trackView;
            SyncTrack *track = trackView->getTrack(trackName);
            if (!track) {
                trackView->createTrack(trackName);
                track = trackView->getTrack(trackName);
            }

            clientSocket.clientTracks[trackName] = clientIndex++;

            SyncTrack::iterator it = track->begin();
            for (; it != track->end(); it++) {
                struct track_key key;
                key.row = it->second.row;
                key.value = it->second.value;
                key.type = (key_type)it->second.type;

                clientSocket.sendSetKeyCommand(trackName, key);
                //SendKey(track->GetName(), it->second);
            }

            }
            break;
        case SET_ROW:
            clientSocket.recv((char*)&newRow, sizeof(newRow), 0);
            newRow = ntohl(newRow);
//			trackView->setEditRow(ntohl(newRow));
            emit rowChanged(newRow);
            break;
        default:
            std::cout << "Unknown command: " << (int)cmd << std::endl;
            break;
        }
    }
}

/*
void ServerWrapper::SendKey(std::string name, SyncKey key) {
    //QDataStream stream(client);
    uint8_t cmd = CMD_SET_KEY;
    uint32_t trackIndex = mainWindow->trackView->getTrackIndex(name);
    uint8_t type = key.type;

    union {
            float f;
            uint32_t i;
    } v;
    v.f = key.value;

    stream << cmd;
    stream << trackIndex;
    stream << key.row;
    stream << v.i;
    stream << type;
}*/

void ServerWrapper::ChangeRow(int row)
{
    if (isClientPaused) {
        clientSocket.sendSetRowCommand(row);
    }
}

void ServerWrapper::cellChanged(std::string track, SyncKey key2)
{
    std::cout << "cell changed" << std::endl;
    struct track_key key;
    key.row = key2.row;
    key.value = key2.value;
    key.type = (key_type)key2.type;
    clientSocket.sendSetKeyCommand(track, key);
}

void ServerWrapper::interpolationTypeChanged(std::string track, SyncKey key2)
{
    struct track_key key;
    key.row = key2.row;
    key.value = key2.value;
    key.type = (key_type)key2.type;
    clientSocket.sendSetKeyCommand(track, key);
}

void ServerWrapper::SendPause()
{
    clientSocket.sendPauseCommand(!isClientPaused);
    isClientPaused = !isClientPaused;
}

void ServerWrapper::keyDeleted(std::string track, SyncKey key)
{
    clientSocket.sendDeleteKeyCommand(track, key.row);
}

void ServerWrapper::sendExportCommand()
{
    clientSocket.sendSaveCommand();
}
