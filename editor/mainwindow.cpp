#include "mainwindow.h"
#include "trackview.h"
#include "syncdocument.h"

#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QFileInfo>
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QTcpServer>
#include <QtEndian>

MainWindow::MainWindow(QTcpServer *serverSocket) :
	QMainWindow(),
	serverSocket(serverSocket),
	clientIndex(0),
	doc(NULL),
	currentTrackView(NULL)
{
	tabWidget = new QTabWidget(this);
	connect(tabWidget, SIGNAL(currentChanged(int)),
	        this, SLOT(onTabChanged(int)));

	setCentralWidget(tabWidget);

	createMenuBar();
	createStatusBar();

	connect(serverSocket, SIGNAL(newConnection()),
	        this, SLOT(onNewConnection()));
}

void MainWindow::showEvent(QShowEvent *event)
{
	QMainWindow::showEvent(event);

	// workaround for QTBUG-16507
	QString filePath = windowFilePath();
	setWindowFilePath(filePath + "foo");
	setWindowFilePath(filePath);
}


void MainWindow::createMenuBar()
{
	fileMenu = menuBar()->addMenu("&File");
	fileMenu->addAction(QIcon::fromTheme("document-new"), "New", this, SLOT(fileNew()), QKeySequence::New);
	fileMenu->addAction(QIcon::fromTheme("document-open"), "&Open", this, SLOT(fileOpen()), QKeySequence::Open);
	fileMenu->addAction(QIcon::fromTheme("document-save"), "&Save", this, SLOT(fileSave()), QKeySequence::Save);
	fileMenu->addAction(QIcon::fromTheme("document-save-as"),"Save &As", this, SLOT(fileSaveAs()), QKeySequence::SaveAs);
	fileMenu->addSeparator();
	fileMenu->addAction("Remote &Export", this, SLOT(fileRemoteExport()), Qt::CTRL + Qt::Key_E);
	recentFilesMenu = fileMenu->addMenu(QIcon::fromTheme("document-open-recent"), "Recent &Files");
	for (int i = 0; i < 5; ++i) {
		recentFileActions[i] = recentFilesMenu->addAction(QIcon::fromTheme("document-open-recent"), "");
		recentFileActions[i]->setVisible(false);
		connect(recentFileActions[i], SIGNAL(triggered()),
		        this, SLOT(openRecentFile()));
	}
	updateRecentFiles();
	fileMenu->addSeparator();
	fileMenu->addAction(QIcon::fromTheme("application-exit"), "E&xit", this, SLOT(fileQuit()), QKeySequence::Quit);

	editMenu = menuBar()->addMenu("&Edit");
	editMenu->addAction(QIcon::fromTheme("edit-undo"), "Undo", this, SLOT(editUndo()), QKeySequence::Undo);
	editMenu->addAction(QIcon::fromTheme("edit-redo"), "Redo", this, SLOT(editRedo()), QKeySequence::Redo);
	editMenu->addSeparator();
	editMenu->addAction(QIcon::fromTheme("edit-copy"), "&Copy", this, SLOT(editCopy()), QKeySequence::Copy);
	editMenu->addAction(QIcon::fromTheme("edit-cut"), "Cu&t", this, SLOT(editCut()), QKeySequence::Cut);
	editMenu->addAction(QIcon::fromTheme("edit-paste"), "&Paste", this, SLOT(editPaste()), QKeySequence::Paste);
	editMenu->addAction(QIcon::fromTheme("edit-clear"), "Clear", this, SLOT(editClear()), QKeySequence::Delete);
	editMenu->addSeparator();
	editMenu->addAction(QIcon::fromTheme("edit-select-all"), "Select All", this, SLOT(editSelectAll()), QKeySequence::SelectAll);
	editMenu->addAction("Select Track", this, SLOT(editSelectTrack()), Qt::CTRL + Qt::Key_T);
	editMenu->addAction("Select Row", this, SLOT(editSelectRow()));
	editMenu->addSeparator();
	editMenu->addAction("Bias Selection", this, SLOT(editBiasSelection()), Qt::CTRL + Qt::Key_B);
	editMenu->addSeparator();
	editMenu->addAction("Set Rows", this, SLOT(editSetRows()), Qt::CTRL + Qt::Key_R);
	editMenu->addSeparator();
	editMenu->addAction("Previous Bookmark", this, SLOT(editPreviousBookmark()), Qt::ALT + Qt::Key_PageUp);
	editMenu->addAction("Next Bookmark", this, SLOT(editNextBookmark()), Qt::ALT + Qt::Key_PageDown);
}

