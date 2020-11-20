#include "mainwindow.h"
#include "gamemusic.h"

#include <QApplication>
#include <QPixmap>
#include <QBitmap>
#include <QPainter>
#include <QVector>
#include <QSet>
#include "Strategy.h"
#include <QFont>
#include <QFile>
#include <QTranslator>

void myMessageOutput(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s\n", msg);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s\n", msg);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s\n", msg);
        break;
    case QtFatalMsg:
    default:
        fprintf(stderr, "Fatal: %s\n", msg);
        abort();
    }
}


int main(int argc, char *argv[])
{
//	qInstallMsgHandler(myMessageOutput);
    QApplication a(argc, argv);
//  QFile::link(a.applicationDirPath() + "/LordGardGame.exe", "C:/Users/Public/Desktop/Game.exe.lnk");
    QTranslator translator;
    translator.load(":/res/zh.qm", QApplication::applicationDirPath());
    a.installTranslator(&translator);

    GameMusic::Instance()->initMusic();

    MainWindow w;
//    w.setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::FramelessWindowHint);
    w.show();

    return a.exec();
}


