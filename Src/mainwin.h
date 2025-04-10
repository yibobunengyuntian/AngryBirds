#ifndef MAINWIN_H
#define MAINWIN_H

#include <QWidget>
#include <QPushButton>
#include <QMouseEvent>
#include <QTimer>
#include <QVariantAnimation>

#include "ui_mainwin.h"
#include "view.h"
#include "stopdlg.h"
#include "finishdlg.h"
#include "homewgt.h"
#include "maskwgt.h"


class MainWin : public QWidget, public Ui_MainWin
{
    Q_OBJECT

public:
    explicit MainWin(QWidget *parent = nullptr);
    ~MainWin();

protected:
    void initialize();
    void startGame(uint level = 0);

    void resizeEvent(QResizeEvent *event) override;

protected slots:
    void onNewGame();
    void onSelectLevel();
    void onExit();
    void onHome();
    void onResume();
    void onRepeat();
    void onStop();
    void onNext();

    void onBeginContact(ItemBase *A, ItemBase *B, QPointF pos);
    void onTimerEvent();

    void onViewMousePress(QMouseEvent *event);
    void onViewMouseRelease(QMouseEvent *event);
    void onViewMouseMove(QMouseEvent *event);

private:
    MaskWgt *m_pMaskWgt = nullptr;
    QPushButton *m_pBtnStop = nullptr;
    Scene *m_pScene = nullptr;
    View *m_pView = nullptr;
    StopDlg *m_pStopDlg = nullptr;
    FinishDlg *m_pFinishDlg = nullptr;
    HomeWgt *m_pHomeWgt = nullptr;

    QPointF m_p1 = QPointF(300 - 55 + 32, 240 + 296);
    QPointF m_p2 = QPointF(300 + 54, 240 + 304);
    QPointF m_launchPos;
    QGraphicsEllipseItem * m_pEllipseItem = nullptr;
    QGraphicsLineItem *m_pLineItem_1 = nullptr;
    QGraphicsLineItem *m_pLineItem_2 = nullptr;

    QPointF m_offset;
    bool m_isPressEllipseItem = false;

    QVariantList m_level;
    uint m_currLevel = 0;

    QList<ItemBase*> m_birds;
    QList<ItemBase*> m_pigs;
    QList<ItemBase*> m_obstacles;
    ItemBase *m_pCurrBird = nullptr;
};

#endif // MAINWIN_H
