#include "Quiz.h"
#include "QuizPage.h"
#include <QVBoxLayout>

Quiz::Quiz(QWidget* parent) : QMainWindow(parent) {
    setupUI();
    setupConnections();
}
void Quiz::setupUI() {
    resize(800, 600);
    auto *central = new QWidget(this);
    setCentralWidget(central);

    auto *rootLayout = new QVBoxLayout(central);

    stack = new QStackedWidget(this);
    start = new StartPage(this);
    quiz = new QuizPage(this);
    end = new EndPage(this);

    stack->addWidget(start);
    stack->addWidget(quiz);
    stack->addWidget(end);

    rootLayout->addWidget(stack);
    stack->setCurrentWidget(start);
}
void Quiz::setupConnections() {
    connect(start, &StartPage::goToQuiz, this, &Quiz::onGoToQuiz);
    connect(quiz, &QuizPage::goToEnd, this, [this]() {
        stack->setCurrentWidget(end);
    });
    connect(end, &EndPage::goToStart, this, [this]() {
        stack->setCurrentWidget(start);
    });
}
void Quiz::onGoToQuiz(const QString &file) {
    quiz->loadQuiz(file);
    stack->setCurrentWidget(quiz);
}
