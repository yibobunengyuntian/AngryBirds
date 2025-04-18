#include "mainwin.h"
#include "utils.h"
#include "defines.h"

MainWin::MainWin(QWidget *parent)
    : QWidget(parent)
{
    this->setupUi(this);

    initialize();
}

MainWin::~MainWin()
{

}

void MainWin::initialize()
{
    this->setWindowTitle("愤怒的小鸟");
    QIcon icon(":/Resource/UI/icon/title.ico");
    QApplication::setWindowIcon(icon);

    // 遮罩层窗口， 用于弹出对话框时，挡住主窗口 使主窗口变暗
    m_pMaskWgt = new MaskWgt(this);
    m_pMaskWgt->hide();

    // 游戏界面
    m_pView = new View();

    // 游戏场景
    m_pScene = new Scene();
    m_pScene->setSceneRect ( 0, 0, 1800 , 967);
    m_pView->setScene(m_pScene);

    // 暂停按钮
    m_pBtnStop = new QPushButton(m_pView);
    m_pBtnStop->setProperty(DEF_QSS_TYPE, "Button Pause");
    m_pBtnStop->setToolTip("暂停");

    // 主页窗口
    m_pHomeWgt = new HomeWgt;

    // 关卡选择窗口
    m_pLevelWgt = new LevelWgt;

    // 窗口容器
    m_pStackedWgt->addWidget(m_pHomeWgt);
    m_pStackedWgt->addWidget(m_pView);
    m_pStackedWgt->addWidget(m_pLevelWgt);

    // 暂停对话框
    m_pStopDlg = new StopDlg(m_pMaskWgt);
    m_pStopDlg->hide();

    // 游戏结算对话框
    m_pFinishDlg = new FinishDlg(m_pMaskWgt);
    m_pFinishDlg->hide();

    // 设置窗口背景及圆角
    this->setObjectName("mainwin");
    this->setStyleSheet(QString("QWidget#mainwin{background-color: rgb(220, 240, 220);border-bottom-left-radius: %1px;border-bottom-right-radius: %1px;}").arg(10));
    m_pView->setObjectName("view");
    m_pView->setStyleSheet(QString("QWidget#view{background-color: rgb(220, 240, 220);border-bottom-left-radius: %1px;border-bottom-right-radius: %1px;}").arg(10));

    // 创建属性动画 用于动态改变 爆炸图形 的大小达到爆炸的效果
    m_pBoomAnimation = new QVariantAnimation();
    m_pBoomAnimation->setDuration(200);
    m_pBoomAnimation->setStartValue(0.2);
    m_pBoomAnimation->setEndValue(1.0);

    // 读取游戏关卡文件 若没有则使用默认文件
    QString levelPath = Utils::readConfig(qApp->applicationDirPath() + "/ini.cfg", "levelPath").toString();
    if(levelPath.isEmpty() || Utils::fileExsit(levelPath))
    {
        levelPath = ":/Resource/Game/json/level.json";
    }
    m_level = Utils::readJson(levelPath);

    // 将关卡文件传递到关卡选择窗口
    m_pLevelWgt->setLevel(m_level);

    // 链接相关窗口 按钮的信号槽
    connect(m_pHomeWgt, SIGNAL(newGame()), this, SLOT(onNewGame()));
    connect(m_pHomeWgt, SIGNAL(selectLevel()), this, SLOT(onSelectLevel()));
    connect(m_pHomeWgt, SIGNAL(exit()), this, SLOT(onExit()));
    connect(m_pLevelWgt, SIGNAL(selectedLevel(uint)), this, SLOT(onStartLevel(uint)));
    connect(m_pLevelWgt, SIGNAL(home()), this, SLOT(onHome()));
    connect(m_pBtnStop, SIGNAL(clicked(bool)), this, SLOT(onStop()));
    connect(m_pStopDlg, SIGNAL(home()), this, SLOT(onHome()));
    connect(m_pStopDlg, SIGNAL(resume()), this, SLOT(onResume()));
    connect(m_pStopDlg, SIGNAL(repeat()), this, SLOT(onRepeat()));
    connect(m_pFinishDlg, SIGNAL(home()), this, SLOT(onHome()));
    connect(m_pFinishDlg, SIGNAL(repeat()), this, SLOT(onRepeat()));
    connect(m_pFinishDlg, SIGNAL(next()), this, SLOT(onNext()));

    // 链接碰撞信号
    connect(m_pScene, SIGNAL(signalBeginContact(ItemBase*,ItemBase*,QPointF)), this, SLOT(onBeginContact(ItemBase*,ItemBase*,QPointF)));
    // 链接游戏每帧信号
    connect(m_pScene, SIGNAL(signalTimerEvent()), this, SLOT(onTimerEvent()));

    // 链接游戏窗口中的鼠标时间信号， 实现游戏操作
    connect(m_pView, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(onViewMousePress(QMouseEvent*)));
    connect(m_pView, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(onViewMouseRelease(QMouseEvent*)));
    connect(m_pView, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(onViewMouseMove(QMouseEvent*)));

    // 爆炸动画结束时，隐藏爆炸图形
    connect(m_pBoomAnimation, &QVariantAnimation::finished, this, [=](){
        if(m_pScene->items().contains(m_pBoomItem))
        {
            m_pBoomItem->setVisible(false);
        }
    });
    // 实现属性动画对爆炸图形缩放的改变
    connect(m_pBoomAnimation, &QVariantAnimation::valueChanged, this, [=](const QVariant &val) {
        if(m_pScene->items().contains(m_pBoomItem))
        {
            m_pBoomItem->setScale(val.toDouble());
        }
    });
}

