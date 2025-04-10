#include "finishdlg.h"

FinishDlg::FinishDlg(QWidget *parent)
    : QWidget(parent)
{
    this->setupUi(this);

    initialize();
}

FinishDlg::~FinishDlg()
{

}

void FinishDlg::initialize()
{
    this->setWindowFlags(Qt::FramelessWindowHint | windowFlags()); // 隐藏默认边框

    m_pBtnHome->setProperty("Qss_Type", "Button Home");
    m_pBtnRepeat->setProperty("Qss_Type", "Button Repeat");
    m_pBtnNext->setProperty("Qss_Type", "Button ArrowRight-Thin");

    m_pBtnHome->setToolTip("返回大厅");
    m_pBtnRepeat->setToolTip("重新开始");
    m_pBtnNext->setToolTip("下一关");

    connect(m_pBtnHome, SIGNAL(clicked(bool)), this, SIGNAL(home()));
    connect(m_pBtnRepeat, SIGNAL(clicked(bool)), this, SIGNAL(repeat()));
    connect(m_pBtnNext, SIGNAL(clicked(bool)), this, SIGNAL(next()));
}

void FinishDlg::paintEvent(QPaintEvent *event)
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
    QColor bg_color = QColor::fromString("#dcf0dc");
    bg_color.setAlpha(190);
    painter.fillPath(path, QBrush(bg_color));
}
