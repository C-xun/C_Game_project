#include "scoreshow.h"
#include "ui_scoreshow.h"

#include <QStyleOption>
#include <QPainter>

ScoreShow::ScoreShow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScoreShow)
{
    ui->setupUi(this);
}

ScoreShow::~ScoreShow()
{
    delete ui;
}

void ScoreShow::setBombMultiple(int value)
{
    ui->labelBombMultiple->setText(QString::number(value));
}

void ScoreShow::changeEvent(QEvent *e)
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

void ScoreShow::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
