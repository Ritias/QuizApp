#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QList>
#include <QString>

#include "results.h"
#include "EndPage.h"
#include "StartPage.h"
#include "QuizPage.h"
#include "CrawlPage.h"

class Quiz : public QMainWindow
{
    Q_OBJECT
public:
    explicit Quiz(QWidget* parent = nullptr);

private:
    void setupUI();
    void setupConnections();
    void showQuizSelection();
    void startPendingQuiz();
    void showPostQuizChoice();

    QStackedWidget* stack = nullptr;
    StartPage* start = nullptr;
    CrawlPage* introTitles = nullptr;
    QuizPage* quiz = nullptr;
    EndPage* end = nullptr;
    CrawlPage* finalTitles = nullptr;

    Results res;
    QList<int> availableQuizzes;
    int currentQuizNumber = 0;
    QString currentQuizFile;

private slots:
    void onGoToQuiz(int, const QString &file);
};
