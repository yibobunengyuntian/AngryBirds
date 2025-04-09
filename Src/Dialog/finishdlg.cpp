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
