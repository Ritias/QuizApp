#include "EndPage.h"

#include <QFont>
#include <QHeaderView>
#include <QPushButton>
#include <QResizeEvent>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QMap>

EndPage::EndPage(QWidget *parent) : QWidget(parent)
{
    setObjectName("endPage");
    setStyleSheet(R"(
        QWidget#endPage { background: #f8ffe5; }
        QLabel#endTitle { color: #173b3b; font-weight: 800; }
        QLabel#endSubtitle { color: #415d5e; }
        QTableWidget {
            background: #ffffff;
            color: #173b3b;
            gridline-color: #d5e2d7;
            border: 1px solid #d5e2d7;
            border-radius: 8px;
        }
        QHeaderView::section {
            background: #1b9aaa;
            color: #ffffff;
            padding: 10px;
            font-weight: 700;
            border: none;
        }
        QPushButton#finishButton {
            color: #173b3b;
            background: #06d6a0;
            border: 1px solid #079f79;
            border-radius: 10px;
            padding: 12px 30px;
            font-weight: 800;
            min-height: 28px;
        }
        QPushButton#finishButton:hover { background: #42e5b9; }
    )");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(34, 26, 34, 26);
    layout->setSpacing(16);

    titleLabel = new QLabel("ИТОГИ СЕССИИ", this);
    titleLabel->setObjectName("endTitle");
    titleLabel->setAlignment(Qt::AlignCenter);

    subtitleLabel = new QLabel(
        "Результаты по выбранным тестам. Незавершённые тесты отмечены отдельно.",
        this);
    subtitleLabel->setObjectName("endSubtitle");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setWordWrap(true);

    resultsTable = new QTableWidget(this);
    resultsTable->setObjectName("resultsTable");
    resultsTable->setColumnCount(4);
    resultsTable->setHorizontalHeaderLabels({"Тест", "Статус", "Ошибки", "Время"});
    resultsTable->verticalHeader()->hide();
    resultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    resultsTable->setSelectionMode(QAbstractItemView::NoSelection);
    resultsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    resultsTable->setAlternatingRowColors(true);
    resultsTable->setStyleSheet(
        "QTableWidget { alternate-background-color: #f0f8ed; }");
    resultsTable->setMinimumHeight(220);

    finishButton = new QPushButton("ЗАВЕРШИТЬ  →", this);
    finishButton->setObjectName("finishButton");
    finishButton->setCursor(Qt::PointingHandCursor);
    connect(finishButton, &QPushButton::clicked, this, &EndPage::showFinalTitles);

    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    layout->addWidget(resultsTable, 1);
    layout->addWidget(finishButton, 0, Qt::AlignHCenter);
}

void EndPage::setResults(const QVector<Result> &results,
                         const QList<int> &selectedQuizzes)
{
    QMap<int, Result> latest;
    for (const Result &result : results)
        latest.insert(result.quizNumber, result);

    resultsTable->setRowCount(selectedQuizzes.size());
    for (int row = 0; row < selectedQuizzes.size(); ++row) {
        const int quizNumber = selectedQuizzes.at(row);
        const bool completed = latest.contains(quizNumber);

        const QString status = completed ? "Завершён" : "Не пройден";
        const QString errors = completed
            ? QString::number(latest.value(quizNumber).countWrong)
            : "—";
        const QString time = completed
            ? latest.value(quizNumber).timeString()
            : "—";

        const QList<QString> values = {
            QString("Тест № %1").arg(quizNumber),
            status,
            errors,
            time
        };

        for (int column = 0; column < values.size(); ++column) {
            auto *item = new QTableWidgetItem(values.at(column));
            item->setTextAlignment(Qt::AlignCenter);
            resultsTable->setItem(row, column, item);
        }
    }
}

void EndPage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    const qreal scale = qMin(width() / 1024.0, height() / 768.0);
    titleLabel->setFont(QFont("Arial", qBound(22, qRound(30 * scale), 38), QFont::Bold));
    subtitleLabel->setFont(QFont("Arial", qBound(14, qRound(16 * scale), 22)));
    finishButton->setFont(QFont("Arial", qBound(15, qRound(18 * scale), 22), QFont::Bold));
    finishButton->setMinimumHeight(qBound(48, height() / 12, 68));
}
