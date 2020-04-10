#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebEngineView>

class WebView : public QWebEngineView
{
    Q_OBJECT

public:
    WebView(QWidget *parent = Q_NULLPTR);

Q_SIGNALS:
    void broadcastPlay();
    void broadcastStop();

public:
    enum MediaState {
        MediaStateStopped = 0,
        MediaStatePlaying = 1,
        MediaStatePaused = 2,
        MediaStateConnecting = 3,
        MediaStateBuffering = 4,
        MediaStateFinished = 5,
        MediaStateError = 6,
    };
    void OipfAVControlObjectPlayState(const int state);
    void OipfAVControlObjectSpeed(const int speed);
    void OipfAVControlObjectPlayPosition(const quint32 pos);
    void OipfAVControlObjectPlayTime(const quint32 len);
    void OipfAVControlObjectError(const int error);

Q_SIGNALS:
    void mediaPlay(const QString &url);
    void mediaPause();
    void mediaContinue();
    void mediaStop();
    void mediaSeek(const quint32 &pos);
    void mediaRect(const QRect &rect);

private:
    QString m_mediaUrl;
    int m_mediaState;

public Q_SLOTS:
    void sendKeyEvent(const int &keyCode);

protected Q_SLOTS:
    void titleChanged(const QString &title);
    void loadFinished(bool ok);
};

#endif // WEBVIEW_H
