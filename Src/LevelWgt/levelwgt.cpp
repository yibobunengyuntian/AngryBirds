#include "levelwgt.h"

LevelWgt::LevelWgt(QWidget *parent)
    :QWidget(parent)
{
    // 设置窗口背景为透明
    setAttribute(Qt::WA_TranslucentBackground);
    QVBoxLayout *pLayout = new QVBoxLayout;

    m_pBtnHome = new QPushButton(this);
    m_pBtnHome->setProperty("Qss_Type", "Button Home");
    m_pBtnHome->setToolTip("返回大厅");

    pLayout->addWidget(m_pBtnHome);

    m_pCenterWgt = new QWidget;
    m_pScrollArea = new QScrollArea;
    m_pScrollArea->setWidget(m_pCenterWgt);
    m_pScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    pLayout->addWidget(m_pScrollArea);

    this->setLayout(pLayout);

    connect(m_pBtnHome, SIGNAL(clicked(bool)), this, SIGNAL(home()));
}

void LevelWgt::setLevel(const QVariantList &level)
{
    for(auto btn: m_buttons)
    {
        btn->deleteLater();
    }
    m_buttons.clear();

    for(int i = 0; i < level.count(); ++i)
    {
        QPushButton *btn = new QPushButton(QString("第%1关").arg(i + 1), m_pCenterWgt);
        btn->setProperty("Qss_Type", "Button Rect");
        m_buttons.append(btn);
        connect(btn, &QPushButton::clicked, this, [=](){
            emit selectedLevel(i);
        });
    }
}

void LevelWgt::resizeEvent(QResizeEvent *event)
{
    int n = (m_pScrollArea->width() - 20) / 100;

    for(int i = 0; i < m_buttons.count(); ++i)
    {
        m_buttons[i]->move(10 + (i % n) * 100, 10 + (i / n) * 60);
    }
    m_pCenterWgt->setFixedWidth(m_pScrollArea->width() - 20);
    m_pCenterWgt->setFixedHeight(qMax(80 + (m_buttons.count() / n) * 60, m_pScrollArea->height()));
}

void LevelWgt::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 启用抗锯齿

    // 创建一个路径
    QPainterPath path;

    // 设置圆角半径
    int radius = 10;

    // 绘制顶部两个直角边
    path.moveTo(0, 0);
    path.lineTo(width(), 0);
    path.lineTo(width(), height() - radius);

    // 绘制右下角圆角
    path.arcTo(width() - 2 * radius, height() - 2 * radius, 2 * radius, 2 * radius, 0, -90);

    // 绘制左下角圆角
    path.arcTo(0, height() - 2 * radius, 2 * radius, 2 * radius, -90, -90);

    // 绘制左上角直角边
    path.lineTo(0, radius);

    // 设置背景颜色
    painter.fillPath(path, QBrush(QColor(220, 240, 220))); // 半透明黑色
}