void MainWin::startGame(uint level)
{
    // 暂停游戏并清空所有图形
    m_pScene->stop();
    m_currLevel = level;
    m_pScene->clear();
    m_birds.clear();
    m_pigs.clear();
    m_obstacles.clear();
    m_pCurrBird = nullptr;

    // 由于我在View窗口内部实现了上下翻转达到物理中的y轴上方为正方向，所以需要用transform对相关图像进行上下翻转
    QTransform transform;
    transform.reset();
    transform.scale ( 1  , -1 );
    // 游戏界面背景图
    QGraphicsPixmapItem *pBord = new QGraphicsPixmapItem(QPixmap(":/Resource/Game/icon/bord.jpg").transformed(transform, Qt::TransformationMode::SmoothTransformation));
    m_pScene->addItem(pBord);

    //m_p1 与 m_p2 为弹弓图片上系绳子的两个点， m_launchPos 为操作发射点
    m_launchPos = (m_p1 + m_p2) / 2;

    // 弹弓图片太大， 进行相应缩放
    transform.scale(0.5, 0.5);
    // 弹弓右半部分图像
    QPixmap pixSlingshot_2 = QPixmap(":/Resource/Game/icon/slingshot_2.png").transformed(transform, Qt::TransformationMode::SmoothTransformation);
    QGraphicsPixmapItem *pSlingshot_2 = new QGraphicsPixmapItem(pixSlingshot_2);
    m_pScene->addItem(pSlingshot_2);
    pSlingshot_2->setPos(300, 240);

    QPen pen(QColor::fromString("#311706"));
    pen.setWidthF(10.0);
    pen.setCapStyle(Qt::PenCapStyle::FlatCap);
    pen.setJoinStyle(Qt::RoundJoin); // 圆角连接

    // 弹弓绳子(右侧)
    m_pLineItem_2 = new QGraphicsLineItem(QLineF(m_p2, m_launchPos));
    m_pLineItem_2->setPen(pen);
    m_pScene->addItem(m_pLineItem_2);

    // 创建 上下左右四面无形的墙将游戏场景包围
    ItemRect* pBottom = m_pScene->CreateRect(QPointF(m_pView->scene()->sceneRect().width()/2.0, 240), m_pView->scene()->sceneRect().width(), 1);
    pBottom->setBrush(QColor(0, 0, 0, 0));
    pBottom->setData(0, ItemType::Box);
    ItemRect* pTop = m_pScene->CreateRect(QPointF(m_pView->scene()->sceneRect().width()/2.0, m_pView->scene()->sceneRect().height() - 1), m_pView->scene()->sceneRect().width(), 1);
    pTop->setBrush(QColor(0, 0, 0, 0));
    pTop->setData(0, ItemType::Box);
    ItemRect* pLeft = m_pScene->CreateRect(QPointF(0, m_pView->scene()->sceneRect().height() / 2.0), 1, m_pView->scene()->sceneRect().height());
    pLeft->setBrush(QColor(0, 0, 0, 0));
    pLeft->setData(0, ItemType::Box);
    ItemRect* pRight = m_pScene->CreateRect(QPointF(m_pView->scene()->sceneRect().width() - 1, m_pView->scene()->sceneRect().height()/2.0), 1, m_pView->scene()->sceneRect().height());
    pRight->setBrush(QColor(0, 0, 0, 0));
    pRight->setData(0, ItemType::Box);

    // 读取当前关卡数据
    QVariantMap levelMap = m_level[m_currLevel].toMap();
    // 鸟
    QVariantList birds = levelMap.value(DEF_BIRDS).toList();
    // 猪
    QVariantList pigs = levelMap.value(DEF_PIGS).toList();
    // 障碍物
    QVariantList obstacles = levelMap.value(DEF_OBSTACLES).toList();

    // 定义刚体数据
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.linearDamping = 0.1f;

    // 弹性系数（反弹系数），范围 [0,1]
    float restitution = 0.55f;
    // 密度(千克/平方米)
    float density = 1.0f;
    // 摩擦系数，范围通常为 [0,1]
    float friction = 0.1f;

    // 将关卡中的小鸟添加到场景
    for(int i = 0; i < birds.count(); ++i)
    {
        QVariantMap birdMap = birds[i].toMap();
        ItemBase *birdItem = nullptr;
        int type = birdMap.value("type").toInt();
        if(type == 0)
        {
            // 红色小鸟(拟为圆形)
            birdItem = m_pScene->CreateCircle(QPointF(110 * i + 100, 900), 50, bodyDef);
            birdItem->setImage(":/Resource/Game/icon/bird_red.png");
        }
        else if(type == 1)
        {
            // 黄色小鸟(拟为三角形)
            QList<QPointF> points;
            points.append(QPoint(0, 0));
            points.append(QPoint(-80/qSqrt(3), -80));
            points.append(QPoint(80/qSqrt(3), -80));
            birdItem = m_pScene->CreatePolygon(points, bodyDef);
            birdItem->setPos(QPoint(110 * i + 100, 900));
            birdItem->setImage(":/Resource/Game/icon/bird_yellow.png");
        }

        if(birdItem != nullptr)
        {
            birdItem->setBrush(QColor(0, 0, 0, 0));
            birdItem->setMaterial(friction, restitution, 0);
            birdItem->setData(0, ItemType::Bird);
            birdItem->setData(1, type);
            m_birds.append(birdItem);
        }
    }
    m_birdQueue = m_birds;

    // 将关卡中的猪添加到场景
    for(const QVariant &pig: pigs)
    {
        QVariantMap pigMap = pig.toMap();
        ItemBase *pigItem = nullptr;
        int type = pigMap.value("type").toInt();
        if(type == PigType::Pig1)
        {
            pigItem = m_pScene->CreateCircle(QPointF(pigMap.value("x").toDouble(), pigMap.value("y").toDouble()), 50, bodyDef);
            pigItem->setImage(":/Resource/Game/icon/pig.png");
        }

        if(pigItem != nullptr)
        {
            pigItem->setBrush(QColor(0, 0, 0, 0));
            pigItem->setMaterial(friction, restitution, density);
            pigItem->setData(0, ItemType::Pig);
            pigItem->setData(1, type);
            m_pigs.append(pigItem);
        }
    }

    // 将关卡中的障碍物添加到场景
    for(const QVariant &obstacle: obstacles)
    {
        QVariantMap obstacleMap = obstacle.toMap();
        ItemBase *obstacleItem = nullptr;
        int type = obstacleMap.value("type").toInt();

        if(type == 0)
        {
            // 水平木板
            obstacleItem = m_pScene->CreateRect(QPointF(obstacleMap.value("x").toDouble(), obstacleMap.value("y").toDouble()), 150, 20, bodyDef);
            obstacleItem->setImage(":/Resource/Game/icon/plank_1.png");
        }
        else if(type == 1)
        {
            // 垂直木板
            obstacleItem = m_pScene->CreateRect(QPointF(obstacleMap.value("x").toDouble(), obstacleMap.value("y").toDouble()), 20, 120, bodyDef);
            obstacleItem->setImage(":/Resource/Game/icon/plank_2.png");
            // QPixmap(":/Resource/Game/icon/plank_2.png").scaled(100, 600, Qt::IgnoreAspectRatio, Qt::SmoothTransformation).save("D:/test/AngryBirds/Resource/Game/plank_2.png");
        }


        if(obstacleItem != nullptr)
        {
            obstacleItem->setBrush(QColor(0, 0, 0, 0));
            obstacleItem->setMaterial(friction, restitution, density);
            obstacleItem->setData(0, ItemType::Obstacle);
            obstacleItem->setData(1, type);
            m_obstacles.append(obstacleItem);
        }
    }

    // 弹弓绳子(左侧)
    m_pLineItem_1 = new QGraphicsLineItem(QLineF(m_p1, m_launchPos));
    m_pLineItem_1->setPen(pen);
    m_pScene->addItem(m_pLineItem_1);

    // 发射操作器(圆形)
    m_pEllipseItem = new QGraphicsEllipseItem(QRectF(m_launchPos - QPointF(15, 15), m_launchPos + QPointF(15, 15)));
    m_pEllipseItem->setPen(pen);
    m_pEllipseItem->setBrush(pen.color());
    m_pScene->addItem(m_pEllipseItem);

    // 弹弓图像(左侧)
    QPixmap pixSlingshot_1 = QPixmap(":/Resource/Game/icon/slingshot_1.png").transformed(transform, Qt::TransformationMode::SmoothTransformation);
    QGraphicsPixmapItem *pSlingshot_1 = new QGraphicsPixmapItem(pixSlingshot_1);
    m_pScene->addItem(pSlingshot_1);
    pSlingshot_1->setPos(pSlingshot_2->pos().x() - pSlingshot_1->boundingRect().width() + 1, pSlingshot_2->pos().y());

    // 爆炸图像
    QPixmap pix(":/Resource/Game/icon/boom.png");
    m_pBoomItem = new QGraphicsPixmapItem(pix);
    m_pBoomItem->setOffset(-pix.width()/2.0, -pix.height()/2.0);
    m_pBoomItem->setScale(0.5);
    m_pScene->addItem(m_pBoomItem);
    m_pBoomItem->setVisible(false);

    m_pScene->start();
}

