#include "stopdlg.h"

StopDlg::StopDlg(QWidget *parent)
    : QWidget(parent)
{
    this->setupUi(this);

    initialize();
}

StopDlg::~StopDlg()
{

}

void StopDlg::initialize()
{
    this->setWindowFlags(Qt::FramelessWindowHint | windowFlags()); // 隐藏默认边框

    m_pBtnHome->setProperty("Qss_Type", "Button Home");
    m_pBtnResume->setProperty("Qss_Type", "Button Play");
    m_pBtnRepeat->setProperty("Qss_Type", "Button Repeat");

    m_bgPixmap.load(":/Resource/UI/icon/background/bg_stop.png");
    m_bgPixmap = m_bgPixmap.scaledToHeight(150, Qt::TransformationMode::SmoothTransformation);

    m_pBtnHome->setToolTip("返回大厅");
    m_pBtnResume->setToolTip("继续游戏");
    m_pBtnRepeat->setToolTip("重新开始");

    connect(m_pBtnHome, SIGNAL(clicked(bool)), this, SIGNAL(home()));
    connect(m_pBtnResume, SIGNAL(clicked(bool)), this, SIGNAL(resume()));
    connect(m_pBtnRepeat, SIGNAL(clicked(bool)), this, SIGNAL(repeat()));
}

void StopDlg::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 启用抗锯齿

    // 创建一个路径
    QPainterPath path;

    // 设置圆角半径
    int radius = 10;
    path.addRoundedRect(this->rect(), radius, radius);

    // 设置背景颜色
    QColor bg_color = QColor("#dcf0dc");
    bg_color.setAlpha(190);
    painter.fillPath(path, QBrush(bg_color)); // 半透明黑色
    painter.drawPixmap(20, 8, m_bgPixmap);
}
