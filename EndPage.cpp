#include "EndPage.h"
#include "StarWarsCrawl.h"

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

EndPage::EndPage(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("endPage");
    setStyleSheet(R"(
        QWidget#endPage {
            background: #f8ffe5;
        }
        QLabel#endTitle {
            color: #173b3b;
            font-size: 28px;
            font-weight: 900;
        }
        QLabel#endSubtitle {
            color: #527071;
            font-size: 14px;
        }
        QFrame#crawlFrame {
            background: #000000;
            border: 3px solid #1b9aaa;
            border-radius: 16px;
        }
        QPushButton#menuButton {
            color: #173b3b;
            background: #06d6a0;
            border: none;
            border-radius: 15px;
            padding: 13px 28px;
            font-size: 14px;
            font-weight: 800;
        }
        QPushButton#menuButton:hover {
            background: #42e5b9;
        }
    )");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(42, 24, 42, 24);
    layout->setSpacing(12);

    auto *title = new QLabel("МИССИЯ ВЫПОЛНЕНА", this);
    title->setObjectName("endTitle");
    title->setAlignment(Qt::AlignCenter);

    auto *subtitle = new QLabel(
        "История твоего путешествия ещё не закончена…", this);
    subtitle->setObjectName("endSubtitle");
    subtitle->setAlignment(Qt::AlignCenter);

    auto *crawlFrame = new QFrame(this);
    crawlFrame->setObjectName("crawlFrame");
    auto *crawlLayout = new QVBoxLayout(crawlFrame);
    crawlLayout->setContentsMargins(0, 0, 0, 0);

    auto *swc = new StarWarsCrawl(crawlFrame);
    crawlLayout->addWidget(swc);

    auto *menu = new QPushButton("←  ГЛАВНОЕ МЕНЮ", this);
    menu->setObjectName("menuButton");
    menu->setCursor(Qt::PointingHandCursor);
    connect(menu, &QPushButton::clicked, this, [this] {
        emit goToStart();
    });

    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addWidget(crawlFrame, 1, Qt::AlignHCenter);
    layout->addWidget(menu, 0, Qt::AlignHCenter);
}