void MainWin::resizeEvent(QResizeEvent *event)
{
    // 使游戏场景始终保持在窗口内
    m_pView->fitInView(m_pScene->sceneRect(), Qt::KeepAspectRatio);

    // 使暂停按钮始终在游戏窗口右上角
    m_pBtnStop->move(m_pView->width() - 50, 10);

    // 调整遮罩层窗口位置与大小， 使其刚好覆盖主窗口， 且留出标题栏
    if(m_pMaskWgt){
        m_pMaskWgt->move(this->mapToGlobal(QPoint(0, 0)));
        m_pMaskWgt->resize(this->size());
        m_pStopDlg->move((m_pMaskWgt->width() - m_pStopDlg->width())/2.0, (m_pMaskWgt->height() - m_pStopDlg->height())/2.0);
        m_pFinishDlg->move((m_pMaskWgt->width() - m_pFinishDlg->width())/2.0, (m_pMaskWgt->height() - m_pFinishDlg->height())/2.0);
    }

    QWidget::resizeEvent(event);
}

void MainWin::onNewGame()
{
    // 将游戏窗口设置为当前窗口
    m_pStackedWgt->setCurrentWidget(m_pView);
    // 使游戏场景保持在窗口内
    m_pView->fitInView(m_pScene->sceneRect(), Qt::KeepAspectRatio);
    // 调整暂停按钮位置
    m_pBtnStop->move(m_pView->width() - 50, 10);

    // 开始新游戏
    startGame(0);
}

