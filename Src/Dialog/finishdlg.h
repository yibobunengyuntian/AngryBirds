#ifndef FINISHDLG_H
#define FINISHDLG_H

#include <QWidget>
#include "ui_finishdlg.h"

class FinishDlg : public QWidget, public Ui_FinishDlg
{
    Q_OBJECT

public:
    explicit FinishDlg(QWidget *parent = nullptr);
    ~FinishDlg();

protected:
    void initialize();


signals:
    void home();
    void repeat();
    void next();

};

#endif // FINISHDLG_H
