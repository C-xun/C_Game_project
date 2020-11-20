#include "gamemusic.h"

#include <QUrl>
#include <QApplication>
#include <QDesktopWidget>

#include <QFile>
#include <QDir>

#define MYAPP_PATH     qApp->applicationDirPath()
#define SOUND_PATH     (MYAPP_PATH + "/sound/" )

GameMusic *GameMusic::__self__ = Q_NULLPTR;

const static char *s_pchMusicSrc[] = {
    "MusicEx_Normal", "MusicEx_Normal2", "MusicEx_Welcome",                 // 3
    "MusicEx_Exciting", "MusicEx_Win", "MusicEx_Lose",                      // 6
    "Special_give", "Special_plane", "Special_Bomb",                        // 9
    "Special_Multiply", "SpecSelectCard","Special_Dispatch",                // 12
    "Man_buyao1", "Man_buyao2", "Man_buyao3", "Man_buyao4",                 // 16
    "Man_sandaiyi", "Man_sandaiyidui", "Man_shunzi", "Man_liandui",         // 20
    "Man_feiji",  "Man_wangzha", "Man_zhadan",                              // 23
    "Man_Order", "Woman_Order", "Man_NoOrder",                              // 26
};

GameMusic::GameMusic(QObject *parent) : QObject(parent)
{
    m_playerShot = new QMediaPlayer(this);

    musicPlaylist = new QMediaPlaylist(this);
    m_backgroundPlayer = new QMediaPlayer(this) ;
    m_backgroundPlayer->setPlaylist(musicPlaylist);
}

void GameMusic::play(const QString &name)
{
    QString strSound = SOUND_PATH + name + ".ogg";
    if (!QFile::exists(strSound)) return;

    m_playerShot->setMedia(QUrl(strSound));
    m_playerShot->play();
}

void GameMusic::initMusic()
{
    // 首先判断是否存在音频目录，如果不存在创建
    QDir dir(SOUND_PATH);
    if (!dir.exists()) dir.mkdir(SOUND_PATH);

    // 添加音频文件
    for (int i = 0; i < (int)(sizeof(s_pchMusicSrc) / sizeof(char*)); i++) {
        addMusicSource(s_pchMusicSrc[i]);
    }

    // 添加牌的音频
    for (int i = 1; i < 14; i++) {
        addMusicSource(QString("Man_%1").arg(i));
        addMusicSource(QString("Man_dui%1").arg(i));
        addMusicSource(QString("Man_tuple%1").arg(i));
    }

    // 添加大小王
    addMusicSource(QString("Man_14"));
    addMusicSource(QString("Man_15"));

    // 添加背景音乐
    musicPlaylist->addMedia(QUrl(SOUND_PATH + "MusicEx_Normal.ogg"));
    musicPlaylist->addMedia(QUrl(SOUND_PATH + "MusicEx_Normal2.ogg"));
    musicPlaylist->addMedia(QUrl(SOUND_PATH + "MusicEx_Welcome.ogg"));
    musicPlaylist->setCurrentIndex(0);
    // 背景音乐随机播放
    musicPlaylist->setPlaybackMode(QMediaPlaylist::Random);

    m_backgroundPlayer->play();
    m_backgroundPlayer->setVolume(60);
}

void GameMusic::addMusicSource(const QString &name)
{
    QString strFileName = SOUND_PATH + name + ".ogg";
    if (!QFile::exists(strFileName)) {
        QFile::copy(QString(":/sound/%1.ogg").arg(name), strFileName);
    }
}
