#include "StartPage.h"

#include <QFont>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

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
            font-size: 12px;
            font-weight: 800;
            letter-spacing: 2px;
        }
        QLabel#pageTitle {
            color: #173b3b;
            font-size: 34px;
            font-weight: 900;
        }
        QLabel#pageSubtitle {
            color: #527071;
            font-size: 15px;
        }
        QPushButton#quizCard {
            color: #173b3b;
            text-align: left;
            padding: 22px;
            border: none;
            border-radius: 22px;
            font-size: 19px;
            font-weight: 800;
        }
        QPushButton#quizCard:hover {
            border: 3px solid #173b3b;
        }
        QPushButton#quizCard:pressed {
            padding-top: 25px;
            padding-left: 25px;
        }
    )");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(48, 42, 48, 42);
    layout->setSpacing(14);

    auto *eyebrow = new QLabel("QUIZ GALAXY  /  KNOWLEDGE ARCADE", this);
    eyebrow->setObjectName("eyebrow");

    label = new QLabel("ВЫБЕРИ СВОЙ КВИЗ", this);
    label->setObjectName("pageTitle");

    auto *subtitle = new QLabel(
        "Пять вариантов. Новые открытия. Твой выход на орбиту знаний.",
        this);
    subtitle->setObjectName("pageSubtitle");

    layout->addWidget(eyebrow);
    layout->addWidget(label);
    layout->addWidget(subtitle);
    layout->addSpacing(18);

    auto *grid = new QGridLayout;
    grid->setHorizontalSpacing(18);
    grid->setVerticalSpacing(18);

    const QStringList cardColors = {
        "#06d6a0", "#ffc43d", "#1b9aaa", "#ef476f", "#06d6a0"
    };

    for (int i = 1; i <= 5; ++i) {
        auto *quizBtn = new QPushButton(
            QString("КВИЗ  %1\nНачать приключение  →").arg(i, 2, 10, QChar('0')),
            this);
        quizBtn->setObjectName("quizCard");
        quizBtn->setMinimumSize(205, 118);
        quizBtn->setCursor(Qt::PointingHandCursor);
        quizBtn->setStyleSheet(QString(
            "QPushButton#quizCard { background-color: %1; }"
            "QPushButton#quizCard:hover { background-color: %2; }")
            .arg(cardColors.at(i - 1),
                 cardColors.at(i - 1)));

        const QString filename = QString("questions_data_%1.json").arg(i);
        connect(quizBtn, &QPushButton::clicked, this, [this, filename] {
            emit goToQuiz(filename);
        });

        const int row = (i - 1) / 3;
        const int column = (i - 1) % 3;
        grid->addWidget(quizBtn, row, column);
    }

    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);
    grid->setColumnStretch(2, 1);

    layout->addLayout(grid);
    layout->addStretch();
}
