#include "mainwin.h"
#include "utils.h"

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
    // m_pView->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    // m_pView->installEventFilter(this);
    setMouseTracking(true); // 如果需要持续跟踪
    this->setWindowTitle("愤怒的小鸟");
    QIcon icon(":/Resource/UI/icon/title.ico");
    QApplication::setWindowIcon(icon);

    m_pMaskWgt = new MaskWgt(this);
    m_pMaskWgt->hide();

    m_pView = new View();

    m_pScene = new Scene();
    m_pScene->setSceneRect ( 0, 0, 1800 , 967);

    m_pView->setScene(m_pScene);

    m_pBtnStop = new QPushButton(m_pView);
    m_pBtnStop->setProperty("Qss_Type", "Button Pause");
    m_pBtnStop->setToolTip("暂停");

    m_pHomeWgt = new HomeWgt;

    m_pStackedWgt->addWidget(m_pHomeWgt);
    m_pStackedWgt->addWidget(m_pView);

    m_pStopDlg = new StopDlg(m_pMaskWgt);
    m_pStopDlg->hide();

    m_pFinishDlg = new FinishDlg(m_pMaskWgt);
    m_pFinishDlg->hide();

    this->setObjectName("mainwin");
    this->setStyleSheet(QString("QWidget#mainwin{background-color: white;border-bottom-left-radius: %1px;border-bottom-right-radius: %1px;}").arg(10));
    m_pView->setObjectName("view");
    m_pView->setStyleSheet(QString("QWidget#view{background-color: rgb(220, 240, 220);border-bottom-left-radius: %1px;border-bottom-right-radius: %1px;}").arg(10));

    QTimer::singleShot(100, this, [this]() {
        m_pView->fitInView(m_pScene->sceneRect(), Qt::KeepAspectRatio);
        m_pBtnStop->move(m_pView->width() - 50, 10);
        m_pMaskWgt->move(this->mapToGlobal(QPoint(0, 0)));
    });

    m_level = Utils::readJson(":/Resource/Game/json/level.json");

    connect(m_pHomeWgt, SIGNAL(newGame()), this, SLOT(onNewGame()));
    connect(m_pHomeWgt, SIGNAL(selectLevel()), this, SLOT(onSelectLevel()));
    connect(m_pHomeWgt, SIGNAL(exit()), this, SLOT(onExit()));
    connect(m_pBtnStop, SIGNAL(clicked(bool)), this, SLOT(onStop()));
    connect(m_pStopDlg, SIGNAL(home()), this, SLOT(onHome()));
    connect(m_pStopDlg, SIGNAL(resume()), this, SLOT(onResume()));
    connect(m_pStopDlg, SIGNAL(repeat()), this, SLOT(onRepeat()));
    connect(m_pFinishDlg, SIGNAL(home()), this, SLOT(onHome()));
    connect(m_pFinishDlg, SIGNAL(repeat()), this, SLOT(onRepeat()));
    connect(m_pFinishDlg, SIGNAL(next()), this, SLOT(onNext()));
    connect(m_pScene, SIGNAL(signalBeginContact(ItemBase*,ItemBase*,QPointF)), this, SLOT(onBeginContact(ItemBase*,ItemBase*,QPointF)));
    connect(m_pScene, SIGNAL(signalTimerEvent()), this, SLOT(onTimerEvent()));

    connect(m_pView, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(onViewMousePress(QMouseEvent*)));
    connect(m_pView, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(onViewMouseRelease(QMouseEvent*)));
    connect(m_pView, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(onViewMouseMove(QMouseEvent*)));
}

