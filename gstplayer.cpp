#include "gstplayer.h"

GstPlayer::GstPlayer(QObject *parent)
    : QObject(parent)
    , m_timeUpdate(new QTimer)
{
    connect(m_timeUpdate, &QTimer::timeout, this, &GstPlayer::mediaTimeUpdate);
}

bool GstPlayer::mediaPlay(const QString &url)
{
    if (url.isEmpty())
        return false;

    gst_init(NULL, NULL);

    m_position = 0;
    m_duration = 0;

    if (m_state == GstStatePlay || m_state == GstStatePause)
        mediaStop();

    pipeline_description = QString::fromLatin1("playbin uri=%1").arg(url);

    pipeline = gst_parse_launch(pipeline_description.toStdString().c_str(), NULL);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    bus = gst_element_get_bus(pipeline);
    msg = NULL; //gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    if (m_state == GstStatePause)
        gst_element_set_state(pipeline, GST_STATE_PAUSED);
    else
        m_state = GstStatePlay;

    m_timeUpdate->start(1000);

    return true;
}

bool GstPlayer::mediaPause()
{
    gst_element_set_state(pipeline, GST_STATE_PAUSED);

    m_state = GstStatePause;

    return true;
}

bool GstPlayer::mediaContinue()
{
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    m_state = GstStatePlay;

    return true;
}

bool GstPlayer::mediaStop()
{
    m_timeUpdate->stop();

    if (!pipeline_description.isEmpty()) {
        if (msg != NULL)
            gst_message_unref(msg);
        gst_object_unref(bus);
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
        pipeline_description.clear();
    }

    m_state = GstStateStop;

    return true;
}

bool GstPlayer::mediaSeek(const quint32 &pos)
{
    gint64 time_nanoseconds = pos * 1000000;

    if (!gst_element_seek(pipeline, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
                          GST_SEEK_TYPE_SET, time_nanoseconds,
                          GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
        return false;

    return true;
}

void GstPlayer::mediaTimeUpdate()
{
    gint64 pos, len;

    if (gst_element_query_duration(pipeline, GST_FORMAT_TIME, &len)) {
        quint32 len_milliseconds;

        len_milliseconds = len / 1000000;

        if (m_duration != len_milliseconds) {
            m_duration = len_milliseconds;
            emit mediaDurationChanged(m_duration);
        }
    }

    if (gst_element_query_position(pipeline, GST_FORMAT_TIME, &pos)) {
        quint32 pos_milliseconds;

        pos_milliseconds = pos / 1000000;

        if (m_position != pos_milliseconds) {
            m_position = pos_milliseconds;
            emit mediaPositionChanged(m_position);
        }
    }
}

bool GstPlayer::mediaRect(const QRect &rect)
{
    Q_UNUSED(rect);
    return true;
}