void MainWindow::createStatusBar()
{
	statusPos = new QLabel;
	statusValue = new QLabel;
	statusKeyType = new QLabel;

	statusBar()->addPermanentWidget(statusPos);
	statusBar()->addPermanentWidget(statusValue);
	statusBar()->addPermanentWidget(statusKeyType);

	statusBar()->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

	setStatusText("Not connected");
	setStatusPosition(0, 0);
	setStatusValue(0.0f, false);
	setStatusKeyType(SyncTrack::TrackKey::STEP, false);
}

static QStringList getRecentFiles()
{
#ifdef Q_OS_WIN32
	QSettings settings("HKEY_CURRENT_USER\\Software\\GNU Rocket",
	                   QSettings::NativeFormat);
#else
	QSettings settings;
#endif
	QStringList list;
	for (int i = 0; i < 5; ++i) {
		QVariant string = settings.value(QString("RecentFile%1").arg(i));
		if (string.isValid())
			list.push_back(string.toString());
	}
	return list;
}

static void setRecentFiles(const QStringList &files)
{
#ifdef Q_OS_WIN32
	QSettings settings("HKEY_CURRENT_USER\\Software\\GNU Rocket",
	                   QSettings::NativeFormat);
#else
	QSettings settings;
#endif

	for (int i = 0; i < files.size(); ++i)
		settings.setValue(QString("RecentFile%1").arg(i), files[i]);

	// remove keys not in the list
	for (int i = files.size(); ;++i) {
		QString key = QString("RecentFile%1").arg(i);

		if (!settings.contains(key))
			break;

		settings.remove(key);
	}
}

void MainWindow::updateRecentFiles()
{
	QStringList files = getRecentFiles();

	if (!files.size()) {
		recentFilesMenu->setEnabled(false);
		return;
	}

	Q_ASSERT(files.size() <= 5);
	for (int i = 0; i < files.size(); ++i) {
		QFileInfo info(files[i]);
		QString text = QString("&%1 %2").arg(i + 1).arg(info.fileName());

		recentFileActions[i]->setText(text);
		recentFileActions[i]->setData(info.absoluteFilePath());
		recentFileActions[i]->setVisible(true);
	}
	for (int i = files.size(); i < 5; ++i)
		recentFileActions[i]->setVisible(false);
	recentFilesMenu->setEnabled(true);
}

void MainWindow::setCurrentFileName(const QString &fileName)
{
	QFileInfo info(fileName);

	QStringList files = getRecentFiles();
	files.removeAll(info.absoluteFilePath());
	files.prepend(info.absoluteFilePath());
	while (files.size() > 5)
		files.removeLast();
	setRecentFiles(files);

	updateRecentFiles();
	setWindowFilePath(fileName);
}

void MainWindow::setStatusText(const QString &text)
{
	statusBar()->showMessage(text);
}

void MainWindow::setStatusPosition(int col, int row)
{
	statusPos->setText(QString("Row %1, Col %2").arg(row).arg(col));
}

void MainWindow::setStatusValue(double val, bool valid)
{
	if (valid)
		statusValue->setText(QString::number(val, 'f', 3));
	else
		statusValue->setText("---");
}

void MainWindow::setStatusKeyType(SyncTrack::TrackKey::KeyType keyType, bool valid)
{
	if (!valid) {
		statusKeyType->setText("---");
		return;
	}

	switch (keyType) {
	case SyncTrack::TrackKey::STEP:   statusKeyType->setText("step"); break;
	case SyncTrack::TrackKey::LINEAR: statusKeyType->setText("linear"); break;
	case SyncTrack::TrackKey::SMOOTH: statusKeyType->setText("smooth"); break;
	case SyncTrack::TrackKey::RAMP:   statusKeyType->setText("ramp"); break;
	default: Q_ASSERT(false);
	}
}

