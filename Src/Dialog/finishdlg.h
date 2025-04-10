#ifndef FINISHDLG_H
#define FINISHDLG_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include "ui_finishdlg.h"

class FinishDlg : public QWidget, public Ui_FinishDlg
{
    Q_OBJECT

public:
    explicit FinishDlg(QWidget *parent = nullptr);
    ~FinishDlg();

protected:
    void initialize();

    void paintEvent(QPaintEvent *event) override;
signals:
    void home();
    void repeat();
    void next();

};

#endif // FINISHDLG_H
