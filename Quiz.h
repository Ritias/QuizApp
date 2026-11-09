#pragma once
#include <QMainWindow>
#include <QStackedWidget>

#include "EndPage.h"
#include "StartPage.h"
#include "QuizPage.h"

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
};
