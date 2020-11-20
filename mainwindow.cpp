#include "mainwindow.h"
#include "GamePanel.h"
#include "ui_mainwindow.h"

#include <QStyleOption>
#include <QPainter>
#include <QMouseEvent>

#include <QSettings>
#include <QPushButton>
#include <QApplication>
#include <QDebug>
#include <QFile>

#define SETTINGS_FILE qApp->applicationDirPath() + "/score.ini"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createSettings();

    setAttribute(Qt::WA_TranslucentBackground, true);
    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
    m_gamePanel = new GamePanel(this);

    setFixedSize(1000, 700);
    setWindowIcon(QIcon(":/lord.ico"));
    setWindowTitle(tr("单机斗地主"));

    connect(ui->btnClose,&QPushButton::clicked,[=](){
        this->close();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QRect myRect = this->rect();
    myRect.setY(50);
    m_gamePanel->setGeometry(myRect);

    QWidget::resizeEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    if (m_mousePressed && (e->buttons() && Qt::LeftButton)) {
        this->move(e->globalPos() - mousePoint);
        e->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent */*e*/)
{
    m_mousePressed = false;
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        m_mousePressed = true;
        mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}

void MainWindow::createSettings()
{
    QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
    QString strGroups = settings.childGroups().join("");
    if (!QFile::exists(SETTINGS_FILE) || (strGroups.isEmpty()))
    {
        /*sip通话配置*/
        settings.beginGroup("Score");
        settings.setValue("User", 10);
        settings.setValue("LeftRobot", 0);
        settings.setValue("RightRobot", 0);
        settings.endGroup();
    }

}
