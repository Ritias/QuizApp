#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QList>

#include "results.h"
#include "EndPage.h"
#include "StartPage.h"
#include "QuizPage.h"

class Quiz : public QMainWindow
{
    Q_OBJECT
public:
    explicit Quiz(QWidget* parent = nullptr);
private:
    void setupUI(const QList<int> &availableQuizzes);
    void setupConnections();
    QStackedWidget* stack;
    StartPage* start;
    QuizPage* quiz;
    EndPage* end;
    Results res;
    int currentQuizNumber = 0;
private slots:
    void onGoToQuiz(int, const QString &file);
};
