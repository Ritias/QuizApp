#include "StartPage.h"
#include <QtMath>

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QResizeEvent>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QFont>
#include <QStyle>
#include <algorithm>

StartPage::StartPage(const QList<int> &availableQuizzes, QWidget *parent)
    : QWidget(parent), availableQuizzes(availableQuizzes)
{
    setObjectName("startPage");
    setStyleSheet(R"(
        QWidget#startPage { background: #f8ffe5; }
        QLabel#eyebrow { color: #1b9aaa; font-weight: 800; letter-spacing: 1px; }
        QLabel#pageTitle { color: #173b3b; font-weight: 800; }
        QLabel#pageSubtitle { color: #415d5e; }
        QPushButton#quizCard {
            color: #173b3b; text-align: left; padding: 22px; background: #ffffff;
            border: 2px solid #d5e2d7; border-radius: 12px; font-weight: 800; min-height: 118px;
        }
        QPushButton#quizCard:hover { background: #effcf5; border: 2px solid #1b9aaa; }
        QPushButton#quizCard:pressed { background: #dff8ed; }
        QPushButton#quizCard[completed="true"] {
            background: #d8f3dc; border: 2px solid #2d9b58; color: #155b35;
        }
        QPushButton#quizCard[completed="true"]:hover {
            background: #c6ebd0; border: 2px solid #21834a;
        }
        QPushButton#resultsButton {
            color: #173b3b; background: #ffc43d; border: 2px solid #d9a51f;
            border-radius: 10px; padding: 13px 24px; font-weight: 800; min-height: 24px;
        }
        QPushButton#resultsButton:hover { background: #ffcf60; }
    )");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(36, 30, 36, 30);
    layout->setSpacing(12);
    eyebrow = new QLabel("ОХРАНА ТРУДА  /  ПРОВЕРКА ЗНАНИЙ", this);
    eyebrow->setObjectName("eyebrow");
    label = new QLabel("БЕЗОПАСНОСТЬ НА ПРОИЗВОДСТВЕ", this);
    label->setObjectName("pageTitle");
    label->setWordWrap(true);
    subtitle = new QLabel("Выберите тест по охране труда и проверьте свои знания.", this);
    subtitle->setObjectName("pageSubtitle");
    subtitle->setWordWrap(true);
    layout->addWidget(eyebrow);
    layout->addWidget(label);
    layout->addWidget(subtitle);
    layout->addSpacing(12);

    auto *grid = new QGridLayout;
    grid->setContentsMargins(14, 12, 14, 12);
    grid->setHorizontalSpacing(22);
    grid->setVerticalSpacing(20);

    int position = 0;
    for (int quizNumber : availableQuizzes) {
        if (quizNumber < 1 || quizNumber > 4) continue;
        auto *quizBtn = new QPushButton(
            QString("ТЕСТ № %1\nОткрыть тест  →").arg(quizNumber, 2, 10, QChar('0')), this);
        quizBtn->setObjectName("quizCard");
        quizBtn->setProperty("completed", false);
        quizBtn->setMinimumSize(150, 118);
        quizBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        quizBtn->setCursor(Qt::PointingHandCursor);
        quizButtons.append(quizBtn);

        const QString filename = QString("questions_data_%1.json").arg(quizNumber);
        connect(quizBtn, &QPushButton::clicked, this, [quizNumber, this, filename] {
            emit goToQuiz(quizNumber, filename);
        });

        const int row = position / 2;
        const int column = (position % 2) * 3;
        grid->addWidget(quizBtn, row, column, 1, 3, Qt::AlignCenter);
        ++position;
    }
    for (int column = 0; column < 6; ++column) grid->setColumnStretch(column, 1);
    for (int row = 0; row < (quizButtons.size() + 1) / 2; ++row) grid->setRowStretch(row, 1);
    layout->addLayout(grid, 1);

    resultsButton = new QPushButton("ПОСМОТРЕТЬ ИТОГИ  →", this);
    resultsButton->setObjectName("resultsButton");
    resultsButton->setCursor(Qt::PointingHandCursor);
    resultsButton->setVisible(false);
    connect(resultsButton, &QPushButton::clicked, this, &StartPage::showFinalResults);
    layout->addWidget(resultsButton, 0, Qt::AlignHCenter);
}

void StartPage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    const int margin = qBound(20, width() / 28, 48);
    if (layout()) layout()->setContentsMargins(margin, qBound(18, height() / 24, 34), margin, qBound(18, height() / 24, 34));
    const int titleSize = qBound(24, width() / 32, 44);
    label->setFont(QFont("Arial", titleSize, QFont::Bold));
    eyebrow->setFont(QFont("Arial", qBound(12, width() / 75, 16), QFont::Bold));
    subtitle->setFont(QFont("Arial", qBound(15, width() / 65, 22)));
    const int columns = qMin(2, quizButtons.size());
    const int cardSize = qBound(18, width() / 44, 30);
    const int cardWidth = qMax(176, qMin(500, (width() - 2 * margin - 70) / columns - 14));
    const int rows = (quizButtons.size() + 1) / 2;
    const int cardHeight = qMax(132, qMin(230, (height() - 210) / qMax(1, rows)));
    for (auto *button : quizButtons) {
        button->setFont(QFont("Arial", cardSize, QFont::Bold));
        button->setMinimumSize(qMax(150, cardWidth * 9 / 10), qMax(118, cardHeight * 9 / 10));
        button->setMaximumSize(cardWidth, cardHeight);
    }
}

void StartPage::markQuizCompleted(int quizNumber)
{
    if (!completedQuizzes.contains(quizNumber))
        completedQuizzes.append(quizNumber);

    const bool allCompleted = !availableQuizzes.isEmpty()
        && std::all_of(availableQuizzes.cbegin(), availableQuizzes.cend(),
                       [this](int number) { return completedQuizzes.contains(number); });
    resultsButton->setVisible(allCompleted);

    for (auto *button : quizButtons) {
        if (button->text().contains(QString("ТЕСТ № %1").arg(quizNumber, 2, 10, QChar('0')))) {
            button->setProperty("completed", true);
            button->style()->unpolish(button);
            button->style()->polish(button);
            button->update();
            break;
        }
    }
}
