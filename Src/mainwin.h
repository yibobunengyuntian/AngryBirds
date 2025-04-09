#ifndef MAINWIN_H
#define MAINWIN_H

#include <QWidget>
#include <QPushButton>
#include <QMouseEvent>
#include <QTimer>

#include "ui_mainwin.h"
#include "view.h"
#include "stopdlg.h"
#include "homewgt.h"
#include "maskwgt.h"


class MainWin : public QWidget, public Ui_MainWin
{
    Q_OBJECT

public:
    explicit MainWin(QWidget *parent = nullptr);
    ~MainWin();

protected:
    void initialize();
    void startGame(uint level = 0);

    void resizeEvent(QResizeEvent *event) override;

protected slots:
    void onNewGame();
    void onSelectLevel();
    void onExit();
    void onHome();
    void onResume();
    void onRepeat();
    void onStop();

    void onBeginContact(ItemBase *A, ItemBase *B, QPointF pos);

private:
    MaskWgt *m_pMaskWgt = nullptr;
    QPushButton *m_pBtnStop = nullptr;
    Scene *m_pScene = nullptr;
    View *m_pView = nullptr;
    StopDlg *m_pStopDlg = nullptr;
    HomeWgt *m_pHomeWgt = nullptr;

    QVariantList m_level;
    uint m_currLevel = 0;
};

#endif // MAINWIN_H