void MainWin::onSelectLevel()
{
    // 将关卡选择窗口设置为当前窗口
    m_pStackedWgt->setCurrentWidget(m_pLevelWgt);
}

void MainWin::onStartLevel(uint level)
{
    // 根据选择的关卡开始游戏

    m_pStackedWgt->setCurrentWidget(m_pView);
    m_pView->fitInView(m_pScene->sceneRect(), Qt::KeepAspectRatio);
    m_pBtnStop->move(m_pView->width() - 50, 10);

    startGame(level);
}

void MainWin::onExit()
{
    // 退出游戏
    this->window()->close();
}

void MainWin::onHome()
{
    // 隐藏所有对话框及遮罩层窗口
    m_pScene->stop();
    m_pStopDlg->hide();
    m_pFinishDlg->hide();
    m_pMaskWgt->hide();
    // 将主页设置为当前窗口
    m_pStackedWgt->setCurrentWidget(m_pHomeWgt);
}

void MainWin::onResume()
{
    // 隐藏所有对话框及遮罩层窗口
    m_pStopDlg->hide();
    m_pFinishDlg->hide();
    m_pMaskWgt->hide();

    // 继续游戏
    m_pScene->start();
    // 若爆炸动画属于暂停状况，则继续动画
    if(m_pBoomAnimation->state() == QAbstractAnimation::Paused)
    {
        m_pBoomAnimation->resume();
    }
}

