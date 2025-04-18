#include "homewgt.h"
#include "defines.h"

HomeWgt::HomeWgt(QWidget *parent)
    : QWidget(parent)
{
    this->setupUi(this);

    initialize();
}

HomeWgt::~HomeWgt()
{

}

void HomeWgt::initialize()
{
    // 加载背景图片
    m_bgPixmap.load(":/Resource/UI/icon/background/bg_home.jpg");
    m_pBtnNew->setProperty(DEF_QSS_TYPE, "Button Rect");
    m_pBtnSelect->setProperty(DEF_QSS_TYPE, "Button Rect");
    m_pBtnExit->setProperty(DEF_QSS_TYPE, "Button Rect");

    connect(m_pBtnNew, SIGNAL(clicked(bool)), this, SIGNAL(newGame()));
    connect(m_pBtnSelect, SIGNAL(clicked(bool)), this, SIGNAL(selectLevel()));
    connect(m_pBtnExit, SIGNAL(clicked(bool)), this, SIGNAL(exit()));
}

void HomeWgt::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 启用抗锯齿

    if (m_bgPixmap.isNull())
    {
        return;
    }

    // 按窗口大小缩放（保持比例，填充整个窗口）
    QPixmap scaled = m_bgPixmap.scaled(size(),Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation);

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

    painter.setClipPath(path);

    // 居中绘制
    painter.drawPixmap((width() - scaled.width()) / 2,(height() - scaled.height()) / 2, scaled);

    QWidget::paintEvent(event);
}
