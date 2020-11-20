#ifndef GAMEMUSIC_H
#define GAMEMUSIC_H

#include <QtCore/QObject>
#include <QtCore/qglobal.h>

#include <QMutex>

#include <QMediaPlayer>
#include <QMediaPlaylist>

class GameMusic : public QObject
{
    Q_OBJECT
public:
    explicit GameMusic(QObject *parent = 0);

    static GameMusic *Instance()
    {
        static QMutex mutex;

        if (!__self__) {
            QMutexLocker locker(&mutex);

            if (!__self__) {
                __self__ = new GameMusic;
            }
        }

        return __self__;
    }

    void play(const QString &name);
    // 初始化背景音乐
    void initMusic();
signals:

public slots:

private:
    static GameMusic *__self__;

    // 短音
    QMediaPlayer *m_playerShot;
    // 背景音乐
    QMediaPlayer *m_backgroundPlayer;
    // 背景音乐播放列表
    QMediaPlaylist *musicPlaylist;
private:
    // 音频文件检测，如果没有则从资源文件中拷贝出来
    void addMusicSource(const QString &name);

};

#endif // GAMEMUSIC_H
