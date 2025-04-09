#ifndef HOMEWGT_H
#define HOMEWGT_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>

#include "ui_homewgt.h"

class HomeWgt : public QWidget, public Ui_HomeWgt
{
    Q_OBJECT

public:
    explicit HomeWgt(QWidget *parent = nullptr);
    ~HomeWgt();

protected:
    void initialize();

    void paintEvent(QPaintEvent *event) override;

signals:
    void newGame();
    void selectLevel();
    void exit();

private:
    QPixmap m_bgPixmap;
};

#endif // HOMEWGT_H