void MainWindow::setDocument(SyncDocument *newDoc)
{
	if (doc)
		QObject::disconnect(doc, SIGNAL(modifiedChanged(bool)),
		                    this, SLOT(setWindowModified(bool)));

	if (doc && clientSocket.connected()) {
		// delete old key frames
		for (size_t i = 0; i < doc->getTrackCount(); ++i) {
			SyncTrack *t = doc->getTrack(i);
			QMap<int, SyncTrack::TrackKey> keyMap = t->getKeyMap();
			QMap<int, SyncTrack::TrackKey>::const_iterator it;
			for (it = keyMap.constBegin(); it != keyMap.constEnd(); ++it)
				t->removeKey(it.key());

			QObject::disconnect(t, SIGNAL(keyFrameAdded(const SyncTrack &, int)),
			                    &clientSocket, SLOT(onKeyFrameAdded(const SyncTrack &, int)));
			QObject::disconnect(t, SIGNAL(keyFrameChanged(const SyncTrack &, int, const SyncTrack::TrackKey &)),
			                    &clientSocket, SLOT(onKeyFrameChanged(const SyncTrack &, int, const SyncTrack::TrackKey &)));
			QObject::disconnect(t, SIGNAL(keyFrameRemoved(const SyncTrack &, int)),
			                    &clientSocket, SLOT(onKeyFrameRemoved(const SyncTrack &, int)));
		}

		if (newDoc) {
			// add back missing client-tracks
			QMap<QString, size_t>::const_iterator it;
			for (it = clientSocket.clientTracks.begin(); it != clientSocket.clientTracks.end(); ++it) {
				SyncTrack *t = newDoc->findTrack(it.key());
				if (!t)
					newDoc->createTrack(it.key());
			}

			for (size_t i = 0; i < newDoc->getTrackCount(); ++i) {
				SyncTrack *t = newDoc->getTrack(i);
				QMap<int, SyncTrack::TrackKey> keyMap = t->getKeyMap();
				QMap<int, SyncTrack::TrackKey>::const_iterator it;
				for (it = keyMap.constBegin(); it != keyMap.constEnd(); ++it)
					clientSocket.sendSetKeyCommand(t->name.toUtf8().constData(), *it);
				QObject::connect(t, SIGNAL(keyFrameAdded(const SyncTrack &, int)),
				                 &clientSocket, SLOT(onKeyFrameAdded(const SyncTrack &, int)));
				QObject::connect(t, SIGNAL(keyFrameChanged(const SyncTrack &, int, const SyncTrack::TrackKey &)),
				                 &clientSocket, SLOT(onKeyFrameChanged(const SyncTrack &, int, const SyncTrack::TrackKey &)));
				QObject::connect(t, SIGNAL(keyFrameRemoved(const SyncTrack &, int)),
				                 &clientSocket, SLOT(onKeyFrameRemoved(const SyncTrack &, int)));
			}
		}
	}

	// recreate empty set of trackViews
	while (trackViews.count() > 0) {
		TrackView *trackView = trackViews.front();
		trackViews.removeFirst();
		delete trackView;
	}
	trackViews.clear();
	defaultTrackView = addTrackView(newDoc->getDefaultSyncPage());

	for (int i = 0; i < newDoc->getSyncPageCount(); ++i)
		addTrackView(newDoc->getSyncPage(i));

	if (doc)
		delete doc;
	doc = newDoc;

	QObject::connect(newDoc, SIGNAL(modifiedChanged(bool)),
	                 this, SLOT(setWindowModified(bool)));

	currentTrackView->dirtyCurrentValue();
	currentTrackView->viewport()->update();
}

void MainWindow::fileNew()
{
	setDocument(new SyncDocument);
	setWindowFilePath("Untitled");
}

