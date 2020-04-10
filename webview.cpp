#include "webview.h"
#include <QWebEngineScript>

WebView::WebView(QWidget *parent)
    : QWebEngineView(parent)
    , m_mediaState(MediaStateStopped)
{
    connect(this, &QWebEngineView::titleChanged, this, &WebView::titleChanged);
    connect(this, &QWebEngineView::loadFinished, this, &WebView::loadFinished);
}

void WebView::titleChanged(const QString &title)
{
    if (title.startsWith("OipfVideoBroadcastMapper")) {
        emit broadcastPlay();
    } else if (title.startsWith("OipfAVControlMapper")) {
        emit broadcastStop();
    }

    if (title.startsWith("OipfAVControlObject.data=")) {
        m_mediaUrl = title.mid(25);
    } else if (title.startsWith("OipfAVControlObject.rect=")) {
        QStringList splitted = title.mid(25).split(",");
        int x1, x2, y1, y2;
        x1 = splitted.at(0).toInt();
        x2 = splitted.at(1).toInt();
        y1 = splitted.at(2).toInt();
        y2 = splitted.at(3).toInt();
        QRect rect(x1, y1, x2 - x1, y2 - y1);
        emit mediaRect(rect);
    } else if (title.startsWith("OipfAVControlObject.play=1")) {
        if (m_mediaState == MediaStateStopped)
            emit mediaPlay(m_mediaUrl);
        else if (m_mediaState == MediaStatePaused)
            emit mediaContinue();
        m_mediaState = MediaStatePlaying;
    } else if (title.startsWith("OipfAVControlObject.play=0")) {
        if (m_mediaState == MediaStatePlaying) {
            emit mediaPause();
            m_mediaState = MediaStatePaused;
        }
    } else if (title.startsWith("OipfAVControlObject.stop")) {
        if (m_mediaState == MediaStatePlaying || m_mediaState == MediaStatePaused) {
            emit mediaStop();
            m_mediaState = MediaStateStopped;
        }
    } else if (title.startsWith("OipfAVControlObject.seek=")) {
        quint32 pos = title.mid(21).toInt();
        emit mediaSeek(pos);
    }
}

void WebView::loadFinished(bool ok)
{
    if (ok) {
        if (size().width() == 1920 && size().height() == 1080) {
            QString s = QString::fromLatin1("document.body.style.setProperty('zoom', '150%');");
            page()->runJavaScript(s);
        }
    }
}

void WebView::sendKeyEvent(const int &keyCode)
{
    QString s = QString::fromLatin1("(function() {"
                                    "  document.dispatchEvent(new KeyboardEvent('keydown',{keyCode:%1}));"
                                    "})()").arg(keyCode);
    page()->runJavaScript(s);
}

void WebView::OipfAVControlObjectPlayState(const int state)
{
    QString s = QString::fromLatin1("(function() {"
                                    "  window.OipfAVControlObject.playState = %1;"
                                    "  window.OipfAVControlObject.dispatchEvent(new Event('PlayStateChange', {'state':%1}));"
                                    "})()").arg(state);
    page()->runJavaScript(s);
}

void WebView::OipfAVControlObjectSpeed(const int speed)
{
    QString s = QString::fromLatin1("(function() {"
                                    "  window.OipfAVControlObject.speed = %1;"
                                    "  window.OipfAVControlObject.dispatchEvent(new Event('PlaySpeedChanged', {'speed':%1}));"
                                    "})()").arg(speed);
    page()->runJavaScript(s);
}

void WebView::OipfAVControlObjectPlayPosition(const quint32 pos)
{
    QString s = QString::fromLatin1("(function() {"
                                    "  window.OipfAVControlObject.playPosition = %1;"
                                    "  window.OipfAVControlObject.dispatchEvent(new Event('PlayPositionChanged', {'position':%1}));"
                                    "})()").arg(pos);
    page()->runJavaScript(s);
}

void WebView::OipfAVControlObjectPlayTime(const quint32 len)
{
    QString s = QString::fromLatin1("(function() {"
                                    "  window.OipfAVControlObject.playTime = %1;"
                                    "  window.OipfAVControlObject.duration = %2;"
                                    "})()").arg(len).arg(len/1000);
    page()->runJavaScript(s);
}

void WebView::OipfAVControlObjectError(const int error)
{
    QString s = QString::fromLatin1("(function() {"
                                    "  window.OipfAVControlObject.error = %1;"
                                    "})()").arg(error);
    page()->runJavaScript(s);
}
