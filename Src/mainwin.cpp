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
    this->setWindowTitle("愤怒的小鸟");
    QIcon icon(":/Resource/UI/icon/title.ico");
    QApplication::setWindowIcon(icon);

    m_pMaskWgt = new MaskWgt(this);
    m_pMaskWgt->hide();

    m_pView = new View();
    m_pView->setMouseTracking(true);
    m_pView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

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
    connect(m_pScene, SIGNAL(signalBeginContact(ItemBase*,ItemBase*,QPointF)), this, SLOT(onBeginContact(ItemBase*,ItemBase*,QPointF)));
}

void MainWin::startGame(uint level)
{
    m_pScene->stop();
    m_currLevel = level;
    m_pScene->clear();
    QTransform transform;
    transform.reset();
    transform.scale ( 1  , -1 );
    QGraphicsPixmapItem *pBord = new QGraphicsPixmapItem(QPixmap(":/Resource/Game/icon/bord.jpg").transformed(transform, Qt::TransformationMode::SmoothTransformation));
    m_pScene->addItem(pBord);

    transform.scale(2, 2);
    QPixmap pixSlingshot_2 = QPixmap(":/Resource/Game/icon/slingshot_2.png").transformed(transform, Qt::TransformationMode::SmoothTransformation);
    QGraphicsPixmapItem *pSlingshot_2 = new QGraphicsPixmapItem(pixSlingshot_2);
    m_pScene->addItem(pSlingshot_2);
    pSlingshot_2->setPos(300, 240);

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
    for(const QVariant &pig: pigs)
    {
        QVariantMap pigMap = pig.toMap();
        auto pigItem = m_pScene->CreateCircle(QPointF(pigMap.value("x").toDouble(), pigMap.value("y").toDouble()), 50, bodyDef);
        pigItem->setImage(":/Resource/Game/icon/pig.png");
        pigItem->setBrush(QColor(0, 0, 0, 0));
        pigItem->setMaterial(ballFixtureDef.friction, ballFixtureDef.restitution, ballFixtureDef.density);
        pigItem->setData(0, 1);
    }

    for(const QVariant &obstacle: obstacles)
    {
        QVariantMap obstacleMap = obstacle.toMap();
        auto obstacleMapItem = m_pScene->CreateRect(QPointF(obstacleMap.value("x").toDouble(), obstacleMap.value("y").toDouble()), 100, 100, bodyDef);
        obstacleMapItem->setImage(":/Resource/Game/icon/wood.png");
        obstacleMapItem->setBrush(QColor(0, 0, 0, 0));
        obstacleMapItem->setMaterial(ballFixtureDef.friction, ballFixtureDef.restitution, ballFixtureDef.density);
        obstacleMapItem->setData(0, 3);
    }

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
    m_pMaskWgt->hide();
    m_pStackedWgt->setCurrentWidget(m_pHomeWgt);
}

void MainWin::onResume()
{
    m_pStopDlg->hide();
    m_pMaskWgt->hide();

    m_pScene->start();
}

void MainWin::onRepeat()
{
    m_pStopDlg->hide();
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

void MainWin::onBeginContact(ItemBase *A, ItemBase *B, QPointF pos)
{
    qDebug() << A->data(0) << "::" << B->data(0) << "::" << pos;
}