void MainWin::onRepeat()
{
    // 隐藏所有对话框及遮罩层窗口
    m_pStopDlg->hide();
    m_pFinishDlg->hide();
    m_pMaskWgt->hide();

    // 重新开始当前关卡
    startGame(m_currLevel);
}

void MainWin::onStop()
{
    // 暂停游戏
    m_pScene->stop();

    // 若爆炸动画正在进行，则暂停爆炸动画
    if(m_pBoomAnimation->state() == QAbstractAnimation::Running)
    {
        m_pBoomAnimation->pause();
    }

    // 显示遮罩层窗口及暂停对话框
    m_pMaskWgt->move(this->mapToGlobal(QPoint(0, 0)));
    m_pMaskWgt->show();
    m_pStopDlg->move((m_pMaskWgt->width() - m_pStopDlg->width())/2.0, (m_pMaskWgt->height() - m_pStopDlg->height())/2.0);
    m_pStopDlg->show();
}

void MainWin::onNext()
{
    // 隐藏所有对话框及遮罩层窗口
    m_pStopDlg->hide();
    m_pFinishDlg->hide();
    m_pMaskWgt->hide();

    // 下一关
    m_currLevel++;
    if(m_currLevel >= m_level.count())
    {
        m_currLevel = 0;
    }

    // 开始下一关
    startGame(m_currLevel);
}

void MainWin::onBeginContact(ItemBase *A, ItemBase *B, QPointF pos)
{
    Q_UNUSED(pos)

    // 检查两个碰撞体是否为一个为鸟，一个为猪
    ItemBase *pPig = nullptr;
    if(A->data(0).toInt() == ItemType::Pig && B->data(0).toInt() == ItemType::Bird)
    {
        pPig = A;
    }
    if(A->data(0).toInt() == ItemType::Bird && B->data(0).toInt() == ItemType::Pig)
    {
        pPig = B;
    }
    // 鸟与猪发生了碰撞
    if(pPig != nullptr)
    {
        // 删除猪，并在猪的位置开始爆炸动画
        m_pScene->DestroyItem(pPig);
        m_pigs.removeOne(pPig);
        m_pBoomItem->setVisible(true);
        m_pBoomItem->setPos(pPig->pos());
        m_pBoomAnimation->stop();
        m_pBoomAnimation->start();

        // 若场景上的猪删除完了，则游戏胜利
        if(m_pigs.isEmpty())
        {
            // 显示遮罩层窗口及结算对话框
            m_pMaskWgt->move(this->mapToGlobal(QPoint(0, 0)));
            m_pMaskWgt->show();
            // 设置星数(目前默认全为3颗星)
            m_pFinishDlg->setStarNumber(3);
            m_pFinishDlg->move((m_pMaskWgt->width() - m_pFinishDlg->width())/2.0, (m_pMaskWgt->height() - m_pFinishDlg->height())/2.0);
            m_pFinishDlg->show();
        }
    }
}