void MainWin::startGame(uint level)
{
    m_pScene->stop();
    m_currLevel = level;
    m_pScene->clear();
    m_birds.clear();
    m_pigs.clear();
    m_obstacles.clear();
    m_pCurrBird = nullptr;

    QTransform transform;
    transform.reset();
    transform.scale ( 1  , -1 );
    QGraphicsPixmapItem *pBord = new QGraphicsPixmapItem(QPixmap(":/Resource/Game/icon/bord.jpg").transformed(transform, Qt::TransformationMode::SmoothTransformation));
    m_pScene->addItem(pBord);


    m_launchPos = (m_p1 + m_p2) / 2;


    transform.scale(2, 2);
    QPixmap pixSlingshot_2 = QPixmap(":/Resource/Game/icon/slingshot_2.png").transformed(transform, Qt::TransformationMode::SmoothTransformation);
    QGraphicsPixmapItem *pSlingshot_2 = new QGraphicsPixmapItem(pixSlingshot_2);
    m_pScene->addItem(pSlingshot_2);
    pSlingshot_2->setPos(300, 240);

    QPen pen(QColor::fromString("#311706"));
    pen.setWidthF(10.0);
    pen.setCapStyle(Qt::PenCapStyle::FlatCap);
    pen.setJoinStyle(Qt::RoundJoin); // 圆角连接

    m_pLineItem_2 = new QGraphicsLineItem(QLineF(m_p2, m_launchPos));
    m_pLineItem_2->setPen(pen);
    m_pScene->addItem(m_pLineItem_2);

    ItemRect* pGround = m_pScene->CreateRect(QPointF(m_pView->scene()->sceneRect().width()/2.0, 240 - 5), m_pView->scene()->sceneRect().width(), 10);
    pGround->setBrush(QColor(0, 0, 0, 0));
    pGround->setData(0, 0);

    QVariantMap map = m_level[m_currLevel].toMap();

    QVariantList birds = map.value("birds").toList();
    QVariantList pigs = map.value("pigs").toList();
    QVariantList obstacles = map.value("obstacles").toList();

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.linearDamping = 0.1f;

    b2FixtureDef ballFixtureDef;
    ballFixtureDef.restitution = 0.55f;
    ballFixtureDef.density = 1.0f;
    ballFixtureDef.friction = 0.1f;

    for(int i = 0; i < birds.count(); ++i)
    {
        QVariantMap birdMap = birds[i].toMap();
        auto birdItem = m_pScene->CreateCircle(QPointF(110 * i + 100, 900), 50, bodyDef);
        birdItem->setImage(":/Resource/Game/icon/bird_red.png");
        birdItem->setBrush(QColor(0, 0, 0, 0));
        birdItem->setMaterial(ballFixtureDef.friction, ballFixtureDef.restitution, 0);
        birdItem->setData(0, 1);

        m_birds.append(birdItem);
    }


    for(const QVariant &pig: pigs)
    {
        QVariantMap pigMap = pig.toMap();
        auto pigItem = m_pScene->CreateCircle(QPointF(pigMap.value("x").toDouble(), pigMap.value("y").toDouble()), 50, bodyDef);
        pigItem->setImage(":/Resource/Game/icon/pig.png");
        pigItem->setBrush(QColor(0, 0, 0, 0));
        pigItem->setMaterial(ballFixtureDef.friction, ballFixtureDef.restitution, ballFixtureDef.density);
        pigItem->setData(0, 2);

        m_pigs.append(pigItem);
    }

    for(const QVariant &obstacle: obstacles)
    {
        QVariantMap obstacleMap = obstacle.toMap();
        auto obstacleItem = m_pScene->CreateRect(QPointF(obstacleMap.value("x").toDouble(), obstacleMap.value("y").toDouble()), 100, 100, bodyDef);
        obstacleItem->setImage(":/Resource/Game/icon/wood.png");
        obstacleItem->setBrush(QColor(0, 0, 0, 0));
        obstacleItem->setMaterial(ballFixtureDef.friction, ballFixtureDef.restitution, ballFixtureDef.density);
        obstacleItem->setData(0, 3);

        m_obstacles.append(obstacleItem);
    }

    m_pLineItem_1 = new QGraphicsLineItem(QLineF(m_p1, m_launchPos));
    m_pLineItem_1->setPen(pen);
    m_pScene->addItem(m_pLineItem_1);

    m_pEllipseItem = new QGraphicsEllipseItem(QRectF(m_launchPos - QPointF(15, 15), m_launchPos + QPointF(15, 15)));
    m_pEllipseItem->setPen(pen);
    m_pEllipseItem->setBrush(pen.color());
    m_pScene->addItem(m_pEllipseItem);

    QPixmap pixSlingshot_1 = QPixmap(":/Resource/Game/icon/slingshot_1.png").transformed(transform, Qt::TransformationMode::SmoothTransformation);
    QGraphicsPixmapItem *pSlingshot_1 = new QGraphicsPixmapItem(pixSlingshot_1);
    m_pScene->addItem(pSlingshot_1);
    pSlingshot_1->setPos(300 - 55, 240);


    m_pScene->start();
}

