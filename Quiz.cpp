#include "Quiz.h"
#include "QuizPage.h"

#include <QApplication>
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

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
    layout->addWidget(buttons, 0, Qt::AlignRight);

    auto updateSelectionState = [checks, buttons]() {
        int selected = 0;
        for (const auto *check : checks)
            if (check->isChecked()) ++selected;
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

const QString introCrawlHtml = QString::fromUtf8(R"HTML(
    <body>
        <p align="center"><b>ЭПИЗОД I</b></p>
        <p align="center"><b>ПРОВЕРКА НАЧИНАЕТСЯ</b></p>
        <p>Давным-давно, на одном далёком производстве...</p>
        <p>Перед вами проверка знаний требований охраны труда. Впереди вопросы о безопасной работе, внимательности и ответственности.</p>
        <p>Читайте задания внимательно, оценивайте возможные риски и выбирайте действия, которые помогают сохранить здоровье и жизнь.</p>
        <p>Подготовьтесь к началу испытания.</p>
    </body>
)HTML");

const QString finalCrawlHtml = QString::fromUtf8(R"HTML(
    <body>
        <p align="center"><b>ЭПИЗОД II</b></p>
        <p align="center"><b>МИССИЯ ЗАВЕРШЕНА</b></p>
        <p>Последний ответ дан. Результаты проверки знаний собраны и сохранены в рамках этой сессии.</p>
        <p>Но безопасность не заканчивается вместе с тестированием. Каждый день требует внимания, соблюдения правил и готовности предупредить опасность.</p>
        <p>Берегите себя и коллег. Пусть безопасная работа станет привычкой.</p>
        <p align="center"><b>КОНЕЦ ПЕРЕДАЧИ</b></p>
    </body>
)HTML");
}

Quiz::Quiz(QWidget *parent) : QMainWindow(parent)
{
    availableQuizzes = chooseQuizzes(this);
    setupUI(availableQuizzes);
    setupConnections();
}

void Quiz::setupUI(const QList<int> &selectedQuizzes)
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
    start = new StartPage(selectedQuizzes, stack);
    introTitles = new CrawlPage(introCrawlHtml, "ПЕРЕЙТИ К ВЫБОРУ ТЕСТА  →", stack);
    quiz = new QuizPage(stack);
    end = new EndPage(stack);
    finalTitles = new CrawlPage(finalCrawlHtml, "ЗАКРЫТЬ ПРИЛОЖЕНИЕ", stack);

    stack->addWidget(start);
    stack->addWidget(introTitles);
    stack->addWidget(quiz);
    stack->addWidget(end);
    stack->addWidget(finalTitles);

    rootLayout->addWidget(stack);
    stack->setCurrentWidget(introTitles);
}

void Quiz::setupConnections()
{
    connect(start, &StartPage::goToQuiz, this, &Quiz::onGoToQuiz);
    connect(introTitles, &CrawlPage::proceed, this, &Quiz::showQuizMenu);

    connect(quiz, &QuizPage::WrongAnsGive, this, [this] {
        res.addWrong();
    });
    connect(quiz, &QuizPage::quizFinish, this, [this] {
        res.end();
        start->markQuizCompleted(currentQuizNumber);
        end->setResults(res.getRows(), availableQuizzes);
    });
    connect(quiz, &QuizPage::goToEnd, this, &Quiz::showPostQuizChoice);

    connect(end, &EndPage::showFinalTitles, this, [this] {
        stack->setCurrentWidget(finalTitles);
    });
    connect(finalTitles, &CrawlPage::proceed, this, [] {
        QApplication::quit();
    });
}

void Quiz::showQuizMenu()
{
    stack->setCurrentWidget(start);
}

void Quiz::onGoToQuiz(int num, const QString &file)
{
    currentQuizNumber = num;
    currentQuizFile = file;
    startPendingQuiz();
}

void Quiz::startPendingQuiz()
{
    if (currentQuizNumber < 1 || currentQuizFile.isEmpty()) {
        stack->setCurrentWidget(start);
        return;
    }

    res.start(currentQuizNumber);
    quiz->loadQuiz(currentQuizFile);
    stack->setCurrentWidget(quiz);
}

void Quiz::showPostQuizChoice()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Тест завершён");
    dialog.setModal(true);
    dialog.setMinimumWidth(450);
    dialog.setStyleSheet(R"(
        QDialog { background: #f8ffe5; }
        QLabel { color: #173b3b; }
        QLabel#dialogTitle { font-size: 20px; font-weight: 800; }
        QLabel#dialogText { color: #415d5e; font-size: 14px; }
        QPushButton {
            color: #173b3b; background: #06d6a0; border: 1px solid #079f79;
            border-radius: 9px; padding: 11px 20px; font-weight: 800; min-height: 28px;
        }
        QPushButton:hover { background: #42e5b9; }
        QPushButton#finishAllButton {
            background: #ffc43d; border-color: #d9a51f;
        }
        QPushButton#finishAllButton:hover { background: #ffcf60; }
    )");

    auto *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(26, 24, 26, 24);
    layout->setSpacing(14);

    auto *title = new QLabel("ТЕСТ ЗАВЕРШЁН", &dialog);
    title->setObjectName("dialogTitle");
    auto *message = new QLabel(
        "Вы можете вернуться к выбору тестов или завершить всю сессию и посмотреть итоговую таблицу.",
        &dialog);
    message->setObjectName("dialogText");
    message->setWordWrap(true);
    layout->addWidget(title);
    layout->addWidget(message);

    auto *buttons = new QHBoxLayout;
    auto *menuButton = new QPushButton("ВЕРНУТЬСЯ В МЕНЮ", &dialog);
    auto *finishButton = new QPushButton("ЗАВЕРШИТЬ ВСЁ", &dialog);
    finishButton->setObjectName("finishAllButton");
    menuButton->setCursor(Qt::PointingHandCursor);
    finishButton->setCursor(Qt::PointingHandCursor);

    connect(menuButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    connect(finishButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    buttons->addWidget(menuButton);
    buttons->addWidget(finishButton);
    layout->addLayout(buttons);

    if (dialog.exec() == QDialog::Accepted)
        stack->setCurrentWidget(end);
    else
        stack->setCurrentWidget(start);
}
