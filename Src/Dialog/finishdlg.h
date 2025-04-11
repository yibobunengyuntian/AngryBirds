#ifndef FINISHDLG_H
#define FINISHDLG_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QVariantAnimation>
#include "ui_finishdlg.h"

class FinishDlg : public QWidget, public Ui_FinishDlg
{
    Q_OBJECT

public:
    explicit FinishDlg(QWidget *parent = nullptr);
    ~FinishDlg();

    void setStarNumber(const uint &num = 3);

protected:
    void initialize();

    void paintEvent(QPaintEvent *event) override;
signals:
    void home();
    void repeat();
    void next();

private:
    QPixmap m_pix0;
    QPixmap m_pix1;
    QPixmap m_pix2;
    QPixmap m_pix3;

    QPixmap m_currPix;

};

#endif // FINISHDLG_H
