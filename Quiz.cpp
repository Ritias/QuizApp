#include "Quiz.h"
#include "QuizPage.h"

#include <QVBoxLayout>

Quiz::Quiz(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setupConnections();
}

void Quiz::setupUI()
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
    start = new StartPage(stack);
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
    connect(quiz, &QuizPage::goToEnd, this, [this] {
        stack->setCurrentWidget(end);
    });
    connect(end, &EndPage::goToStart, this, [this] {
        stack->setCurrentWidget(start);
    });
}

void Quiz::onGoToQuiz(const QString &file)
{
    quiz->loadQuiz(file);
    stack->setCurrentWidget(quiz);
}
