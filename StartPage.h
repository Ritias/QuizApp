#pragma once

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>


class StartPage : public QWidget {
    Q_OBJECT
public:
    explicit StartPage(QWidget *parent = nullptr);
    signals:
    void goToQuiz();
private:
    QLabel *label;
    QPushButton *quizBtn;
};