bool MainWindow::loadDocument(const QString &path)
{
	SyncDocument *newDoc = SyncDocument::load(path);
	if (newDoc) {
		// set new document
		setDocument(newDoc);
		setCurrentFileName(path);
		return true;
	}
	return false;
}

void MainWindow::fileOpen()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "ROCKET File (*.rocket);;All Files (*.*)");
	if (fileName.length()) {
		loadDocument(fileName);
	}
}

void MainWindow::fileSaveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Save File", "", "ROCKET File (*.rocket);;All Files (*.*)");
	if (fileName.length()) {
		if (doc->save(fileName)) {
			clientSocket.sendSaveCommand();
			setCurrentFileName(fileName);
			doc->fileName = fileName;
		}
	}
}

void MainWindow::fileSave()
{
	if (doc->fileName.isEmpty())
		return fileSaveAs();

	if (!doc->save(doc->fileName))
		clientSocket.sendSaveCommand();
}

void MainWindow::fileRemoteExport()
{
	clientSocket.sendSaveCommand();
}

void MainWindow::openRecentFile()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (action) {
		QString fileName = action->data().toString();
		if (!loadDocument(fileName)) {
			QStringList files = getRecentFiles();
			files.removeAll(fileName);
			setRecentFiles(files);
			updateRecentFiles();
		}
	}
}

