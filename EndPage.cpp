#include "EndPage.h"
#include <QtMath>
#include "StarWarsCrawl.h"

#include <QDialog>
#include <QFrame>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QResizeEvent>
#include <QFont>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMap>

EndPage::EndPage(QWidget *parent) : QWidget(parent)
{
    setObjectName("endPage");
    setStyleSheet(R"(
        QWidget#endPage { background: #f8ffe5; }
        QLabel#endTitle { color: #173b3b; font-weight: 800; }
        QLabel#endSubtitle { color: #415d5e; }
        QFrame#crawlFrame { background: #000000; border: 2px solid #1b9aaa; border-radius: 12px; }
        QPushButton#menuButton, QPushButton#resultsButton {
            color: #173b3b; background: #06d6a0; border: 1px solid #079f79;
            border-radius: 10px; padding: 12px 26px; font-weight: 800; min-height: 28px;
        }
        QPushButton#menuButton:hover, QPushButton#resultsButton:hover { background: #42e5b9; }
    )");
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(32, 20, 32, 20);
    layout->setSpacing(10);
    titleLabel = new QLabel("ТЕСТИРОВАНИЕ ЗАВЕРШЕНО", this);
    titleLabel->setObjectName("endTitle");
    titleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel = new QLabel("Спасибо за прохождение проверки знаний по охране труда.", this);
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

    resultsButton = new QPushButton("РЕЗУЛЬТАТЫ", this);
    resultsButton->setObjectName("resultsButton");
    resultsButton->setCursor(Qt::PointingHandCursor);
    connect(resultsButton, &QPushButton::clicked, this, &EndPage::showResults);

    menuButton = new QPushButton("←  ВЕРНУТЬСЯ К ВЫБОРУ ТЕСТА", this);
    menuButton->setObjectName("menuButton");
    menuButton->setCursor(Qt::PointingHandCursor);
    connect(menuButton, &QPushButton::clicked, this, [this] { emit goToStart(); });

    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    layout->addWidget(crawlFrame, 1);
    auto *buttons = new QHBoxLayout;
    buttons->addStretch();
    buttons->addWidget(resultsButton);
    buttons->addWidget(menuButton);
    buttons->addStretch();
    layout->addLayout(buttons);
}

void EndPage::setResults(const QVector<Result> &results)
{
    quizResults = results;
}

void EndPage::showResults()
{
    QMap<int, Result> latest;
    for (const Result &result : quizResults)
        latest.insert(result.quizNumber, result);

    QDialog dialog(this);
    dialog.setWindowTitle("Результаты прохождения тестов");
    dialog.setMinimumSize(520, 300);
    dialog.setStyleSheet(R"(
        QDialog { background: #f8ffe5; }
        QLabel#dialogTitle { color: #173b3b; font-size: 19px; font-weight: 800; }
        QTableWidget { background: white; color: #173b3b; gridline-color: #d5e2d7; border: 1px solid #d5e2d7; }
        QHeaderView::section { background: #1b9aaa; color: white; padding: 8px; font-weight: 700; border: none; }
        QPushButton { background: #06d6a0; color: #173b3b; border: 1px solid #079f79; border-radius: 8px; padding: 9px 24px; font-weight: 700; }
    )");
    auto *layout = new QVBoxLayout(&dialog);
    auto *heading = new QLabel("ПОСЛЕДНИЕ РЕЗУЛЬТАТЫ ПО КАЖДОМУ ТЕСТУ", &dialog);
    heading->setObjectName("dialogTitle");
    heading->setWordWrap(true);
    layout->addWidget(heading);

    auto *table = new QTableWidget(latest.count(), 3, &dialog);
    table->setHorizontalHeaderLabels({"Тест", "Ошибки", "Время"});
    table->verticalHeader()->hide();
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setAlternatingRowColors(true);
    table->setStyleSheet("QTableWidget { alternate-background-color: #f0f8ed; }");
    for (int quiz = 1; quiz <= latest.count(); ++quiz) {
        table->setItem(quiz - 1, 0, new QTableWidgetItem(QString("Тест № %1").arg(quiz)));
        const Result r = latest.value(quiz);
        table->setItem(quiz - 1, 1, new QTableWidgetItem(QString::number(r.countWrong)));
        table->setItem(quiz - 1, 2, new QTableWidgetItem(r.timeString()));
        for (int col = 0; col < 3; ++col)
            table->item(quiz - 1, col)->setTextAlignment(Qt::AlignCenter);
    }
    layout->addWidget(table);
    auto *close = new QPushButton("ЗАКРЫТЬ", &dialog);
    connect(close, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout->addWidget(close, 0, Qt::AlignRight);
    dialog.exec();
}

void EndPage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    const qreal scale = qMin(width() / 1024.0, height() / 768.0);
    titleLabel->setFont(QFont("Arial", qBound(20, qRound(30 * scale), 38), QFont::Bold));
    subtitleLabel->setFont(QFont("Arial", qBound(14, qRound(16 * scale), 22)));
    menuButton->setFont(QFont("Arial", qBound(15, qRound(17 * scale), 22), QFont::Bold));
    resultsButton->setFont(QFont("Arial", qBound(15, qRound(17 * scale), 22), QFont::Bold));
    menuButton->setMinimumHeight(qBound(48, height() / 12, 68));
    resultsButton->setMinimumHeight(qBound(48, height() / 12, 68));
}