void MainWin::resizeEvent(QResizeEvent *event)
{
    m_pView->fitInView(m_pScene->sceneRect(), Qt::KeepAspectRatio);
    m_pBtnStop->move(m_pView->width() - 50, 10);

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
    m_pStackedWgt->setCurrentWidget(m_pView);
    m_pView->fitInView(m_pScene->sceneRect(), Qt::KeepAspectRatio);
    m_pBtnStop->move(m_pView->width() - 50, 10);

    startGame(1);
}

void MainWin::onSelectLevel()
{

}

void MainWin::onExit()
{
    this->window()->close();
}

void MainWin::onHome()
{
    m_pStopDlg->hide();
    m_pFinishDlg->hide();
    m_pMaskWgt->hide();
    m_pStackedWgt->setCurrentWidget(m_pHomeWgt);
}

void MainWin::onResume()
{
    m_pStopDlg->hide();
    m_pFinishDlg->hide();
    m_pMaskWgt->hide();

    m_pScene->start();
}

void MainWin::onRepeat()
{
    m_pStopDlg->hide();
    m_pFinishDlg->hide();
    m_pMaskWgt->hide();

    startGame(m_currLevel);
}

void MainWin::onStop()
{
    m_pScene->stop();
    m_pMaskWgt->move(this->mapToGlobal(QPoint(0, 0)));
    m_pMaskWgt->show();
    m_pStopDlg->move((m_pMaskWgt->width() - m_pStopDlg->width())/2.0, (m_pMaskWgt->height() - m_pStopDlg->height())/2.0);
    m_pStopDlg->show();
}

void MainWin::onNext()
{
    m_pStopDlg->hide();
    m_pFinishDlg->hide();
    m_pMaskWgt->hide();

    m_currLevel++;
    if(m_currLevel >= m_level.count())
    {
        m_currLevel = 0;
    }
    startGame(m_currLevel);
}

