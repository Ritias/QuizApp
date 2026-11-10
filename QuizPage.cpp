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
    answers = QList<Answer*>();
    for (int i = 0; i < 5; i++) {
        answers.append(new Answer(nullptr, i + 1));
        ansLayout->addWidget(answers.at(i));
    }

    questionLabel = new QLabel();

    nextBtn = new QPushButton("Дальше", this);
    nextBtn->hide();

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
        if (index >= questions.size()) {
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
    if (ind < 0 || ind >= questions.size()) {
        qWarning() << "loadQuestion: bad index" << ind
                   << "size =" << questions.size();
        return;
    }
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
void QuizPage::loadQuiz(const QString &filename) {
    questions.clear();
    index = 0;
    nextBtn->hide();
    questionLabel->setText(QString());
    for (auto *a : answers)
        a->setNewAnswer(QString(), QString());
    const QString path =
        QCoreApplication::applicationDirPath() + "/assets/" + filename;
    qDebug() << "Loading quiz file:" << path;
    loadFromJson(path);
    if (questions.isEmpty()) {
        questionLabel->setText(tr("File is empty: %1").arg(filename));
        return;
    }
    loadQuestion(0);
}
