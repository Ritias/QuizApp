#include "QuizPage.h"

#include <QVBoxLayout>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QCoreApplication>
#include <QDir>

QuizPage::QuizPage(QWidget *parent) {
    setupSounds();
    auto * layout = new QVBoxLayout(this);
    auto * ansLayout = new QHBoxLayout;

    QString path = QCoreApplication::applicationDirPath() + "/assets/questions_data.json";
    if (!QFile::exists(path)) {
        qWarning() << "File not found at:" << path;
        return;
    }
    loadFromJson(path);

    answers = QList<Answer*>();
    for (int i = 0; i < 5; i++) {
        answers.append(new Answer(nullptr, i + 1));
        ansLayout->addWidget(answers.at(i));
    }

    questionLabel = new QLabel();

    nextBtn = new QPushButton("Дальше", this);
    nextBtn->hide();

    loadQuestion(0);
    layout->addWidget(questionLabel);
    layout->addLayout(ansLayout);
    layout->addWidget(nextBtn);

    for (int i = 0; i < 5; i++) {
        connect(answers[i], &Answer::giveAns, this, [this](bool c) {
            if (c) {
                playGood();
                nextBtn->show();
            } else {
                playBad();
            }
        });
    }

    connect(nextBtn, &QPushButton::clicked, this, [this]() {
        nextBtn->hide();
        index++;
        if (index >= 5) {
            emit goToEnd();
        } else {
            loadQuestion(index);
        }
    });
}

void QuizPage::loadFromJson(const QString &jsonFilePath) {
    QFile file(jsonFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open quiz file";
        qWarning() << "Reason: " << file.errorString();
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    QJsonArray jsonArray = doc.array();

    for (int i = 0; i < jsonArray.size(); ++i) {
        questions.append(jsonArray[i].toObject());
    }
}

void QuizPage::loadQuestion(int ind) {
    QJsonObject cur = questions.at(ind);
    questionLabel->setText(cur.value("question").toString());
    QList<QString> answers_front;
    QList<QString> answers_back;
    QJsonArray jsonArray = cur.value("answers_unrevealed").toArray();
    for (auto answer : jsonArray) {
        answers_front.append(answer.toString());
    }
    jsonArray = cur.value("answers_revealed").toArray();
    for (auto answer : jsonArray) {
        answers_back.append(answer.toString());
    }
    for (int i = 0; i < 5; i++) {
        answers[i]->setNewAnswer(answers_front[i], answers_back[i]);
    }
    answers[cur.value("correct").toInt() - 1]->setCorrect(true);
}

void QuizPage::setupSounds() {
    const QString base = QCoreApplication::applicationDirPath();

    goodOut = new QAudioOutput(this);
    goodOut->setVolume(1.0f);
    goodPlayer = new QMediaPlayer(this);
    goodPlayer->setAudioOutput(goodOut);
    goodPlayer->setSource(QUrl::fromLocalFile(base + "/assets/right_sound.wav"));

    badOut = new QAudioOutput(this);
    badOut->setVolume(1.0f);
    badPlayer = new QMediaPlayer(this);
    badPlayer->setAudioOutput(badOut);
    badPlayer->setSource(QUrl::fromLocalFile(base + "/assets/wrong_sound.wav"));
}

void QuizPage::playGood() {
    goodPlayer->setPosition(0);
    goodPlayer->play();
}

void QuizPage::playBad() {
    badPlayer->setPosition(0);
    badPlayer->play();
}
