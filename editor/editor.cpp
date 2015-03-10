#include <QApplication>
#include <QMessageBox>
#include <QTcpServer>

#ifdef QT_WEBSOCKETS_LIB
#include <QWebSocketServer>
#endif

#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setOrganizationName("GNU Rocket Foundation");
	app.setApplicationName("GNU Rocket Editor");
	app.setWindowIcon(QIcon(":appicon.ico"));

	QTcpServer tcpServerSocket;
	if (!tcpServerSocket.listen(QHostAddress::Any, 1338)) {
		QMessageBox::critical(NULL, NULL, QString("Could not start server:\n%1").arg(tcpServerSocket.errorString()), QMessageBox::Ok);
		exit(EXIT_FAILURE);
	}

#ifdef QT_WEBSOCKETS_LIB
	QWebSocketServer wsServerSocket("GNU Rocket websocket-server", QWebSocketServer::NonSecureMode);
	if (!wsServerSocket.listen(QHostAddress::Any, 1339)) {
		QMessageBox::critical(NULL, NULL, QString("Could not start server:\n%1").arg(wsServerSocket.errorString()), QMessageBox::Ok);
		exit(EXIT_FAILURE);
	}

	MainWindow mainWindow(&tcpServerSocket, &wsServerSocket);
#else
	MainWindow mainWindow(&tcpServerSocket, NULL);
#endif
	if (app.arguments().size() > 1) {
		if (app.arguments().size() > 2) {
			QMessageBox::critical(&mainWindow, NULL, QString("usage: %1 [filename.rocket]").arg(argv[0]), QMessageBox::Ok);
			exit(EXIT_FAILURE);
		}
		mainWindow.loadDocument(app.arguments()[1]);
	} else
		mainWindow.fileNew();
	
	mainWindow.show();
	app.exec();

	return 0;
}
