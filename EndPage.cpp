#include "EndPage.h"
#include "StarWarsCrawl.h"

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
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
            font-weight: 800;
        }
        QLabel#endSubtitle {
            color: #415d5e;
            font-size: 15px;
        }
        QFrame#crawlFrame {
            background: #000000;
            border: 2px solid #1b9aaa;
            border-radius: 12px;
        }
        QPushButton#menuButton {
            color: #173b3b;
            background: #06d6a0;
            border: 1px solid #079f79;
            border-radius: 10px;
            padding: 14px 30px;
            font-size: 16px;
            font-weight: 800;
            min-height: 28px;
        }
        QPushButton#menuButton:hover {
            background: #42e5b9;
        }
    )");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(32, 20, 32, 20);
    layout->setSpacing(10);

    auto *title = new QLabel("ТЕСТИРОВАНИЕ ЗАВЕРШЕНО", this);
    title->setObjectName("endTitle");
    title->setAlignment(Qt::AlignCenter);

    auto *subtitle = new QLabel(
        "Спасибо за прохождение проверки знаний по охране труда.", this);
    subtitle->setObjectName("endSubtitle");
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setWordWrap(true);

    auto *crawlFrame = new QFrame(this);
    crawlFrame->setObjectName("crawlFrame");
    crawlFrame->setMinimumHeight(180);
    crawlFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *crawlLayout = new QVBoxLayout(crawlFrame);
    crawlLayout->setContentsMargins(0, 0, 0, 0);
    auto *swc = new StarWarsCrawl(crawlFrame);
    swc->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    crawlLayout->addWidget(swc);

    auto *menu = new QPushButton("←  ВЕРНУТЬСЯ К ВЫБОРУ ТЕСТА", this);
    menu->setObjectName("menuButton");
    menu->setCursor(Qt::PointingHandCursor);
    connect(menu, &QPushButton::clicked, this, [this] {
        emit goToStart();
    });

    layout->addWidget(title);
    layout->addWidget(subtitle);
    // Do not center the frame at its size hint: let it fill the wide page.
    layout->addWidget(crawlFrame, 1);
    layout->addWidget(menu, 0, Qt::AlignHCenter);
}
