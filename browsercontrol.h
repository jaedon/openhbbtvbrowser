#ifndef BROWSERCONTROL_H
#define BROWSERCONTROL_H

#include <QApplication>
#include <QLocalSocket>
#include <QObject>
#include <QSocketNotifier>

class BrowserWindow;

struct input_event {
    long pad1;
    long pad2;
    quint16 type;
    quint16 code;
    qint32 value;
};
#define KEY_1			2
#define KEY_2			3
#define KEY_3			4
#define KEY_4			5
#define KEY_5			6
#define KEY_6			7
#define KEY_7			8
#define KEY_8			9
#define KEY_9			10
#define KEY_0			11
#define KEY_UP			103
#define KEY_LEFT		105
#define KEY_RIGHT		106
#define KEY_DOWN		108
#define KEY_MUTE		113
#define KEY_VOLUMEDOWN	114
#define KEY_VOLUMEUP	115
#define KEY_PAUSE		119
#define KEY_STOP		128
#define KEY_MENU		139
#define KEY_REWIND		168
#define KEY_EXIT		174
#define KEY_PLAY		207
#define KEY_FASTFORWARD	208
#define KEY_OK			0x160
#define KEY_RED			0x18e
#define KEY_GREEN		0x18f
#define KEY_YELLOW		0x190
#define KEY_BLUE		0x191

enum VirtualKey {
    // Based on CEA-2014-A CE-HTML Annex F
    VK_UNDEFINED = 0,
    VK_ENTER = 13,
    VK_LEFT = 37,
    VK_UP = 38,
    VK_RIGHT = 39,
    VK_DOWN = 40,
    VK_SPACE = 32,
    VK_BACK_SPACE = 8,
    VK_0 = 48,
    VK_1 = 49,
    VK_2 = 50,
    VK_3 = 51,
    VK_4 = 52,
    VK_5 = 63,
    VK_6 = 54,
    VK_7 = 55,
    VK_8 = 56,
    VK_9 = 57,
    VK_A = 65,
    VK_B = 66,
    VK_C = 67,
    VK_D = 68,
    VK_E = 69,
    VK_F = 70,
    VK_G = 71,
    VK_H = 72,
    VK_I = 73,
    VK_J = 74,
    VK_K = 75,
    VK_L = 76,
    VK_M = 77,
    VK_N = 78,
    VK_O = 79,
    VK_P = 80,
    VK_Q = 81,
    VK_R = 82,
    VK_S = 83,
    VK_T = 84,
    VK_U = 85,
    VK_V = 86,
    VK_W = 87,
    VK_X = 88,
    VK_Y = 89,
    VK_Z = 90,
    VK_RED = 403,
    VK_GREEN = 404,
    VK_YELLOW = 405,
    VK_BLUE = 406,
    VK_HELP = 156,
    VK_SEARCH = 112,
    VK_AUDIODESCRIPTION = 113,
    VK_HD = 114,
    VK_PLAY = 415,
    VK_PAUSE = 19,
    VK_PLAY_PAUSE = 402,
    VK_STOP = 413,
    VK_PREV = 424,
    VK_NEXT = 425,
    VK_FAST_FWD = 417,
    VK_REWIND = 412,
    VK_INFO = 457,
    VK_SUBTITLE = 460,
    VK_BACK = 461,
    VK_VOLUME_UP = 447,
    VK_VOLUME_DOWN = 448,
    VK_MUTE = 449,
};

class RemoteController : public QObject
{
    Q_OBJECT

public:
    RemoteController(const QString &device = QString("/dev/input/event0"));
    ~RemoteController();

Q_SIGNALS:
    void activate(const int &keyCode);
    void volumeMute();
    void volumeDown();
    void volumeUp();

protected Q_SLOTS:
    void readKeycode();

private:
    int m_fd;
    QSocketNotifier *m_notify;
};

class WindowEventFilter : public QObject
{
    Q_OBJECT

public:
    WindowEventFilter(QObject *parent = Q_NULLPTR);

Q_SIGNALS:
    void activate(const int &keyCode);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

class CommandClient : public QObject
{
    Q_OBJECT

public:
    enum BrowserCommand {
        CommandUrlChange = 1,
        CommandSIChange = 2,
        CommandAITChange = 3,
        CommandStopChange = 4,
        CommandTimeChange = 5,

        CommandBroadcastPlay = 1001,
        CommandBroadcastStop = 1002,
        CommandVolumeMute = 1003,
        CommandVolumeDown = 1004,
        CommandVolumeUp = 1005,
        CommandExit = 1006,
        CommandMediaPlay = 1011,
        CommandMediaPause = 1012,
        CommandMediaContinue = 1013,
        CommandMediaStop = 1014,
        CommandMediaSeek = 1015,
        CommandMediaRect = 1016,
    };

    CommandClient(const QString &sockFile = QString("/tmp/.sock.browser"));
    bool writeCommand(int command);
    bool writeCommand(int command, const QString &data);
    bool writeCommand(int command, const int &pos);
    bool writeCommand(int command, const QRect &rect);

Q_SIGNALS:
    void setUrl(const QString &url);
    void setSIData(const quint32 &pmt, const quint32 &tsid, const quint32 &onid, const quint32 &ssid, const quint32 &chantype, const quint32 &chanid);
    void setAITData();
    void setMediaStop();
    void setMediaDuration(const quint32 &len);
    void setMediaPosition(const quint32 &pos);

protected Q_SLOTS:
    void readCommand();

private:
    QLocalSocket *m_socket;
};

#endif // BROWSERCONTROL_H
