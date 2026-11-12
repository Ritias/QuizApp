#pragma once
#include <QMainWindow>
#include <QStackedWidget>

#include "results.h"
#include "EndPage.h"
#include "StartPage.h"
#include "QuizPage.h"
#include<QDebug>
class Quiz : public QMainWindow
{
    Q_OBJECT

public:
    explicit Quiz(QWidget* parent = nullptr);
private:
    void setupUI();
    void setupConnections();
    QStackedWidget* stack;
    StartPage* start;
    QuizPage* quiz;
    EndPage* end;
    Results res;
private slots:
    void onGoToQuiz(int,const QString &file);
};
