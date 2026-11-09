#pragma once
#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include "Answer.h"
#include <QJsonObject>
#include <QtMultimedia>


class QuizPage : public QWidget {
    Q_OBJECT
    public:
    explicit QuizPage(QWidget *parent = nullptr);
    signals:
    void goToEnd();
private:
    QLabel * questionLabel;
    QList<Answer*> answers;
    QPushButton * nextBtn;

    QMediaPlayer *goodPlayer = nullptr;
    QAudioOutput *goodOut    = nullptr;
    QMediaPlayer *badPlayer  = nullptr;
    QAudioOutput *badOut     = nullptr;

    void setupSounds();

    void playGood();

    void playBad();

    QList<QJsonObject> questions;
    int index = 0;

    void loadFromJson(const QString &jsonFilePath);
    void loadQuestion(int ind);
};
