#include "Answer.h"
#include <QtMath>

#include <QEvent>
#include <QResizeEvent>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QVBoxLayout>

void Answer::fitLabelText()
{
    if (!btnLabel || !answerBtn)
        return;

    const int padding = qMax(12, answerBtn->width() / 14);
    const int maxW = qMax(1, answerBtn->width() - padding * 2);
    const int maxH = qMax(1, answerBtn->height() - padding * 2);

    const int initialPt = qBound(
        14, qRound(qMin(maxW, maxH) * 0.14), 36);

    QFont font("Arial", initialPt, QFont::DemiBold);
    for (int pt = initialPt; pt >= 10; --pt) {
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

    font.setPointSize(10);
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
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 10, 12, 10);
    layout->setSpacing(6);

    answerBtn = new QToolButton(this);
    answerBtn->setObjectName("answerButton");
    answerBtn->setMinimumSize(140, 108);
    answerBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    answerBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
    answerBtn->setCursor(Qt::PointingHandCursor);
    answerBtn->installEventFilter(this);
    applyDefaultStyle();

    btnLabel = new QLabel(frontText, answerBtn);
    btnLabel->setWordWrap(true);
    btnLabel->setAlignment(Qt::AlignCenter);
    btnLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    btnLabel->setStyleSheet(
        "color: #173b3b; background: transparent; font-weight: 700;");

    questionNumber = new QLabel(QString("ВАРИАНТ %1").arg(num), this);
    questionNumber->setWordWrap(true);
    questionNumber->setAlignment(Qt::AlignCenter);
    questionNumber->setStyleSheet(
        "color: #415d5e; font-size: 12px; font-weight: 800;");

    connect(answerBtn, &QToolButton::clicked, this, [this] {
        if (!answered) {
            answered = true;
            answerBtn->setEnabled(false);
            flip();
            emit giveAns(correct);
        }
    });

    layout->addWidget(answerBtn, 1, Qt::AlignCenter);
    layout->addWidget(questionNumber);
}

void Answer::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // Keep a modest border of breathing room inside each grid cell.
    // The cap prevents tiles from becoming oversized on wide displays.
    const int buttonWidth = qBound(140, width() - 40, 440);
    const int buttonHeight = qBound(108, height() - 48, 250);
    answerBtn->setMaximumSize(buttonWidth, buttonHeight);

    updateAnswerLabelGeometry();
    fitLabelText();
}

bool Answer::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == answerBtn && event->type() == QEvent::Resize) {
        updateAnswerLabelGeometry();
        fitLabelText();
    }
    return QWidget::eventFilter(watched, event);
}

void Answer::updateAnswerLabelGeometry()
{
    if (!btnLabel || !answerBtn)
        return;

    const int padding = qMax(12, answerBtn->width() / 14);
    btnLabel->setGeometry(
        answerBtn->rect().adjusted(padding, padding, -padding, -padding));
}

void Answer::applyDefaultStyle()
{
    answerBtn->setStyleSheet(R"(
        QToolButton#answerButton {
            color: #173b3b;
            background: #ffffff;
            border: 2px solid #cbdad0;
            border-radius: 12px;
            padding: 10px;
            font-size: 18px;
            font-weight: 700;
        }
        QToolButton#answerButton:hover {
            background: #effcf5;
            border: 2px solid #1b9aaa;
        }
        QToolButton#answerButton:disabled {
            color: #173b3b;
        }
    )");
}

void Answer::flip()
{
    btnLabel->setText(flipText);
    fitLabelText();

    const QString background = correct ? "#06d6a0" : "#ef476f";
    answerBtn->setStyleSheet(QString(R"(
        QToolButton#answerButton {
            color: #173b3b;
            background: %1;
            border: 2px solid %1;
            border-radius: 12px;
            padding: 10px;
            font-size: 18px;
            font-weight: 800;
        }
    )").arg(background));
    btnLabel->setStyleSheet(
        "color: #173b3b; background: transparent; font-weight: 800;");
}

void Answer::setNewAnswer(const QString &front, const QString &back)
{
    frontText = front;
    flipText = back;
    answered = false;
    correct = false;

    btnLabel->setText(frontText);
    answerBtn->setEnabled(true);
    applyDefaultStyle();
    updateAnswerLabelGeometry();
    fitLabelText();
    btnLabel->setStyleSheet(
        "color: #173b3b; background: transparent; font-weight: 700;");
}

void Answer::setCorrect(bool isCorrect)
{
    correct = isCorrect;
}
