#include "finishdlg.h"
#include "defines.h"

FinishDlg::FinishDlg(QWidget *parent)
    : QWidget(parent)
{
    this->setupUi(this);

    initialize();
}

FinishDlg::~FinishDlg()
{

}

void FinishDlg::setStarNumber(const uint &num)
{
    m_currPix = m_pix0;
    if(num == 0)
    {
        m_pBtnNext->hide();
        this->update();
        return;
    }
    m_pBtnNext->hide();
    m_pBtnHome->hide();
    m_pBtnRepeat->hide();
    QVariantAnimation *pBoomAnimation = new QVariantAnimation();
    pBoomAnimation->setDuration(300 * num);
    pBoomAnimation->setStartValue(0.0);
    pBoomAnimation->setEndValue(num * 1.0);

    connect(pBoomAnimation, &QVariantAnimation::finished, this, [=](){
        m_pBtnNext->show();
        m_pBtnHome->show();
        m_pBtnRepeat->show();
        pBoomAnimation->deleteLater();
    });
    connect(pBoomAnimation, &QVariantAnimation::valueChanged, this, [=](const QVariant &val) {
        if(val.toDouble() >= 1)
        {
            m_currPix = m_pix1;
        }
        if(val.toDouble() >= 2)
        {
            m_currPix = m_pix2;
        }
        if(val.toDouble() >= 3)
        {
            m_currPix = m_pix3;
        }
        this->update();
    });

    pBoomAnimation->start();

    this->update();
}

void FinishDlg::initialize()
{
    this->setWindowFlags(Qt::FramelessWindowHint | windowFlags()); // 隐藏默认边框

    m_pBtnHome->setProperty(DEF_QSS_TYPE, "Button Home");
    m_pBtnRepeat->setProperty(DEF_QSS_TYPE, "Button Repeat");
    m_pBtnNext->setProperty(DEF_QSS_TYPE, "Button ArrowRight-Thin");

    m_pBtnHome->setToolTip("返回大厅");
    m_pBtnRepeat->setToolTip("重新开始");
    m_pBtnNext->setToolTip("下一关");

    m_pix0.load(":/Resource/UI/icon/Star/0-3.png");
    m_pix1.load(":/Resource/UI/icon/Star/1-3.png");
    m_pix2.load(":/Resource/UI/icon/Star/2-3.png");
    m_pix3.load(":/Resource/UI/icon/Star/3-3.png");

    m_pix0 = m_pix0.scaledToWidth(200, Qt::SmoothTransformation);
    m_pix1 = m_pix1.scaledToWidth(200, Qt::SmoothTransformation);
    m_pix2 = m_pix2.scaledToWidth(200, Qt::SmoothTransformation);
    m_pix3 = m_pix3.scaledToWidth(200, Qt::SmoothTransformation);

    m_currPix = m_pix0;

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
    painter.drawPixmap(10, 30, m_currPix);
}
