#include "browsercontrol.h"
#include "browserwindow.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <QKeyEvent>

RemoteController::RemoteController(const QString &device)
{
    m_fd = ::open(device.toLocal8Bit().constData(), O_RDONLY, 0);
    if (m_fd >= 0) {
        m_notify = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
        connect(m_notify, &QSocketNotifier::activated, this, &RemoteController::readKeycode);
    } else {
        qErrnoWarning("Cannot open keyboard input device '%ls'", qUtf16Printable(device));
    }
}

RemoteController::~RemoteController()
{
    if (m_fd >= 0)
        ::close(m_fd);
}

void RemoteController::readKeycode()
{
    struct ::input_event buffer[32];
    int n = 0;

    forever {
        int result = read(m_fd, reinterpret_cast<char *>(buffer) + n, sizeof(buffer) - n);

        if (result == 0) {
            qWarning("evdevkeyboard: Got EOF from the input device");
            return;
        } else if (result < 0) {
            if (errno != EINTR && errno != EAGAIN) {
                qErrnoWarning("evdevkeyboard: Could not read from input device");
                if (errno == ENODEV) {
                    delete m_notify;
                    m_notify = nullptr;
                    ::close(m_fd);
                    m_fd = -1;
                }
                return;
            }
        } else {
            n += result;
            if (n % sizeof(buffer[0]) == 0)
                break;
        }
    }

    n /= sizeof(buffer[0]);

    for (int i = 0; i < n; ++i) {
        if (buffer[i].type == 1 && buffer[i].value != 0) {
            int vk = VK_UNDEFINED;
            switch (buffer[i].code) {
            case KEY_RED:           vk = VK_RED; break;
            case KEY_GREEN:         vk = VK_GREEN; break;
            case KEY_YELLOW:        vk = VK_YELLOW; break;
            case KEY_BLUE:          vk = VK_BLUE; break;
            case KEY_LEFT:          vk = VK_LEFT; break;
            case KEY_UP:            vk = VK_UP; break;
            case KEY_RIGHT:         vk = VK_RIGHT; break;
            case KEY_DOWN:          vk = VK_DOWN; break;
            case KEY_OK:            vk = VK_ENTER; break;
            case KEY_EXIT:          vk = VK_BACK_SPACE; break;
            case KEY_0:             vk = VK_0; break;
            case KEY_1:             vk = VK_1; break;
            case KEY_2:             vk = VK_2; break;
            case KEY_3:             vk = VK_3; break;
            case KEY_4:             vk = VK_4; break;
            case KEY_5:             vk = VK_5; break;
            case KEY_6:             vk = VK_6; break;
            case KEY_7:             vk = VK_7; break;
            case KEY_8:             vk = VK_8; break;
            case KEY_9:             vk = VK_9; break;
            case KEY_PAUSE:         vk = VK_PAUSE; break;
            case KEY_PLAY:          vk = VK_PLAY; break;
            case KEY_STOP:          vk = VK_STOP; break;
            case KEY_FASTFORWARD:   vk = VK_FAST_FWD; break;
            case KEY_REWIND:        vk = VK_REWIND; break;
            case KEY_MUTE:          emit volumeMute(); break;
            case KEY_VOLUMEDOWN:    emit volumeDown(); break;
            case KEY_VOLUMEUP:      emit volumeUp(); break;
            case KEY_MENU:          QApplication::quit(); break; // TODO
            }
            if (vk != VK_UNDEFINED) emit activate(vk);
        }
    }
}

WindowEventFilter::WindowEventFilter(QObject *parent)
    : QObject(parent)
{
}

