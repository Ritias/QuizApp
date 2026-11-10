#include "StartPage.h"
#include <QVBoxLayout>

StartPage::StartPage(QWidget *parent) {
    auto * layout = new QVBoxLayout(this);
    label = new QLabel;
    label->setText("abacaba");
    layout->addWidget(label);
    for (int i = 1; i <= 5; ++i) {
        auto * quizBtn = new QPushButton(QString("Квиз %1").arg(i), this);
        const QString name = QString("questions_data_%1.json").arg(i);
        connect(quizBtn, &QPushButton::clicked, this, [this, name] {
            emit goToQuiz(name);
        });
        layout->addWidget(quizBtn);
    }
}