void MainWin::onBeginContact(ItemBase *A, ItemBase *B, QPointF pos)
{
    ItemBase *pPig = nullptr;
    if(A->data(0).toInt() == 2 && B->data(0).toInt() == 1)
    {
        pPig = A;
    }
    if(A->data(0).toInt() == 1 && B->data(0).toInt() == 2)
    {
        pPig = B;
    }
    if(pPig != nullptr)
    {
        m_pScene->DestroyItem(pPig);
        m_pigs.removeOne(pPig);
        if(m_pigs.isEmpty())
        {
            m_pMaskWgt->move(this->mapToGlobal(QPoint(0, 0)));
            m_pMaskWgt->show();
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

    for(auto item: birds)
    {
        if(!m_pScene->sceneRect().contains(item->pos()))
        {
            m_pScene->DestroyItem(item);
            m_birds.removeOne(item);
        }
    }

    for(auto item: pigs)
    {
        if(!m_pScene->sceneRect().contains(item->pos()))
        {
            m_pScene->DestroyItem(item);
            m_pigs.removeOne(item);
        }
    }

    for(auto item: obstacles)
    {
        if(!m_pScene->sceneRect().contains(item->pos()))
        {
            m_pScene->DestroyItem(item);
            m_obstacles.removeOne(item);
        }
    }
}

void MainWin::onViewMousePress(QMouseEvent *event)
{
    if(m_pStackedWgt->currentWidget() != m_pView)
    {
        return;
    }
    QPointF pos = m_pView->mapToScene(event->pos());
    m_isPressEllipseItem = m_pEllipseItem->shape().contains(pos);
    if(m_isPressEllipseItem)
    {
        m_offset = m_pView->mapToScene(event->pos()) - m_launchPos;
        if(m_pCurrBird != nullptr)
        {
            m_pCurrBird->setPos(m_launchPos);
        }
        return;
    }

    for(ItemBase *bird: m_birds)
    {
        if(bird->shape().contains(pos - bird->pos()))
        {
            if(m_pCurrBird != nullptr)
            {
                m_birds.push_back(m_pCurrBird);
            }
            m_birds.removeOne(bird);
            m_pCurrBird = bird;

            b2FixtureDef ballFixtureDef;
            ballFixtureDef.restitution = 0.55f;
            ballFixtureDef.density = 1.0f;
            ballFixtureDef.friction = 0.1f;
            for(int i = 0; i < m_birds.count(); ++i)
            {
                m_birds[i]->setPos(QPointF(110 * i + 100, 900));
                m_birds[i]->setMaterial(ballFixtureDef.friction, ballFixtureDef.restitution, 0);
            }

            m_pCurrBird->setPos((m_p1 + m_p2) / 2);
            break;
        }
    }
}

void MainWin::onViewMouseRelease(QMouseEvent *event)
{
    if(m_isPressEllipseItem)
    {
        QLineF line((m_p1 + m_p2) / 2, m_launchPos);
        if(line.length() > 0)
        {
            QVariantAnimation *pBoomAnimation = new QVariantAnimation();
            pBoomAnimation->setDuration(100);
            pBoomAnimation->setStartValue(line.p2());
            pBoomAnimation->setEndValue(line.p1());

            connect(pBoomAnimation, &QVariantAnimation::finished, this, [=](){
                pBoomAnimation->deleteLater();
            });
            connect(pBoomAnimation, &QVariantAnimation::valueChanged, [=](const QVariant &val) {
                m_launchPos = val.toPointF();
                m_pLineItem_1->setLine(QLineF(m_p1, m_launchPos));
                m_pLineItem_2->setLine(QLineF(m_p2, m_launchPos));
                m_pEllipseItem->setRect(QRectF(m_launchPos - QPointF(10, 10), m_launchPos + QPointF(10, 10)));
            });

            pBoomAnimation->start();
            b2FixtureDef ballFixtureDef;
            ballFixtureDef.restitution = 0.55f;
            ballFixtureDef.density = 1.0f;
            ballFixtureDef.friction = 0.1f;
            if(m_pCurrBird != nullptr)
            {
                m_pCurrBird->setMaterial(ballFixtureDef.friction, ballFixtureDef.restitution, ballFixtureDef.restitution);
                m_pCurrBird->setLinearVelocity(((m_p1 + m_p2) / 2- m_launchPos) * 4);
                m_pCurrBird = nullptr;
            }
        }
    }
    m_isPressEllipseItem = false;
}

void MainWin::onViewMouseMove(QMouseEvent *event)
{
    if(m_isPressEllipseItem)
    {
        m_launchPos = m_pView->mapToScene(event->pos()) - m_offset;
        QLineF line = QLineF((m_p1 + m_p2) / 2, m_launchPos);
        if(line.length() > 300)
        {
            line.setLength(300);
            m_launchPos = line.p2();
        }
        m_pLineItem_1->setLine(QLineF(m_p1, m_launchPos));
        m_pLineItem_2->setLine(QLineF(m_p2, m_launchPos));
        m_pEllipseItem->setRect(QRectF(m_launchPos - QPointF(10, 10), m_launchPos + QPointF(10, 10)));
        if(m_pCurrBird != nullptr)
        {
            m_pCurrBird->setPos(m_launchPos);
            m_pCurrBird->setRotation(180-line.angle());
        }
    }
}
