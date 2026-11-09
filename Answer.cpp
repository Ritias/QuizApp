#include "Answer.h"
#include <QVBoxLayout>
#include <QFontMetrics>

void Answer::fitLabelText() {
    if (!btnLabel) return;

    const int maxW = btnLabel->width();
    const int maxH = btnLabel->height();
    if (maxW <= 0 || maxH <= 0) return;

    QFont f = btnLabel->font();
    const int minPt = 6;

    for (int pt = f.pointSize(); pt >= minPt; --pt) {
        f.setPointSize(pt);
        QFontMetrics fm(f);
        const QRect r = fm.boundingRect(
            QRect(0, 0, maxW, maxH),
            Qt::TextWordWrap | Qt::AlignCenter,
            btnLabel->text());
        if (r.height() <= maxH) {
            btnLabel->setFont(f);
            return;
        }
    }
    f.setPointSize(minPt);
    btnLabel->setFont(f);
}

Answer::Answer(QWidget *parent, int num, const QString &front, const QString &back)
    : QWidget(parent), correct(false), answered(false), frontText(front), flipText(back)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    setObjectName("Answer");

    auto *layout = new QVBoxLayout(this);

    answerBtn = new QToolButton(this);
    answerBtn->setFixedSize(160, 160);
    answerBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    answerBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);

    btnLabel = new QLabel(frontText, answerBtn);
    btnLabel->setWordWrap(true);
    btnLabel->setAlignment(Qt::AlignCenter);
    btnLabel->setAttribute(Qt::WA_TransparentForMouseEvents);   // clicks pass through to the button
    btnLabel->setGeometry(answerBtn->rect().adjusted(6, 6, -6, -6));
    fitLabelText();

    questionNumber = new QLabel(QString::number(num), this);   // <-- assign member
    questionNumber->setWordWrap(true);
    questionNumber->setAlignment(Qt::AlignCenter);

    connect(answerBtn, &QToolButton::clicked, this, [this] {
        if (!answered) {
            answered = true;
            answerBtn->setEnabled(false);
            flip();
            emit giveAns(correct);
        }
    });

    layout->addWidget(answerBtn);
    layout->addWidget(questionNumber);
    layout->addStretch();
}

void Answer::flip() {
    btnLabel->setText(flipText);
    fitLabelText();

    if (correct) {
        // deep green
        answerBtn->setStyleSheet(
            "QToolButton { background-color: #1b5e20; border: none; border-radius: 6px; }");
        btnLabel->setStyleSheet("color: white; background: transparent;");
    } else {
        // faded red (pale, desaturated)
        answerBtn->setStyleSheet(
            "QToolButton { background-color: #ef9a9a; border: none; border-radius: 6px; }");
        btnLabel->setStyleSheet("color: #4a1010; background: transparent;");
    }
}

void Answer::setNewAnswer(const QString &front, const QString &back) {
    frontText = front;
    flipText  = back;
    answered  = false;
    correct   = false;

    btnLabel->setText(front);        // <-- was commented out
    fitLabelText();
    answerBtn->setEnabled(true);
    answerBtn->setStyleSheet({});     // reset any highlight
    btnLabel->setStyleSheet({});
}

void Answer::setCorrect(bool isCorrect) {   // <-- rename param
    correct = isCorrect;                    // <-- set the member, not `answered`
}
