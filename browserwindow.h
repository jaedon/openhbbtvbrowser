#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QMainWindow>
#include <QWebSocketServer>
#include <QWebSocket>

class CommandClient;
class WebView;

class BrowserWindow : public QMainWindow
{
    Q_OBJECT

public:
    BrowserWindow(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::Widget);
    WebView *webView();

protected Q_SLOTS:
    void newConnection();
    void processTextMessage(QString message);
    void socketDisconnected();
    void broadcastPlay();
    void broadcastStop();

private:
    CommandClient *m_commandClient;
    QWebSocketServer *m_webSocketServer;
    QList<QWebSocket *> m_clients;
    WebView *m_webView;
};

#endif // BROWSERWINDOW_H