void MainWindow::fileQuit()
{
	if (doc->isModified()) {
		QMessageBox::StandardButton res = QMessageBox::question(
		    this, "GNU Rocket", "Save before exit?",
		    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		if (res == QMessageBox::Yes) {
			fileSave();
			QApplication::quit();
		} else if (res == QMessageBox::No)
			QApplication::quit();
	}
	else QApplication::quit();
}

void MainWindow::editUndo()
{
	if (!doc->canUndo())
		QApplication::beep();
	else
		doc->undo();

	// unfortunately, we don't know how much to invalidate... so we'll just invalidate it all.
	currentTrackView->viewport()->update();
}

void MainWindow::editRedo()
{
	if (!doc->canRedo())
		QApplication::beep();
	else
		doc->redo();

	// unfortunately, we don't know how much to invalidate... so we'll just invalidate it all.
	currentTrackView->viewport()->update();
}

void MainWindow::editCopy()
{
	currentTrackView->editCopy();
}

void MainWindow::editCut()
{
	currentTrackView->editCut();
}

void MainWindow::editPaste()
{
	currentTrackView->editPaste();
}

void MainWindow::editClear()
{
	currentTrackView->editClear();
}

void MainWindow::editSelectAll()
{
	currentTrackView->selectAll();
}

void MainWindow::editSelectTrack()
{
	currentTrackView->selectTrack();
}

void MainWindow::editSelectRow()
{
	currentTrackView->selectRow();
}

void MainWindow::editBiasSelection()
{
	bool ok = false;
	float bias = QInputDialog::getDouble(this, "Bias Selection", "", 0, INT_MIN, INT_MAX, 1, &ok);
	if (ok)
		currentTrackView->editBiasValue(bias);
}

void MainWindow::editSetRows()
{
	bool ok = false;
	int rows = QInputDialog::getInt(this, "Set Rows", "", currentTrackView->getRows(), 0, INT_MAX, 1, &ok);
	if (ok)
		currentTrackView->setRows(rows);
}

void MainWindow::editPreviousBookmark()
{
	int row = doc->prevRowBookmark(currentTrackView->getEditRow());
	if (row >= 0)
		currentTrackView->setEditRow(row);
}

void MainWindow::editNextBookmark()
{
	int row = doc->nextRowBookmark(currentTrackView->getEditRow());
	if (row >= 0)
		currentTrackView->setEditRow(row);
}

void MainWindow::onPosChanged(int col, int row)
{
	setStatusPosition(col, row);
	// YUCK: need to get paused state out of TrackView
	if (clientSocket.paused() && clientSocket.connected())
		clientSocket.sendSetRowCommand(row);
}

void MainWindow::onCurrValDirty()
{
	if (doc && doc->getTrackCount() > 0) {
		const SyncTrack *t = currentTrackView->page->getTrack(currentTrackView->getEditTrack());
		int row = currentTrackView->getEditRow();

		setStatusValue(t->getValue(row), true);

		const SyncTrack::TrackKey *k = t->getPrevKeyFrame(row);
		if (k)
			setStatusKeyType(k->type, true);
		else
			setStatusKeyType(SyncTrack::TrackKey::STEP, false);
	} else {
		setStatusValue(0.0f, false);
		setStatusKeyType(SyncTrack::TrackKey::STEP, false);
	}
}

TrackView *MainWindow::addTrackView(SyncPage *page)
{
	TrackView *trackView = new TrackView(page, NULL);

	trackViews.append(trackView);
	tabWidget->addTab(trackView, page->getName());

	return trackView;
}

void MainWindow::setTrackView(TrackView *newTrackView)
{
	if (currentTrackView) {
		disconnect(currentTrackView, SIGNAL(posChanged(int, int)),
		           this,               SLOT(onPosChanged(int, int)));
/*		disconnect(currentTrackView, SIGNAL(posChanged(int, int)),
		           clientSocket,       SLOT(onPosChanged(int, int))); */
		disconnect(currentTrackView, SIGNAL(pauseChanged(bool)),
		           &clientSocket,      SLOT(onPauseChanged(bool)));
		disconnect(currentTrackView, SIGNAL(currValDirty()),
		           this,               SLOT(onCurrValDirty()));
	}

	currentTrackView = newTrackView;

	if (currentTrackView) {
		connect(currentTrackView, SIGNAL(posChanged(int, int)),
		        this,               SLOT(onPosChanged(int, int)));
/*		connect(currentTrackView, SIGNAL(posChanged(int, int)),
		        clientSocket,       SLOT(onPosChanged(int, int))); */
		connect(currentTrackView, SIGNAL(pauseChanged(bool)),
		        &clientSocket,      SLOT(onPauseChanged(bool)));
		connect(currentTrackView, SIGNAL(currValDirty()),
		        this,               SLOT(onCurrValDirty()));
	}
}

void MainWindow::onTabChanged(int index)
{
	setTrackView(index < 0 ? NULL : trackViews[index]);
}

void MainWindow::processCommand(ClientSocket &sock)
{
	unsigned char cmd = 0;
	if (sock.recv((char*)&cmd, 1)) {
		switch (cmd) {
		case GET_TRACK:
			processGetTrack(sock);
			break;

		case SET_ROW:
			processSetRow(sock);
			break;
		}
	}
}

void MainWindow::processGetTrack(ClientSocket &sock)
{
	// read data
	int strLen;
	sock.recv((char *)&strLen, sizeof(int));
	strLen = qFromBigEndian((quint32)strLen);
	if (!sock.connected())
		return;

	if (!strLen) {
		sock.disconnect();
		currentTrackView->update();
		return;
	}

	QByteArray trackNameBuffer;
	trackNameBuffer.resize(strLen);
	if (!sock.recv(trackNameBuffer.data(), strLen))
		return;

	if (trackNameBuffer.contains('\0')) {
		sock.disconnect();
		currentTrackView->update();
		return;
	}

	QString trackName = QString::fromUtf8(trackNameBuffer);

	// find track
	const SyncTrack *t = doc->findTrack(trackName.toUtf8());
	if (!t)
		t = doc->createTrack(trackName);

	// hook up signals to slots
	QObject::connect(t, SIGNAL(keyFrameAdded(const SyncTrack &, int)),
	                 &clientSocket, SLOT(onKeyFrameAdded(const SyncTrack &, int)));
	QObject::connect(t, SIGNAL(keyFrameChanged(const SyncTrack &, int, const SyncTrack::TrackKey &)),
	                 &clientSocket, SLOT(onKeyFrameChanged(const SyncTrack &, int, const SyncTrack::TrackKey &)));
	QObject::connect(t, SIGNAL(keyFrameRemoved(const SyncTrack &, int)),
	                 &clientSocket, SLOT(onKeyFrameRemoved(const SyncTrack &, int)));

	// setup remap
	clientSocket.clientTracks[trackName] = clientIndex++;

	// send key frames
	QMap<int, SyncTrack::TrackKey> keyMap = t->getKeyMap();
	QMap<int, SyncTrack::TrackKey>::const_iterator it;
	for (it = keyMap.constBegin(); it != keyMap.constEnd(); ++it)
		clientSocket.sendSetKeyCommand(t->name.toUtf8().constData(), *it);

	currentTrackView->update();
}

void MainWindow::processSetRow(ClientSocket &sock)
{
	int newRow;
	sock.recv((char*)&newRow, sizeof(int));
	currentTrackView->setEditRow(qToBigEndian((quint32)newRow));
}

static TcpSocket *clientConnect(QTcpServer *serverSocket, QHostAddress *host)
{
	QTcpSocket *clientSocket = serverSocket->nextPendingConnection();
	Q_ASSERT(clientSocket != NULL);

	QByteArray line;

	// Read greetings or WebSocket upgrade
	// command from the socket
	for (;;) {
		char ch;
		if (!clientSocket->getChar(&ch)) {
			// Read failed; wait for data and try again
			clientSocket->waitForReadyRead();
			if(!clientSocket->getChar(&ch)) {
				clientSocket->close();
				return NULL;
			}
		}

		if (ch == '\n')
			break;
		if (ch != '\r')
			line.push_back(ch);
		if (ch == '!')
			break;
	}

	TcpSocket *ret = NULL;
	if (line.startsWith("GET ")) {
		ret = WebSocket::upgradeFromHttp(clientSocket);
		line.resize(strlen(CLIENT_GREET));
		if (!ret || !ret->recv(line.data(), line.size())) {
			clientSocket->close();
			return NULL;
		}
	} else
		ret = new TcpSocket(clientSocket);

	if (!line.startsWith(CLIENT_GREET) ||
	    !ret->send(SERVER_GREET, strlen(SERVER_GREET), true)) {
		ret->disconnect();
		return NULL;
	}

	if (NULL != host)
		*host = clientSocket->peerAddress();
	return ret;
}

void MainWindow::onReadyRead()
{
	while (clientSocket.pollRead())
		processCommand(clientSocket);
}

void MainWindow::onNewConnection()
{
	if (!clientSocket.connected()) {
		setStatusText("Accepting...");
		QHostAddress client;
		TcpSocket *socket = clientConnect(serverSocket, &client);
		if (socket) {
			setStatusText(QString("Connected to %1").arg(client.toString()));
			clientSocket.socket = socket;
			connect(socket->socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
			connect(socket->socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

			clientIndex = 0;
			clientSocket.setPaused(true);
			clientSocket.sendSetRowCommand(currentTrackView->getEditRow());
		} else
			setStatusText(QString("Not Connected: %1").arg(serverSocket->errorString()));
	}
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Space:
		if (clientSocket.connected()) {
			int paused = !clientSocket.paused();
			clientSocket.setPaused(paused);
			for (int i = 0; i < trackViews.count(); ++i)
				trackViews[i]->readOnly = !paused;
		}
		event->accept();
		return;
	}
}

void MainWindow::onDisconnected()
{
	clientSocket.setPaused(true);
	clientSocket.disconnect();

	// disconnect track-signals
	for (size_t i = 0; i < doc->getTrackCount(); ++i) {
		SyncTrack *t = doc->getTrack(i);
		QObject::disconnect(t, SIGNAL(keyFrameAdded(const SyncTrack &, int)),
		                    &clientSocket, SLOT(onKeyFrameAdded(const SyncTrack &, int)));
		QObject::disconnect(t, SIGNAL(keyFrameChanged(const SyncTrack &, int, const SyncTrack::TrackKey &)),
		                    &clientSocket, SLOT(onKeyFrameChanged(const SyncTrack &, int, const SyncTrack::TrackKey &)));
		QObject::disconnect(t, SIGNAL(keyFrameRemoved(const SyncTrack &, int)),
		                    &clientSocket, SLOT(onKeyFrameRemoved(const SyncTrack &, int)));
	}

	currentTrackView->update();
	setStatusText("Not Connected.");
}
