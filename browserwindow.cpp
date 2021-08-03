#include "browsercontrol.h"
#include "browserwindow.h"
#include "webpage.h"
#include "webview.h"
#include <QWebEngineProfile>

BrowserWindow::BrowserWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , m_commandClient(new CommandClient)
    , m_webSocketServer(new QWebSocketServer(QStringLiteral("OpenHbbTVBrowser"), QWebSocketServer::NonSecureMode, this))
    , m_webView(new WebView(this))
{
    WebPage *page = new WebPage(QWebEngineProfile::defaultProfile(), m_webView);
    m_webView->setPage(page);
    m_webView->show();

    setCentralWidget(m_webView);

    connect(m_webView, &WebView::broadcastPlay, this, &BrowserWindow::broadcastPlay);
    connect(m_webView, &WebView::broadcastStop, this, &BrowserWindow::broadcastStop);

    int port = 8086; // TODO: option

    if (m_webSocketServer->listen(QHostAddress::Any, port)) {
        qDebug() << "QWebSocketServer listening on port" << port;
        connect(m_webSocketServer, &QWebSocketServer::newConnection, this, &BrowserWindow::newConnection);
    }
}

WebView *BrowserWindow::webView()
{
    return m_webView;
}

void BrowserWindow::newConnection()
{
    QWebSocket *pSocket = m_webSocketServer->nextPendingConnection();

    connect(pSocket, &QWebSocket::textMessageReceived, this, &BrowserWindow::processTextMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &BrowserWindow::socketDisconnected);

    m_clients << pSocket;
}

void BrowserWindow::processTextMessage(QString message)
{
    qDebug() << "Message received:" << message;
}

void BrowserWindow::socketDisconnected()
{
    QWebSocket *socket = qobject_cast<QWebSocket *>(sender());

    qDebug() << "socketDisconnected:" << socket;
    if (socket) {
        m_clients.removeAll(socket);
        socket->deleteLater();
    }
}

void BrowserWindow::broadcastPlay()
{
    m_commandClient->writeCommand(CommandClient::CommandBroadcastPlay);
}

void BrowserWindow::broadcastStop()
{
    m_commandClient->writeCommand(CommandClient::CommandBroadcastStop);
}
