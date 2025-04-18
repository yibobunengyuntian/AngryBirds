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
#include "levelwgt.h"


class MainWin : public QWidget, public Ui_MainWin
{
    Q_OBJECT

    enum ItemType
    {
        Box = 0,    // 地面与四周墙壁
        Bird,       // 鸟
        Pig,        // 猪
        Obstacle    // 障碍物
    };
    enum BirdType
    {
        RedBird = 0,// 红色鸟
        YellowBird, // 黄色鸟
    };
    enum PigType
    {
        Pig1 = 0,   // 猪1
    };
    enum ObstacleType
    {
        Plank_H = 0,// 木板(水平)
        Plank_V,    // 木板(竖直)
    };

public:
    explicit MainWin(QWidget *parent = nullptr);
    ~MainWin();

protected:
    // 初始化
    void initialize();

    /**
     * @brief startGame 开始游戏
     * @param level     关卡
     */
    void startGame(uint level = 0);

    void resizeEvent(QResizeEvent *event) override;

protected slots:
    // 新游戏
    void onNewGame();

    // 选择关卡
    void onSelectLevel();

    /**
     * @brief onStartLevel  开始关卡
     * @param level         关卡
     */
    void onStartLevel(uint level);

    // 退出游戏
    void onExit();

    // 主页
    void onHome();

    // 继续游戏
    void onResume();

    // 重新开始
    void onRepeat();

    // 暂停
    void onStop();

    // 下一关
    void onNext();

    /**
     * @brief onBeginContact    // 发生碰撞触发该函数
     * @param A                 // 物体A
     * @param B                 // 物体B
     * @param pos               // 碰撞点
     */
    void onBeginContact(ItemBase *A, ItemBase *B, QPointF pos);

    // 游戏每一帧触发该函数
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
    LevelWgt *m_pLevelWgt = nullptr;

    QPointF m_p1 = QPointF(300 - 30, 240 + 347);
    QPointF m_p2 = QPointF(300 + 58, 240 + 344);
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
    QList<ItemBase*> m_birdQueue;
    ItemBase *m_pCurrBird = nullptr;

    QGraphicsPixmapItem *m_pBoomItem = nullptr;
    QVariantAnimation *m_pBoomAnimation = nullptr;
};

#endif // MAINWIN_H
