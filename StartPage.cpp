#include "StartPage.h"
#include <QtMath>

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QResizeEvent>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QFont>

StartPage::StartPage(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("startPage");
    setStyleSheet(R"(
        QWidget#startPage {
            background: #f8ffe5;
        }
        QLabel#eyebrow {
            color: #1b9aaa;
            font-weight: 800;
            letter-spacing: 1px;
        }
        QLabel#pageTitle {
            color: #173b3b;
            font-weight: 800;
        }
        QLabel#pageSubtitle {
            color: #415d5e;
        }
        QPushButton#quizCard {
            color: #173b3b;
            text-align: left;
            padding: 22px;
            background: #ffffff;
            border: 2px solid #d5e2d7;
            border-radius: 12px;
            font-weight: 800;
            min-height: 118px;
        }
        QPushButton#quizCard:hover {
            background: #effcf5;
            border: 2px solid #1b9aaa;
        }
        QPushButton#quizCard:pressed {
            background: #dff8ed;
        }
    )");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(36, 30, 36, 30);
    layout->setSpacing(12);

    eyebrow = new QLabel("ОХРАНА ТРУДА  /  ПРОВЕРКА ЗНАНИЙ", this);
    eyebrow->setObjectName("eyebrow");

    label = new QLabel("БЕЗОПАСНОСТЬ НА ПРОИЗВОДСТВЕ", this);
    label->setObjectName("pageTitle");
    label->setWordWrap(true);

    subtitle = new QLabel(
        "Выберите тест по охране труда и проверьте свои знания.",
        this);
    subtitle->setObjectName("pageSubtitle");
    subtitle->setWordWrap(true);

    layout->addWidget(eyebrow);
    layout->addWidget(label);
    layout->addWidget(subtitle);
    layout->addSpacing(12);

    auto *grid = new QGridLayout;
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setHorizontalSpacing(14);
    grid->setVerticalSpacing(14);

    for (int i = 1; i <= 5; ++i) {
        auto *quizBtn = new QPushButton(
            QString("ТЕСТ № %1\nНачать проверку  →")
                .arg(i, 2, 10, QChar('0')),
            this);
        quizBtn->setObjectName("quizCard");
        quizBtn->setMinimumSize(150, 118);
        quizBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        quizBtn->setCursor(Qt::PointingHandCursor);
        quizButtons.append(quizBtn);

        const QString filename = QString("questions_data_%1.json").arg(i);
        connect(quizBtn, &QPushButton::clicked, this, [this, filename] {
            emit goToQuiz(filename);
        });

        // Three cards in the first row; the remaining two are centered.
        const int row = (i <= 3) ? 0 : 1;
        const int column = (i <= 3) ? (i - 1) * 2 : 1 + (i - 4) * 2;
        grid->addWidget(quizBtn, row, column, 1, 2);
    }

    for (int column = 0; column < 6; ++column)
        grid->setColumnStretch(column, 1);
    grid->setRowStretch(0, 1);
    grid->setRowStretch(1, 1);

    layout->addLayout(grid, 1);
}

void StartPage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    const int margin = qBound(20, width() / 28, 48);
    if (layout())
        layout()->setContentsMargins(margin, qBound(18, height() / 24, 34),
                                     margin, qBound(18, height() / 24, 34));

    const int titleSize = qBound(24, width() / 32, 44);
    label->setFont(QFont("Arial", titleSize, QFont::Bold));
    eyebrow->setFont(QFont("Arial", qBound(12, width() / 75, 16), QFont::Bold));
    subtitle->setFont(QFont("Arial", qBound(15, width() / 65, 22)));

    const int cardSize = qBound(18, width() / 44, 30);
    for (auto *button : quizButtons)
        button->setFont(QFont("Arial", cardSize, QFont::Bold));
}
