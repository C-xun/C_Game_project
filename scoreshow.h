#ifndef SCORESHOW_H
#define SCORESHOW_H

#include <QWidget>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif

namespace Ui {
class ScoreShow;
}

class ScoreShow : public QWidget
{
    Q_OBJECT

public:
    explicit ScoreShow(QWidget *parent = 0);
    ~ScoreShow();

    void setBombMultiple(int value);
protected:
    void changeEvent(QEvent *e);

    void paintEvent(QPaintEvent *);
private:
    Ui::ScoreShow *ui;
};

#endif // SCORESHOW_H
