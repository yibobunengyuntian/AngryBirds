#ifndef STOPDLG_H
#define STOPDLG_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include "ui_stopdlg.h"

class StopDlg : public QWidget, public Ui_StopDlg
{
    Q_OBJECT

public:
    explicit StopDlg(QWidget *parent = nullptr);
    ~StopDlg();

protected:
    void initialize();

    void paintEvent(QPaintEvent *event) override;

signals:
    void home();
    void resume();
    void repeat();

private:
    QPixmap m_bgPixmap;
};

#endif // STOPDLG_H
