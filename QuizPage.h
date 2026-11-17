#pragma once
#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QElapsedTimer>
#include <QTimer>
#include "Answer.h"
#include <QJsonObject>
#include <QtMultimedia>

class QResizeEvent;

class QuizPage : public QWidget {
    Q_OBJECT
public:
    explicit QuizPage(QWidget *parent = nullptr);
    void loadQuiz(const QString &file);

signals:
    void goToEnd();
    void WrongAnsGive();
    void quizFinish();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QLabel *questionLabel;
    QLabel *progressLabel;
    QLabel *timerLabel;
    QLabel *wrongLabel;
    QProgressBar *progressBar;
    QList<Answer*> answers;
    QPushButton *nextBtn;
    QWidget *actionArea;
    QTimer *displayTimer;
    QElapsedTimer quizTimer;
    int wrongCount = 0;
    void refreshStats();

    QMediaPlayer *goodPlayer = nullptr;
    QAudioOutput *goodOut = nullptr;
    QMediaPlayer *badPlayer = nullptr;
    QAudioOutput *badOut = nullptr;

    void setupSounds();
    void playGood();
    void playBad();

    QList<QJsonObject> questions;
    int index = 0;

    void loadFromJson(const QString &jsonFilePath);
    void loadQuestion(int ind);
};
