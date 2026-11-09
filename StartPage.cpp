#include "StartPage.h"
#include <QVBoxLayout>

StartPage::StartPage(QWidget *parent) {
    auto * layout = new QVBoxLayout(this);

    auto * label = new QLabel;
    auto * quizBtn = new QPushButton;

    label->setText("abacaba");
    quizBtn->setText("Начать Квиз");

    layout->addWidget(label);
    layout->addWidget(quizBtn);

    connect(quizBtn, &QPushButton::clicked, this, [this] {
        emit goToQuiz();
    });
}
