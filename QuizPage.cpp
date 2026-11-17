#include "QuizPage.h"
#include <QtMath>

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

QuizPage::QuizPage(QWidget *parent) : QWidget(parent)
{
    setupSounds();
    setObjectName("quizPage");
    setStyleSheet(R"(
        QWidget#quizPage { background: #f8ffe5; }
        QLabel#eyebrow { color: #1b9aaa; font-size: 13px; font-weight: 800; letter-spacing: 1px; }
        QLabel#progressLabel, QLabel#timerLabel, QLabel#wrongLabel { color: #415d5e; font-weight: 700; }
        QProgressBar { background: #e2ebdc; border: none; border-radius: 6px; height: 12px; text-align: center; }
        QProgressBar::chunk { background: #06d6a0; border-radius: 6px; }
        QFrame#questionCard { background: #ffffff; border: 2px solid #d5e2d7; border-radius: 12px; }
        QLabel#questionLabel { color: #173b3b; font-weight: 800; }
        QPushButton#nextButton { color: #173b3b; background: #ffc43d; border: 1px solid #d9a51f; border-radius: 10px; padding: 14px 30px; font-weight: 800; min-height: 26px; }
        QPushButton#nextButton:hover { background: #ffcf60; }
        QPushButton#nextButton:pressed { background: #f3b92d; }
    )");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(34, 24, 34, 24);
    layout->setSpacing(12);

    auto *topRow = new QHBoxLayout;
    auto *eyebrow = new QLabel("ОХРАНА ТРУДА  /  ПРОВЕРКА ЗНАНИЙ", this);
    eyebrow->setObjectName("eyebrow");
    progressLabel = new QLabel("ВОПРОС 1 ИЗ 5", this);
    progressLabel->setObjectName("progressLabel");
    timerLabel = new QLabel("ВРЕМЯ: 00:00", this);
    timerLabel->setObjectName("timerLabel");
    wrongLabel = new QLabel("ОШИБКИ: 0", this);
    wrongLabel->setObjectName("wrongLabel");
    topRow->addWidget(eyebrow);
    topRow->addStretch();
    topRow->addWidget(progressLabel);
    topRow->addSpacing(18);
    topRow->addWidget(timerLabel);
    topRow->addSpacing(18);
    topRow->addWidget(wrongLabel);

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
    answerGrid->setContentsMargins(12, 8, 12, 8);
    answerGrid->setHorizontalSpacing(20);
    answerGrid->setVerticalSpacing(18);
    for (int i = 0; i < 5; ++i) {
        auto *answer = new Answer(this, i + 1);
        answer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        answers.append(answer);
    }
    answerGrid->addWidget(answers[0], 0, 0, 1, 2);
    answerGrid->addWidget(answers[1], 0, 2, 1, 2);
    answerGrid->addWidget(answers[2], 0, 4, 1, 2);
    answerGrid->addWidget(answers[3], 1, 1, 1, 2);
    answerGrid->addWidget(answers[4], 1, 3, 1, 2);
    for (int column = 0; column < 6; ++column) answerGrid->setColumnStretch(column, 1);
    answerGrid->setRowStretch(0, 1);
    answerGrid->setRowStretch(1, 1);

    actionArea = new QWidget(this);
    actionArea->setFixedHeight(68);
    auto *actionLayout = new QHBoxLayout(actionArea);
    actionLayout->setContentsMargins(0, 0, 0, 0);
    actionLayout->addStretch();
    nextBtn = new QPushButton("СЛЕДУЮЩИЙ ВОПРОС  →", actionArea);
    nextBtn->setObjectName("nextButton");
    nextBtn->setCursor(Qt::PointingHandCursor);
    nextBtn->setMinimumWidth(210);
    nextBtn->hide();
    nextBtn->setFocusPolicy(Qt::NoFocus);
    actionLayout->addWidget(nextBtn, 0, Qt::AlignRight | Qt::AlignVCenter);

    layout->addLayout(topRow);
    layout->addWidget(progressBar);
    layout->addWidget(questionCard);
    layout->addLayout(answerGrid, 1);
    layout->addWidget(actionArea);

    displayTimer = new QTimer(this);
    displayTimer->setInterval(1000);
    connect(displayTimer, &QTimer::timeout, this, &QuizPage::refreshStats);

    for (int i = 0; i < answers.size(); ++i) {
        connect(answers[i], &Answer::giveAns, this, [this](bool correct) {
            if (correct) {
                playGood();
                nextBtn->show();
            } else {
                playBad();
                ++wrongCount;
                refreshStats();
                emit WrongAnsGive();
            }
        });
    }
    connect(nextBtn, &QPushButton::clicked, this, [this]() {
        nextBtn->hide();
        ++index;
        if (index == questions.size() - 1) {
            nextBtn->setText("ЗАВЕРШИТЬ ПОПЫТКУ  →");
        }
        if (index >= questions.size()) {
            displayTimer->stop();
            nextBtn->setText("СЛЕДУЮЩИЙ ВОПРОС  →");
            emit quizFinish();
            emit goToEnd();
        } else loadQuestion(index);
    });
}

void QuizPage::refreshStats()
{
    const qint64 seconds = quizTimer.elapsed() / 1000;
    timerLabel->setText(QString("ВРЕМЯ: %1:%2")
        .arg(seconds / 60, 2, 10, QChar('0'))
        .arg(seconds % 60, 2, 10, QChar('0')));
    wrongLabel->setText(QString("ОШИБКИ: %1").arg(wrongCount));
}

void QuizPage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    const int margin = qBound(18, width() / 30, 40);
    if (layout()) layout()->setContentsMargins(margin, qBound(14, height() / 30, 26), margin, qBound(14, height() / 30, 26));
    const int questionSize = qBound(16, qRound(24.0 * qMin(width() / 960.0, height() / 760.0)), 34);
    questionLabel->setFont(QFont("Arial", questionSize, QFont::Bold));
    const int headerSize = qBound(12, width() / 82, 17);
    progressLabel->setFont(QFont("Arial", headerSize, QFont::DemiBold));
    timerLabel->setFont(QFont("Arial", headerSize, QFont::DemiBold));
    wrongLabel->setFont(QFont("Arial", headerSize, QFont::DemiBold));
    nextBtn->setFont(QFont("Arial", qBound(15, width() / 64, 21), QFont::Bold));
    nextBtn->setMinimumHeight(qBound(46, height() / 14, 60));
    nextBtn->setMinimumWidth(qBound(210, width() / 4, 320));
    actionArea->setFixedHeight(qBound(62, height() / 10, 82));
}

void QuizPage::loadFromJson(const QString &jsonFilePath)
{
    QFile file(jsonFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Не удалось открыть файл теста" << file.errorString();
        return;
    }
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    for (const auto &entry : doc.array()) questions.append(entry.toObject());
}

void QuizPage::loadQuestion(int ind)
{
    if (ind < 0 || ind >= questions.size()) {
        qWarning() << "Некорректный номер вопроса:" << ind << "Количество вопросов:" << questions.size();
        return;
    }
    const QJsonObject current = questions.at(ind);
    questionLabel->setText(current.value("question").toString());
    progressLabel->setText(QString("ВОПРОС %1 ИЗ %2").arg(ind + 1).arg(questions.size()));
    progressBar->setValue(qRound(100.0 * (ind + 1) / questions.size()));
    QList<QString> answersFront, answersBack;
    for (const auto &answer : current.value("answers_unrevealed").toArray()) answersFront.append(answer.toString());
    for (const auto &answer : current.value("answers_revealed").toArray()) answersBack.append(answer.toString());
    if (answersFront.size() != 5 || answersBack.size() != 5) {
        questionLabel->setText("Ошибка в данных вопроса: требуется ровно пять вариантов ответа.");
        return;
    }
    for (int i = 0; i < 5; ++i) answers[i]->setNewAnswer(answersFront[i], answersBack[i]);
    const int correct = current.value("correct").toInt() - 1;
    if (correct >= 0 && correct < answers.size()) answers[correct]->setCorrect(true);
}

void QuizPage::setupSounds()
{
    const QString base = QCoreApplication::applicationDirPath();
    goodOut = new QAudioOutput(this); goodOut->setVolume(1.0f);
    goodPlayer = new QMediaPlayer(this); goodPlayer->setAudioOutput(goodOut);
    goodPlayer->setSource(QUrl::fromLocalFile(base + "/assets/right_sound.wav"));
    badOut = new QAudioOutput(this); badOut->setVolume(1.0f);
    badPlayer = new QMediaPlayer(this); badPlayer->setAudioOutput(badOut);
    badPlayer->setSource(QUrl::fromLocalFile(base + "/assets/wrong_sound.wav"));
}
void QuizPage::playGood() { goodPlayer->setPosition(0); goodPlayer->play(); }
void QuizPage::playBad() { badPlayer->setPosition(0); badPlayer->play(); }

void QuizPage::loadQuiz(const QString &filename)
{
    displayTimer->stop();
    questions.clear();
    index = 0;
    wrongCount = 0;
    quizTimer.start();
    refreshStats();
    displayTimer->start();
    nextBtn->hide();
    questionLabel->clear();
    for (auto *answer : answers) answer->setNewAnswer(QString(), QString());
    const QString path = QCoreApplication::applicationDirPath() + "/assets/" + filename;
    qDebug() << "Загрузка теста:" << path;
    loadFromJson(path);
    if (questions.isEmpty()) {
        displayTimer->stop();
        questionLabel->setText(tr("Файл теста пуст или не найден: %1").arg(filename));
        progressLabel->setText("ВОПРОСОВ НЕТ");
        progressBar->setValue(0);
        return;
    }
    loadQuestion(0);
}
