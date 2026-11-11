#include "QuizPage.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVBoxLayout>

QuizPage::QuizPage(QWidget *parent)
    : QWidget(parent)
{
    setupSounds();
    setObjectName("quizPage");
    setStyleSheet(R"(
        QWidget#quizPage {
            background: #f8ffe5;
        }
        QLabel#eyebrow {
            color: #1b9aaa;
            font-size: 12px;
            font-weight: 800;
            letter-spacing: 2px;
        }
        QLabel#progressLabel {
            color: #527071;
            font-size: 13px;
            font-weight: 700;
        }
        QProgressBar {
            background: #e3edcf;
            border: none;
            border-radius: 5px;
            height: 9px;
            text-align: center;
        }
        QProgressBar::chunk {
            background: #06d6a0;
            border-radius: 5px;
        }
        QFrame#questionCard {
            background: #ffffff;
            border: 2px solid #e3edcf;
            border-radius: 22px;
        }
        QLabel#questionLabel {
            color: #173b3b;
            font-size: 23px;
            font-weight: 800;
        }
        QPushButton#nextButton {
            color: #173b3b;
            background: #ffc43d;
            border: none;
            border-radius: 16px;
            padding: 13px 34px;
            font-size: 15px;
            font-weight: 800;
        }
        QPushButton#nextButton:hover {
            background: #ffd36b;
        }
        QPushButton#nextButton:pressed {
            padding-top: 15px;
            padding-bottom: 11px;
        }
    )");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(38, 28, 38, 30);
    layout->setSpacing(14);

    auto *topRow = new QHBoxLayout;
    auto *eyebrow = new QLabel("QUIZ GALAXY  /  CHALLENGE", this);
    eyebrow->setObjectName("eyebrow");

    progressLabel = new QLabel("ВОПРОС 1", this);
    progressLabel->setObjectName("progressLabel");
    topRow->addWidget(eyebrow);
    topRow->addStretch();
    topRow->addWidget(progressLabel);

    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setTextVisible(false);

    auto *questionCard = new QFrame(this);
    questionCard->setObjectName("questionCard");
    auto *questionLayout = new QVBoxLayout(questionCard);
    questionLayout->setContentsMargins(24, 18, 24, 18);

    questionLabel = new QLabel(questionCard);
    questionLabel->setObjectName("questionLabel");
    questionLabel->setWordWrap(true);
    questionLabel->setAlignment(Qt::AlignCenter);
    questionLayout->addWidget(questionLabel);

    auto *ansLayout = new QGridLayout;
    ansLayout->setHorizontalSpacing(18);
    ansLayout->setVerticalSpacing(12);
    answers = QList<Answer*>();

    for (int i = 0; i < 5; ++i) {
        auto *answer = new Answer(this, i + 1);
        answers.append(answer);

        const int row = i / 3;
        const int column = i % 3;
        ansLayout->addWidget(answer, row, column, Qt::AlignCenter);
    }

    for (int column = 0; column < 3; ++column)
        ansLayout->setColumnStretch(column, 1);

    nextBtn = new QPushButton("ДАЛЬШЕ  →", this);
    nextBtn->setObjectName("nextButton");
    nextBtn->setCursor(Qt::PointingHandCursor);
    nextBtn->hide();

    layout->addLayout(topRow);
    layout->addWidget(progressBar);
    layout->addWidget(questionCard);
    layout->addLayout(ansLayout, 1);
    layout->addWidget(nextBtn, 0, Qt::AlignRight);

    for (int i = 0; i < 5; i++) {
        connect(answers[i], &Answer::giveAns, this, [this](bool correct) {
            if (correct) {
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

void QuizPage::loadFromJson(const QString &jsonFilePath)
{
    QFile file(jsonFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open quiz file";
        qWarning() << "Reason: " << file.errorString();
        return;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    const QJsonArray jsonArray = doc.array();

    for (const auto &entry : jsonArray)
        questions.append(entry.toObject());
}

void QuizPage::loadQuestion(int ind)
{
    if (ind < 0 || ind >= questions.size()) {
        qWarning() << "loadQuestion: bad index" << ind
                   << "size =" << questions.size();
        return;
    }

    const QJsonObject current = questions.at(ind);
    questionLabel->setText(current.value("question").toString());

    progressLabel->setText(
        QString("ВОПРОС %1 / %2").arg(ind + 1).arg(questions.size()));
    progressBar->setValue(
        qRound(100.0 * (ind + 1) / questions.size()));

    QList<QString> answersFront;
    QList<QString> answersBack;

    QJsonArray jsonArray = current.value("answers_unrevealed").toArray();
    for (const auto &answer : jsonArray)
        answersFront.append(answer.toString());

    jsonArray = current.value("answers_revealed").toArray();
    for (const auto &answer : jsonArray)
        answersBack.append(answer.toString());

    for (int i = 0; i < 5; i++)
        answers[i]->setNewAnswer(answersFront[i], answersBack[i]);

    answers[current.value("correct").toInt() - 1]->setCorrect(true);
}

void QuizPage::setupSounds()
{
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

void QuizPage::playGood()
{
    goodPlayer->setPosition(0);
    goodPlayer->play();
}

void QuizPage::playBad()
{
    badPlayer->setPosition(0);
    badPlayer->play();
}

void QuizPage::loadQuiz(const QString &filename)
{
    questions.clear();
    index = 0;
    nextBtn->hide();
    questionLabel->setText(QString());

    for (auto *answer : answers)
        answer->setNewAnswer(QString(), QString());

    const QString path =
        QCoreApplication::applicationDirPath() + "/assets/" + filename;
    qDebug() << "Loading quiz file:" << path;

    loadFromJson(path);
    if (questions.isEmpty()) {
        questionLabel->setText(tr("Файл пуст или не найден: %1").arg(filename));
        progressLabel->setText("НЕТ ВОПРОСОВ");
        progressBar->setValue(0);
        return;
    }

    loadQuestion(0);
}