bool WindowEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        int vk = VK_UNDEFINED;
        switch (keyEvent->key()) {
        case Qt::Key_R:             vk = VK_RED; break;
        case Qt::Key_G:             vk = VK_GREEN; break;
        case Qt::Key_Y:             vk = VK_YELLOW; break;
        case Qt::Key_B:             vk = VK_BLUE; break;
        case Qt::Key_Left:          vk = VK_LEFT; break;
        case Qt::Key_Up:            vk = VK_UP; break;
        case Qt::Key_Right:         vk = VK_RIGHT; break;
        case Qt::Key_Down:          vk = VK_DOWN; break;
        case Qt::Key_Return:        vk = VK_ENTER; break;
        case Qt::Key_Backspace:     vk = VK_BACK_SPACE; break;
        case Qt::Key_0:             vk = VK_0; break;
        case Qt::Key_1:             vk = VK_1; break;
        case Qt::Key_2:             vk = VK_2; break;
        case Qt::Key_3:             vk = VK_3; break;
        case Qt::Key_4:             vk = VK_4; break;
        case Qt::Key_5:             vk = VK_5; break;
        case Qt::Key_6:             vk = VK_6; break;
        case Qt::Key_7:             vk = VK_7; break;
        case Qt::Key_8:             vk = VK_8; break;
        case Qt::Key_9:             vk = VK_9; break;
        case Qt::Key_K:             vk = VK_PAUSE; break;
        case Qt::Key_J:             vk = VK_PLAY; break;
        case Qt::Key_L:             vk = VK_STOP; break;
        case Qt::Key_P:             vk = VK_FAST_FWD; break;
        case Qt::Key_N:             vk = VK_REWIND; break;
        }
        if (vk != VK_UNDEFINED) emit activate(vk);
        return true;
    }

    return QObject::eventFilter(obj, event);
}

CommandClient::CommandClient(const QString &sockFile)
    : m_socket(new QLocalSocket(this))
{
    connect(m_socket, &QLocalSocket::readyRead, this, &CommandClient::readCommand);

    m_socket->abort();
    m_socket->connectToServer(sockFile);
}

void CommandClient::readCommand()
{
    while (!m_socket->atEnd()) {
        if (m_socket->bytesAvailable() < 12)
            return;

        quint32 magic, command, dataSize;

        QDataStream inStream(m_socket->read(12));
        inStream >> magic
                 >> command
                 >> dataSize;

        if (magic != 987654321)
            continue;

        char buf[dataSize + 1];
        if (dataSize)
            m_socket->read(buf, dataSize);
        buf[dataSize] = 0;

        switch (command) {
        case CommandUrlChange: {
            QString url(buf);
            emit setUrl(url);
            break;
        }

        case CommandSIChange: {
            QDataStream dataStream(buf);
            quint32 pmt, tsid, onid, ssid, chantype, chanid;
            dataStream >> pmt
                       >> tsid
                       >> onid
                       >> ssid
                       >> chantype
                       >> chanid;
            emit setSIData(pmt, tsid, onid, ssid, chantype, chanid);
            break;
        }

        case CommandAITChange:
            emit setAITData();
            break;

        case CommandStopChange:
            emit setMediaStop();
            break;

        case CommandTimeChange:
            QDataStream dataStream(buf);
            quint32 pos, len;
            dataStream >> len
                       >> pos;
            emit setMediaDuration(len);
            emit setMediaPosition(pos);
            break;
        }
    }
}

bool CommandClient::writeCommand(int command)
{
    if (!m_socket->isValid())
        return false;

    QDataStream outStream(m_socket);
    outStream << (qint32)987654321
              << (qint32)command
              << (qint32)0;
    return m_socket->waitForConnected(1000);
}

bool CommandClient::writeCommand(int command, const QString &data)
{
    if (!m_socket->isValid())
        return false;

    QDataStream outStream(m_socket);
    outStream << (qint32)987654321
              << (qint32)command
              << (qint32)data.size();
    m_socket->write(data.toStdString().c_str(), data.size());
    return m_socket->waitForConnected(1000);
}

bool CommandClient::writeCommand(int command, const int &pos)
{
    if (!m_socket->isValid())
        return false;

    QDataStream outStream(m_socket);
    outStream << (qint32)987654321
              << (qint32)command
              << (qint32)4
              << (qint32)pos;
    return m_socket->waitForConnected(1000);
}

bool CommandClient::writeCommand(int command, const QRect &rect)
{
    if (!m_socket->isValid())
        return false;

    QDataStream outStream(m_socket);
    outStream << (qint32)987654321
              << (qint32)command
              << (qint32)16
              << (qint32)rect.left()
              << (qint32)rect.top()
              << (qint32)rect.width()
              << (qint32)rect.height();
    return m_socket->waitForConnected(1000);
}
