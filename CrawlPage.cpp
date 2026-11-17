#include "CrawlPage.h"
#include "StarWarsCrawl.h"

#include <QFrame>
#include <QPushButton>
#include <QVBoxLayout>

CrawlPage::CrawlPage(const QString &crawlHtml,
                     const QString &buttonText,
                     QWidget *parent)
    : QWidget(parent)
{
    setObjectName("crawlPage");
    setStyleSheet(R"(
        QWidget#crawlPage { background: #f8ffe5; }
        QFrame#crawlFrame {
            background: #000000;
            border: 2px solid #1b9aaa;
            border-radius: 12px;
        }
        QPushButton#crawlAction {
            color: #173b3b;
            background: #06d6a0;
            border: 1px solid #079f79;
            border-radius: 10px;
            padding: 12px 30px;
            font-size: 18px;
            font-weight: 800;
            min-width: 220px;
            min-height: 30px;
        }
        QPushButton#crawlAction:hover { background: #42e5b9; }
        QPushButton#crawlAction:pressed { background: #04bd8e; }
    )");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(28, 24, 28, 24);
    layout->setSpacing(18);

    auto *crawlFrame = new QFrame(this);
    crawlFrame->setObjectName("crawlFrame");
    crawlFrame->setMinimumHeight(220);
    auto *crawlLayout = new QVBoxLayout(crawlFrame);
    crawlLayout->setContentsMargins(0, 0, 0, 0);

    crawl_ = new StarWarsCrawl(crawlFrame);
    crawl_->setCrawlHtml(crawlHtml);
    crawlLayout->addWidget(crawl_);
    layout->addWidget(crawlFrame, 1);

    button_ = new QPushButton(buttonText, this);
    button_->setObjectName("crawlAction");
    button_->setCursor(Qt::PointingHandCursor);
    connect(button_, &QPushButton::clicked, this, &CrawlPage::proceed);
    layout->addWidget(button_, 0, Qt::AlignHCenter);
}
