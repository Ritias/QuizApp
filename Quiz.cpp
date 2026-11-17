#include "Quiz.h"
#include "QuizPage.h"

#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

namespace {
QList<int> chooseQuizzes(QWidget *parent)
{
    QDialog dialog(parent);
    dialog.setWindowTitle("Выбор тестов");
    dialog.setModal(true);
    dialog.setMinimumWidth(390);
    dialog.setStyleSheet(R"(
        QDialog { background: #f8ffe5; }
        QLabel { color: #173b3b; }
        QLabel#dialogTitle { font-size: 19px; font-weight: 800; }
        QLabel#dialogHint { color: #415d5e; }
        QCheckBox { color: #173b3b; font-size: 15px; spacing: 10px; padding: 5px; }
        QCheckBox::indicator { width: 19px; height: 19px; }
        QPushButton {
            color: #173b3b; background: #06d6a0; border: 1px solid #079f79;
            border-radius: 8px; padding: 9px 24px; font-weight: 800; min-width: 100px;
        }
        QPushButton:disabled { color: #82918a; background: #dce7df; border-color: #c5d2c8; }
    )");

    auto *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(26, 22, 26, 22);
    layout->setSpacing(12);
    auto *title = new QLabel("ВЫБОР ДОСТУПНЫХ ТЕСТОВ", &dialog);
    title->setObjectName("dialogTitle");
    auto *hint = new QLabel("Отметьте тесты, которые будут доступны для прохождения. Необходимо выбрать минимум два.", &dialog);
    hint->setObjectName("dialogHint");
    hint->setWordWrap(true);
    layout->addWidget(title);
    layout->addWidget(hint);

    QList<QCheckBox*> checks;
    for (int i = 1; i <= 4; ++i) {
        auto *check = new QCheckBox(QString("Тест № %1").arg(i), &dialog);
        check->setChecked(true);
        checks.append(check);
        layout->addWidget(check);
    }

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok, &dialog);
    buttons->button(QDialogButtonBox::Ok)->setText("ПОДТВЕРДИТЬ");
    buttons->button(QDialogButtonBox::Ok)->setEnabled(true);
    layout->addWidget(buttons, 0, Qt::AlignRight);

    auto updateSelectionState = [checks, buttons]() {
        int selected = 0;
        for (const auto *check : checks) if (check->isChecked()) ++selected;
        buttons->button(QDialogButtonBox::Ok)->setEnabled(selected >= 2);
    };
    for (auto *check : checks)
        QObject::connect(check, &QCheckBox::toggled, &dialog, updateSelectionState);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);

    if (dialog.exec() != QDialog::Accepted)
        return {1, 2, 3, 4};

    QList<int> selected;
    for (int i = 0; i < checks.size(); ++i)
        if (checks[i]->isChecked()) selected.append(i + 1);
    return selected.size() >= 2 ? selected : QList<int>{1, 2, 3, 4};
}
}

Quiz::Quiz(QWidget *parent) : QMainWindow(parent)
{
    const QList<int> availableQuizzes = chooseQuizzes(this);
    setupUI(availableQuizzes);
    setupConnections();
}

void Quiz::setupUI(const QList<int> &availableQuizzes)
{
    resize(1024, 768);
    setMinimumSize(680, 580);
    setWindowTitle("Охрана труда — проверка знаний");
    auto *central = new QWidget(this);
    central->setStyleSheet("background: #f8ffe5;");
    setCentralWidget(central);
    auto *rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    stack = new QStackedWidget(central);
    start = new StartPage(availableQuizzes, stack);
    quiz = new QuizPage(stack);
    end = new EndPage(stack);
    stack->addWidget(start);
    stack->addWidget(quiz);
    stack->addWidget(end);
    rootLayout->addWidget(stack);
    stack->setCurrentWidget(start);
}

void Quiz::setupConnections()
{
    connect(start, &StartPage::goToQuiz, this, &Quiz::onGoToQuiz);
    connect(quiz, &QuizPage::goToEnd, this, [this] { stack->setCurrentWidget(end); });
    connect(end, &EndPage::goToStart, this, [this] { stack->setCurrentWidget(start); });
    connect(quiz, &QuizPage::WrongAnsGive, this, [this] { res.addWrong(); });
    connect(quiz, &QuizPage::quizFinish, this, [this] {
        res.end();
        end->setResults(res.getRows());
    });
}

void Quiz::onGoToQuiz(int num, const QString &file)
{
    res.start(num);
    quiz->loadQuiz(file);
    stack->setCurrentWidget(quiz);
}
