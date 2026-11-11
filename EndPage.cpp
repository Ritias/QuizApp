#include "EndPage.h"
#include "StarWarsCrawl.h"

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QResizeEvent>
#include <QFont>
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

    titleLabel = new QLabel("ТЕСТИРОВАНИЕ ЗАВЕРШЕНО", this);
    titleLabel->setObjectName("endTitle");
    titleLabel->setAlignment(Qt::AlignCenter);

    subtitleLabel = new QLabel(
        "Спасибо за прохождение проверки знаний по охране труда.", this);
    subtitleLabel->setObjectName("endSubtitle");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setWordWrap(true);

    auto *crawlFrame = new QFrame(this);
    crawlFrame->setObjectName("crawlFrame");
    crawlFrame->setMinimumHeight(180);
    crawlFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *crawlLayout = new QVBoxLayout(crawlFrame);
    crawlLayout->setContentsMargins(0, 0, 0, 0);
    auto *swc = new StarWarsCrawl(crawlFrame);
    swc->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    crawlLayout->addWidget(swc);

    menuButton = new QPushButton("←  ВЕРНУТЬСЯ К ВЫБОРУ ТЕСТА", this);
    menuButton->setObjectName("menuButton");
    menuButton->setCursor(Qt::PointingHandCursor);
    connect(menuButton, &QPushButton::clicked, this, [this] {
        emit goToStart();
    });

    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    // Do not center the frame at its size hint: let it fill the wide page.
    layout->addWidget(crawlFrame, 1);
    layout->addWidget(menuButton, 0, Qt::AlignHCenter);
}

void EndPage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    const qreal scale = qMin(width() / 1024.0, height() / 768.0);
    titleLabel->setFont(QFont("Arial", qBound(20, qRound(30 * scale), 38), QFont::Bold));
    subtitleLabel->setFont(QFont("Arial", qBound(14, qRound(16 * scale), 22)));
    menuButton->setFont(QFont("Arial", qBound(15, qRound(17 * scale), 22), QFont::Bold));
    menuButton->setMinimumHeight(qBound(48, height() / 12, 68));
}
