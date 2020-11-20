#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif

class GamePanel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    QPoint mousePoint;
    bool m_mousePressed;

    void changeEvent(QEvent *e);
    void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent* event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
private:
    Ui::MainWindow *ui;
    GamePanel* m_gamePanel;

private:
    void createSettings();
};

#endif // MAINWINDOW_H
