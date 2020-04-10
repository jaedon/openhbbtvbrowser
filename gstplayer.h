#ifndef GSTPLAYER_H
#define GSTPLAYER_H

#include <QObject>
#include <QTimer>
#include <gst/gst.h>

class GstPlayer : public QObject
{
    Q_OBJECT

public:
    enum GstState {
        GstStateStop,
        GstStatePlay,
        GstStatePause,
    };

    GstPlayer(QObject *parent = Q_NULLPTR);
    bool mediaPlay(const QString &url);
    bool mediaPause();
    bool mediaContinue();
    bool mediaStop();
    bool mediaSeek(const quint32 &pos);
    bool mediaRect(const QRect &rect);

Q_SIGNALS:
    void mediaStopChanged();
    void mediaDurationChanged(const quint32 &len);
    void mediaPositionChanged(const quint32 &pos);

private Q_SLOTS:
    void mediaTimeUpdate();

private:
    QString pipeline_description;
    GstElement *pipeline;
    GstBus *bus;
    GstMessage *msg;
    int m_state;
    QTimer *m_timeUpdate;
    quint32 m_position;
    quint32 m_duration;
};

#endif // GSTPLAYER_H