void MainWin::onTimerEvent()
{
    auto birds = m_birds;
    auto pigs = m_pigs;
    auto obstacles = m_obstacles;

    // 场景是否处于静止
    bool isStatic = true;

    for(auto item: birds)
    {
        // 若物体运动到场景之外，则删除(目前场景设置了无形墙体，不会出现该情况)
        if(!m_pScene->sceneRect().contains(item->pos()))
        {
            m_pScene->DestroyItem(item);
            m_birds.removeOne(item);
        }
        // 若有物体的线速度大于1则认为场景非静止
        if(QVector2D(item->linearVelocity()).length() > 1)
        {
            isStatic = false;
        }
    }

    for(auto item: pigs)
    {
        // 若物体运动到场景之外，则删除(目前场景设置了无形墙体，不会出现该情况)
        if(!m_pScene->sceneRect().contains(item->pos()))
        {
            m_pScene->DestroyItem(item);
            m_pigs.removeOne(item);
        }
        // 若有物体的线速度大于1则认为场景非静止
        if(QVector2D(item->linearVelocity()).length() > 1)
        {
            isStatic = false;
        }
    }

    for(auto item: obstacles)
    {
        // 若物体运动到场景之外，则删除(目前场景设置了无形墙体，不会出现该情况)
        if(!m_pScene->sceneRect().contains(item->pos()))
        {
            m_pScene->DestroyItem(item);
            m_obstacles.removeOne(item);
        }
        // 若有物体的线速度大于1则认为场景非静止
        if(QVector2D(item->linearVelocity()).length() > 1)
        {
            isStatic = false;
        }
    }

    // 若场景静止，且鸟已经使用完，则判定游戏失败
    if(m_pCurrBird == nullptr && m_birdQueue.isEmpty() && isStatic && m_pFinishDlg->isHidden())
    {
        m_pMaskWgt->move(this->mapToGlobal(QPoint(0, 0)));
        m_pMaskWgt->show();
        // 失败则为0颗星
        m_pFinishDlg->setStarNumber(0);
        m_pFinishDlg->move((m_pMaskWgt->width() - m_pFinishDlg->width())/2.0, (m_pMaskWgt->height() - m_pFinishDlg->height())/2.0);
        m_pFinishDlg->show();
    }
}

void MainWin::onViewMousePress(QMouseEvent *event)
{
    if(m_pStackedWgt->currentWidget() != m_pView)
    {
        return;
    }
    // 得到光标在场景中的位置
    QPointF pos = m_pView->mapToScene(event->pos());
    // 判断是否点击了发射器
    m_isPressEllipseItem = m_pEllipseItem->shape().contains(pos);
    if(m_isPressEllipseItem)
    {
        // 若点击了发射器，计算一个光标位置补偿
        m_offset = m_pView->mapToScene(event->pos()) - m_launchPos;
        return;
    }

    for(ItemBase *bird: m_birdQueue)
    {
        // 若点击了上方待使用区的鸟
        if(bird->shape().contains(pos - bird->pos()))
        {
            // 若发射器上面已经有鸟，则先将该鸟移动到待使用区
            if(m_pCurrBird != nullptr)
            {
                m_birdQueue.push_back(m_pCurrBird);
            }
            // 将点击的鸟移除待使用区，并放到发射器
            m_birdQueue.removeOne(bird);
            m_pCurrBird = bird;

            // 刷新待使用区的鸟的位置及状态
            float restitution = 0.55f;
            float friction = 0.1f;
            for(int i = 0; i < m_birdQueue.count(); ++i)
            {
                m_birdQueue[i]->setPos(QPointF(110 * i + 100, 900));
                // 密度设置为0， 以保持静止状态
                m_birdQueue[i]->setMaterial(friction, restitution, 0);
            }

            // 将鸟放到发射器位置
            m_pCurrBird->setPos((m_p1 + m_p2) / 2);
            break;
        }
    }
}

