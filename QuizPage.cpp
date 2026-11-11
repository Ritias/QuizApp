#include "QuizPage.h"

#include <QCoreApplication>
#include <QFile>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QResizeEvent>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QFont>

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
            font-size: 13px;
            font-weight: 800;
            letter-spacing: 1px;
        }
        QLabel#progressLabel {
            color: #415d5e;
            font-size: 14px;
            font-weight: 700;
        }
        QProgressBar {
            background: #e2ebdc;
            border: none;
            border-radius: 6px;
            height: 12px;
            text-align: center;
        }
        QProgressBar::chunk {
            background: #06d6a0;
            border-radius: 6px;
        }
        QFrame#questionCard {
            background: #ffffff;
            border: 2px solid #d5e2d7;
            border-radius: 12px;
        }
        QLabel#questionLabel {
            color: #173b3b;
            font-weight: 800;
        }
        QPushButton#nextButton {
            color: #173b3b;
            background: #ffc43d;
            border: 1px solid #d9a51f;
            border-radius: 10px;
            padding: 14px 30px;
            font-size: 16px;
            font-weight: 800;
            min-height: 26px;
        }
        QPushButton#nextButton:hover {
            background: #ffcf60;
        }
        QPushButton#nextButton:pressed {
            background: #f3b92d;
        }
    )");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(34, 24, 34, 24);
    layout->setSpacing(12);

    auto *topRow = new QHBoxLayout;
    auto *eyebrow = new QLabel("ОХРАНА ТРУДА  /  ПРОВЕРКА ЗНАНИЙ", this);
    eyebrow->setObjectName("eyebrow");

    progressLabel = new QLabel("ВОПРОС 1 ИЗ 5", this);
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
    questionLayout->setContentsMargins(24, 16, 24, 16);

    questionLabel = new QLabel(questionCard);
    questionLabel->setObjectName("questionLabel");
    questionLabel->setWordWrap(true);
    questionLabel->setAlignment(Qt::AlignCenter);
    questionLayout->addWidget(questionLabel);

    auto *answerGrid = new QGridLayout;
    answerGrid->setContentsMargins(0, 0, 0, 0);
    answerGrid->setHorizontalSpacing(14);
    answerGrid->setVerticalSpacing(12);
    answers = QList<Answer*>();

    for (int i = 0; i < 5; ++i) {
        auto *answer = new Answer(this, i + 1);
        answer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        answers.append(answer);
    }

    // Six equal columns keep every card the same width. The two lower
    // cards occupy the middle four columns and are therefore centered.
    answerGrid->addWidget(answers[0], 0, 0, 1, 2);
    answerGrid->addWidget(answers[1], 0, 2, 1, 2);
    answerGrid->addWidget(answers[2], 0, 4, 1, 2);
    answerGrid->addWidget(answers[3], 1, 1, 1, 2);
    answerGrid->addWidget(answers[4], 1, 3, 1, 2);

    for (int column = 0; column < 6; ++column)
        answerGrid->setColumnStretch(column, 1);
    answerGrid->setRowStretch(0, 1);
    answerGrid->setRowStretch(1, 1);

    nextBtn = new QPushButton("СЛЕДУЮЩИЙ ВОПРОС  →", this);
    nextBtn->setObjectName("nextButton");
    nextBtn->setCursor(Qt::PointingHandCursor);
    nextBtn->hide();

    layout->addLayout(topRow);
    layout->addWidget(progressBar);
    layout->addWidget(questionCard);
    layout->addLayout(answerGrid, 1);
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

void QuizPage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    const int margin = qBound(18, width() / 30, 40);
    if (layout())
        layout()->setContentsMargins(margin, qBound(14, height() / 30, 26),
                                     margin, qBound(14, height() / 30, 26));

    const int questionSize = qBound(
        16,
        qRound(24.0 * qMin(width() / 960.0, height() / 760.0)),
        34);
    questionLabel->setFont(QFont("Arial", questionSize, QFont::Bold));

    const int headerSize = qBound(13, width() / 72, 18);
    progressLabel->setFont(QFont("Arial", headerSize, QFont::DemiBold));
    nextBtn->setFont(QFont("Arial", qBound(15, width() / 64, 21), QFont::Bold));
    nextBtn->setMinimumHeight(qBound(48, height() / 12, 68));
    progressLabel->setFont(QFont("Arial", headerSize, QFont::DemiBold));
}

void QuizPage::loadFromJson(const QString &jsonFilePath)
{
    QFile file(jsonFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Не удалось открыть файл теста";
        qWarning() << "Причина:" << file.errorString();
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
        qWarning() << "Некорректный номер вопроса:" << ind
                   << "Количество вопросов:" << questions.size();
        return;
    }

    const QJsonObject current = questions.at(ind);
    questionLabel->setText(current.value("question").toString());

    progressLabel->setText(
        QString("ВОПРОС %1 ИЗ %2").arg(ind + 1).arg(questions.size()));
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
    qDebug() << "Загрузка теста:" << path;

    loadFromJson(path);
    if (questions.isEmpty()) {
        questionLabel->setText(tr("Файл теста пуст или не найден: %1").arg(filename));
        progressLabel->setText("ВОПРОСОВ НЕТ");
        progressBar->setValue(0);
        return;
    }

    loadQuestion(0);
}
