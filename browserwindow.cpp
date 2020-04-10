#include "browsercontrol.h"
#include "browserwindow.h"
#include "gstplayer.h"
#include "webpage.h"
#include "webview.h"
#include <QWebEngineProfile>

BrowserWindow::BrowserWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , m_commandClient(new CommandClient)
    , m_gstPlayer(nullptr)
    , m_webView(new WebView(this))
{
    WebPage *page = new WebPage(QWebEngineProfile::defaultProfile(), m_webView);
    m_webView->setPage(page);
    m_webView->show();

    setCentralWidget(m_webView);

    connect(m_webView, &WebView::broadcastPlay, this, &BrowserWindow::broadcastPlay);
    connect(m_webView, &WebView::broadcastStop, this, &BrowserWindow::broadcastStop);

    connect(m_commandClient, &CommandClient::setUrl, this, &BrowserWindow::setUrl);
    connect(m_commandClient, &CommandClient::setSIData, this, &BrowserWindow::setSIData);
    connect(m_commandClient, &CommandClient::setAITData, this, &BrowserWindow::setAITData);

    connect(m_webView, &WebView::mediaPlay, this, &BrowserWindow::mediaPlay);
    connect(m_webView, &WebView::mediaPause, this, &BrowserWindow::mediaPause);
    connect(m_webView, &WebView::mediaContinue, this, &BrowserWindow::mediaContinue);
    connect(m_webView, &WebView::mediaStop, this, &BrowserWindow::mediaStop);
    connect(m_webView, &WebView::mediaSeek, this, &BrowserWindow::mediaSeek);
    connect(m_webView, &WebView::mediaRect, this, &BrowserWindow::mediaRect);
    connect(m_commandClient, &CommandClient::setMediaStop, this, &BrowserWindow::mediaStop);
    connect(m_commandClient, &CommandClient::setMediaDuration, this, &BrowserWindow::mediaDuration);
    connect(m_commandClient, &CommandClient::setMediaPosition, this, &BrowserWindow::mediaPosition);

    if (QCoreApplication::arguments().contains(QStringLiteral("--use-internal-gstplayer")))
        m_gstPlayer = new GstPlayer();

    if (m_gstPlayer)  {
        connect(m_gstPlayer, &GstPlayer::mediaStopChanged, this, &BrowserWindow::mediaStop);
        connect(m_gstPlayer, &GstPlayer::mediaDurationChanged, this, &BrowserWindow::mediaDuration);
        connect(m_gstPlayer, &GstPlayer::mediaPositionChanged, this, &BrowserWindow::mediaPosition);
     }
}

WebView *BrowserWindow::webView()
{
    return m_webView;
}

void BrowserWindow::broadcastPlay()
{
    m_commandClient->writeCommand(CommandClient::CommandBroadcastPlay);
}

void BrowserWindow::broadcastStop()
{
    m_commandClient->writeCommand(CommandClient::CommandBroadcastStop);
}

void BrowserWindow::volumeMute()
{
    m_commandClient->writeCommand(CommandClient::CommandVolumeMute);
}

void BrowserWindow::volumeDown()
{
    m_commandClient->writeCommand(CommandClient::CommandVolumeDown);
}

void BrowserWindow::volumeUp()
{
    m_commandClient->writeCommand(CommandClient::CommandVolumeUp);
}

void BrowserWindow::setUrl(const QString &url)
{
    m_webView->setUrl(url);
}

void BrowserWindow::setSIData(const quint32 &pmt,
                              const quint32 &tsid,
                              const quint32 &onid,
                              const quint32 &ssid,
                              const quint32 &chantype,
                              const quint32 &chanid)
{
    Q_UNUSED(pmt);
    Q_UNUSED(tsid);
    Q_UNUSED(onid);
    Q_UNUSED(ssid);
    Q_UNUSED(chantype);
    Q_UNUSED(chanid);
}

void BrowserWindow::setAITData()
{
}

void BrowserWindow::mediaPlay(const QString &url)
{
    if (m_gstPlayer)
        m_gstPlayer->mediaPlay(url);
    else
        m_commandClient->writeCommand(CommandClient::CommandMediaPlay, url);

    m_webView->OipfAVControlObjectPlayState(WebView::MediaStatePlaying);
}

void BrowserWindow::mediaPause()
{
    if (m_gstPlayer)
        m_gstPlayer->mediaPause();
    else
        m_commandClient->writeCommand(CommandClient::CommandMediaPause);

    m_webView->OipfAVControlObjectPlayState(WebView::MediaStatePaused);
}

void BrowserWindow::mediaContinue()
{
    if (m_gstPlayer)
        m_gstPlayer->mediaContinue();
    else
        m_commandClient->writeCommand(CommandClient::CommandMediaContinue);

    m_webView->OipfAVControlObjectPlayState(WebView::MediaStatePlaying);
}

void BrowserWindow::mediaStop()
{
    if (m_gstPlayer)
        m_gstPlayer->mediaStop();
    else
        m_commandClient->writeCommand(CommandClient::CommandMediaStop);

    m_webView->OipfAVControlObjectPlayState(WebView::MediaStateStopped);
    m_webView->OipfAVControlObjectPlayTime(0);
    m_webView->OipfAVControlObjectSpeed(0);
}

void BrowserWindow::mediaSeek(const quint32 &pos)
{
    if (m_gstPlayer)
        m_gstPlayer->mediaSeek(pos);
    else
        m_commandClient->writeCommand(CommandClient::CommandMediaSeek, pos);
}

void BrowserWindow::mediaRect(const QRect &rect)
{
    if (m_gstPlayer)
        m_gstPlayer->mediaRect(rect);
    else
        m_commandClient->writeCommand(CommandClient::CommandMediaRect, rect);
}

void BrowserWindow::mediaDuration(const quint32 &len)
{
    m_webView->OipfAVControlObjectPlayTime(len);
}

void BrowserWindow::mediaPosition(const quint32 &pos)
{
    m_webView->OipfAVControlObjectPlayPosition(pos);
}
