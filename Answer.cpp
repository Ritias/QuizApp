#include "Answer.h"

#include <QFontMetrics>
#include <QVBoxLayout>

void Answer::fitLabelText()
{
    if (!btnLabel)
        return;

    const int maxW = btnLabel->width();
    const int maxH = btnLabel->height();
    if (maxW <= 0 || maxH <= 0)
        return;

    QFont font = btnLabel->font();
    const int minPt = 7;
    const int initialPt = font.pointSize() > 0 ? font.pointSize() : 18;

    for (int pt = initialPt; pt >= minPt; --pt) {
        font.setPointSize(pt);
        const QFontMetrics metrics(font);
        const QRect bounds = metrics.boundingRect(
            QRect(0, 0, maxW, maxH),
            Qt::TextWordWrap | Qt::AlignCenter,
            btnLabel->text());

        if (bounds.height() <= maxH) {
            btnLabel->setFont(font);
            return;
        }
    }

    font.setPointSize(minPt);
    btnLabel->setFont(font);
}

Answer::Answer(QWidget *parent, int num, const QString &front, const QString &back)
    : QWidget(parent),
      correct(false),
      answered(false),
      frontText(front),
      flipText(back)
{
    setObjectName("answerTile");
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    answerBtn = new QToolButton(this);
    answerBtn->setObjectName("answerButton");
    answerBtn->setFixedSize(160, 160);
    answerBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
    answerBtn->setCursor(Qt::PointingHandCursor);
    answerBtn->setStyleSheet(R"(
        QToolButton#answerButton {
            color: #173b3b;
            background: #ffffff;
            border: 2px solid #dce8c9;
            border-radius: 24px;
            padding: 10px;
            font-size: 18px;
            font-weight: 800;
        }
        QToolButton#answerButton:hover {
            background: #e8fff4;
            border: 3px solid #06d6a0;
        }
        QToolButton#answerButton:disabled {
            opacity: 1;
        }
    )");

    btnLabel = new QLabel(frontText, answerBtn);
    btnLabel->setWordWrap(true);
    btnLabel->setAlignment(Qt::AlignCenter);
    btnLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    btnLabel->setGeometry(answerBtn->rect().adjusted(12, 12, -12, -12));
    btnLabel->setStyleSheet("color: #173b3b; background: transparent; font-weight: 700;");
    fitLabelText();

    questionNumber = new QLabel(QString("OPTION %1").arg(num), this);
    questionNumber->setWordWrap(true);
    questionNumber->setAlignment(Qt::AlignCenter);
    questionNumber->setStyleSheet(
        "color: #527071; font-size: 10px; font-weight: 800; letter-spacing: 1px;");

    connect(answerBtn, &QToolButton::clicked, this, [this] {
        if (!answered) {
            answered = true;
            answerBtn->setEnabled(false);
            flip();
            emit giveAns(correct);
        }
    });

    layout->addWidget(answerBtn, 0, Qt::AlignHCenter);
    layout->addWidget(questionNumber);
}

void Answer::flip()
{
    btnLabel->setText(flipText);
    fitLabelText();

    const QString background = correct ? "#06d6a0" : "#ef476f";
    const QString foreground = "#173b3b";

    answerBtn->setStyleSheet(QString(R"(
        QToolButton#answerButton {
            color: %1;
            background: %2;
            border: 3px solid %2;
            border-radius: 24px;
            padding: 10px;
            font-size: 18px;
            font-weight: 800;
        }
    )").arg(foreground, background));
    btnLabel->setStyleSheet(
        "color: #173b3b; background: transparent; font-weight: 800;");
}

void Answer::setNewAnswer(const QString &front, const QString &back)
{
    frontText = front;
    flipText = back;
    answered = false;
    correct = false;

    btnLabel->setText(front);
    fitLabelText();
    answerBtn->setEnabled(true);
    answerBtn->setStyleSheet(R"(
        QToolButton#answerButton {
            color: #173b3b;
            background: #ffffff;
            border: 2px solid #dce8c9;
            border-radius: 24px;
            padding: 10px;
            font-size: 18px;
            font-weight: 800;
        }
        QToolButton#answerButton:hover {
            background: #e8fff4;
            border: 3px solid #06d6a0;
        }
    )");
    btnLabel->setStyleSheet(
        "color: #173b3b; background: transparent; font-weight: 700;");
}

void Answer::setCorrect(bool isCorrect)
{
    correct = isCorrect;
}