void MainWin::onViewMouseRelease(QMouseEvent *event)
{
    Q_UNUSED(event)

    // 如果发射器处于按下状态
    if(m_isPressEllipseItem)
    {
        // 以发射器当前位置与初始位置的连线，计算出发射速度与方向
        QLineF line((m_p1 + m_p2) / 2, m_launchPos);
        // 若连线长度大于0则为有效发射
        if(line.length() > 0)
        {
            // 创建一个属性动画，实现弹弓绳子与发射器 发射后的运动效果
            QVariantAnimation *pBoomAnimation = new QVariantAnimation();
            // 动画时间 100 毫秒
            pBoomAnimation->setDuration(100);
            // 起点 发射器当前位置
            pBoomAnimation->setStartValue(line.p2());
            // 终点 发射器初始位置
            pBoomAnimation->setEndValue(line.p1());

            // 动画结束即销毁，防止内存泄露
            connect(pBoomAnimation, &QVariantAnimation::finished, this, [=](){
                pBoomAnimation->deleteLater();
            });
            // 根据动画属性变换过程中的值调整绳子与发射器的位置，实现动画效果
            connect(pBoomAnimation, &QVariantAnimation::valueChanged, this, [=](const QVariant &val) {
                m_launchPos = val.toPointF();
                m_pLineItem_1->setLine(QLineF(m_p1, m_launchPos));
                m_pLineItem_2->setLine(QLineF(m_p2, m_launchPos));
                m_pEllipseItem->setRect(QRectF(m_launchPos - QPointF(15, 15), m_launchPos + QPointF(15, 15)));
            });

            // 动画开始
            pBoomAnimation->start();

            float restitution = 0.55f;
            float density = 1.0f;
            float friction = 0.1f;

            // 如果发射器上面有鸟，则发射该鸟
            if(m_pCurrBird != nullptr)
            {
                m_pCurrBird->setMaterial(friction, restitution, density);
                // 根据发射器当前位置与初始位置的连线计算出一个矢量的线速度，并赋予当前发射的鸟
                m_pCurrBird->setLinearVelocity((line.p1() - line.p2()) * 5);
                m_pCurrBird = nullptr;
            }
        }
    }
    // 取消发射器的点击状态
    m_isPressEllipseItem = false;
}

void MainWin::onViewMouseMove(QMouseEvent *event)
{
    // 如果发射器被选中
    if(m_isPressEllipseItem)
    {
        // 计算出发射器随鼠标移动的位置
        m_launchPos = m_pView->mapToScene(event->pos()) - m_offset;
        QLineF line = QLineF((m_p1 + m_p2) / 2, m_launchPos);
        // 限制发射器的拉伸长度
        if(line.length() > 300)
        {
            line.setLength(300);
            m_launchPos = line.p2();
        }
        // 改变绳子与发射器的位置
        m_pLineItem_1->setLine(QLineF(m_p1, m_launchPos));
        m_pLineItem_2->setLine(QLineF(m_p2, m_launchPos));
        m_pEllipseItem->setRect(QRectF(m_launchPos - QPointF(10, 10), m_launchPos + QPointF(10, 10)));

        // 若发射器上面有鸟
        if(m_pCurrBird != nullptr)
        {
            // 改变鸟的位置
            m_pCurrBird->setPos(m_launchPos);
            // 根据拉伸角度改变鸟的角度
            m_pCurrBird->setRotation(180-line.angle());
        }
    }
}
