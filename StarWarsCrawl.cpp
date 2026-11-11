#include "StarWarsCrawl.h"

#include <QAbstractTextDocumentLayout>
#include <QColor>
#include <QFont>
#include <QPainter>
#include <QPaintEvent>
#include <QPalette>
#include <QResizeEvent>
#include <QShowEvent>
#include <QSizePolicy>

#include <algorithm>
#include <cmath>

// ------------------------------------------------------------
// Constructor
// ------------------------------------------------------------

StarWarsCrawl::StarWarsCrawl(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(320, 180);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setAttribute(Qt::WA_OpaquePaintEvent);
    setAutoFillBackground(false);

    // Keep the text document free of unwanted outer margins.
    document_.setDocumentMargin(0);

    // Larger source font so the rendered crawl remains readable.
    document_.setDefaultFont(
        QFont("Arial", 56, QFont::Bold)
        );

    document_.setDefaultStyleSheet(R"(
        body {
            color: #ffc43d;
            background: transparent;
        }

        p {
            color: #ffc43d;
            margin-top: 0px;
            margin-bottom: 18px;
            padding: 0px;
            text-align: justify;
        }
    )");

    // Replace this example with your own crawl text.
    document_.setHtml(R"(
        <body>
            <p align="center">
                <b>EPISODE X</b>
            </p>

            <p align="center">
                <b>THE QUIZ STRIKES BACK</b>
            </p>

            <p>
                Давным-давно, на одном далёком производстве...
            </p>

            <p>
                Пять вопросов помогут проверить знания требований охраны труда.
                Внимательность, ответственность и понимание правил безопасности
                важны на каждом рабочем месте.
            </p>

            <p>
                Пусть полученные знания помогают замечать опасности заранее,
                выбирать безопасные действия и беречь себя и коллег.
            </p>
        </body>
    )");

    // Lay out the text at a fixed source width. Perspective scaling
    // will determine its actual size on the screen.
    document_.setTextWidth(TextWidth);

    renderTextImage();

    timer_.setInterval(16);

    connect(
        &timer_,
        &QTimer::timeout,
        this,
        [this]()
        {
            if (!started_ || textImage_.isNull())
                return;

            // Drive the animation using elapsed time rather than
            // accumulating a perspective-dependent phase increment.
            progress_ = qBound(
                0.0,
                elapsed_.elapsed() / qreal(durationMs_),
                1.0
                );

            update();

            if (progress_ >= 1.0 && !finishedEmitted_) {
                timer_.stop();

                finishedEmitted_ = true;
                emit finished();
            }
        }
        );
}


// ------------------------------------------------------------
// 16:9 landscape viewport
// ------------------------------------------------------------

QRectF StarWarsCrawl::crawlViewport() const
{
    const qreal availableW = width();
    const qreal availableH = height();

    if (availableW <= 0.0 || availableH <= 0.0)
        return QRectF();

    // Wide 16:9 landscape viewport.
    qreal viewportW = availableW;
    qreal viewportH = viewportW * 9.0 / 16.0;

    // Fit inside the widget while preserving the 16:9 ratio.
    if (viewportH > availableH) {
        viewportH = availableH;
        viewportW = viewportH * 16.0 / 9.0;
    }

    // Center the viewport.
    return QRectF(
        (availableW - viewportW) / 2.0,
        (availableH - viewportH) / 2.0,
        viewportW,
        viewportH
        );
}


// ------------------------------------------------------------
// Scale of the text at the bottom of the viewport
// ------------------------------------------------------------

qreal StarWarsCrawl::bottomScale() const
{
    if (textImage_.isNull())
        return 1.0;

    const QRectF viewport = crawlViewport();

    if (viewport.width() <= 0.0)
        return 1.0;

    // Make the text image span 95% of the viewport width
    // at the bottom of the crawl.
    return viewport.width() * 1.10 / textImage_.width();
}


// ------------------------------------------------------------
// Perspective constant
// ------------------------------------------------------------

qreal StarWarsCrawl::perspectiveConstant() const
{
    const QRectF viewport = crawlViewport();

    if (viewport.isEmpty())
        return 1.0;

    const qreal horizonY = viewport.height() * 0.20;
    const qreal distanceToBottom = viewport.height() - horizonY;
    const qreal scale = bottomScale();

    if (scale <= 0.0)
        return 1.0;

    // Keeps the perspective transform and bottom text scale
    // mathematically consistent.
    return distanceToBottom * distanceToBottom / scale;
}


// ------------------------------------------------------------
// Calculate the perspective phase from animation progress
// ------------------------------------------------------------

qreal StarWarsCrawl::phaseForProgress() const
{
    if (textImage_.isNull())
        return 0.0;

    const QRectF viewport = crawlViewport();

    if (viewport.isEmpty())
        return 0.0;

    const qreal horizonY = viewport.height() * 0.20;
    const qreal distanceToBottom =
        viewport.height() - horizonY;

    const qreal c = perspectiveConstant();

    // Start the text below the visible area.
    const qreal startMargin = 80.0;

    const qreal startPhase =
        c / (distanceToBottom + startMargin);

    // Let the text shrink into the horizon.
    // A larger value makes the crawl finish sooner.
    const qreal finishDistance = 120.0;

    const qreal finishPhase =
        textImage_.height() + c / finishDistance;

    // Linear progression prevents the scrolling phase
    // from accelerating near the end.
    const qreal p = qBound(0.0, progress_, 1.0);

    return startPhase
           + (finishPhase - startPhase) * p;
}


// ------------------------------------------------------------
// Render QTextDocument to a transparent image
// ------------------------------------------------------------

void StarWarsCrawl::renderTextImage()
{
    const QSizeF documentSize = document_.size();

    const int imageHeight = std::max(
        1,
        static_cast<int>(std::ceil(documentSize.height()))
        );

    textImage_ = QImage(
        TextWidth,
        imageHeight,
        QImage::Format_ARGB32_Premultiplied
        );

    textImage_.fill(Qt::transparent);

    QPainter painter(&textImage_);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    QAbstractTextDocumentLayout::PaintContext context;

    context.palette.setColor(
        QPalette::Text,
        QColor("#ffc43d")
        );

    document_.documentLayout()->draw(&painter, context);
}


// ------------------------------------------------------------
// Start the crawl
// ------------------------------------------------------------

void StarWarsCrawl::startCrawl()
{
    if (started_ || width() <= 0 || height() <= 0)
        return;

    if (textImage_.isNull())
        return;

    started_ = true;
    progress_ = 0.0;

    elapsed_.start();
    timer_.start();

    update();
}


// ------------------------------------------------------------
// Widget events
// ------------------------------------------------------------

void StarWarsCrawl::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    startCrawl();
}


void StarWarsCrawl::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    update();
}


// ------------------------------------------------------------
// Paint the perspective crawl
// ------------------------------------------------------------

void StarWarsCrawl::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    // Classic black background.
    painter.fillRect(rect(), Qt::black);

    if (!started_ || textImage_.isNull())
        return;

    const QRectF viewport = crawlViewport();

    if (viewport.isEmpty())
        return;

    // Work in viewport-local coordinates from this point onward.
    painter.save();

    painter.translate(viewport.topLeft());

    painter.setClipRect(
        QRectF(
            0.0,
            0.0,
            viewport.width(),
            viewport.height()
            )
        );

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    const qreal viewportW = viewport.width();
    const qreal viewportH = viewport.height();

    // Horizon is 20% down from the top of the viewport.
    const qreal horizonY = viewportH * 0.20;

    const qreal distanceToBottom = viewportH - horizonY;

    const qreal c = perspectiveConstant();
    const qreal phase = phaseForProgress();

    const qreal imageWidth = textImage_.width();
    const qreal imageHeight = textImage_.height();

    const qreal scaleAtBottom = bottomScale();

    if (c <= 0.0 ||
        distanceToBottom <= 0.0 ||
        scaleAtBottom <= 0.0) {
        painter.restore();
        return;
    }

    // Fade the text as it approaches the horizon.
    const qreal fadeDistance = viewportH * 0.80;

    // Render one horizontal strip per screen row. Each strip is
    // mapped back to the correct source rows in the text image.
    const int firstRow =
        static_cast<int>(std::floor(horizonY));

    const int lastRow =
        static_cast<int>(std::ceil(viewportH));

    for (int screenRow = firstRow;
         screenRow < lastRow;
         ++screenRow) {

        const qreal screenY0 = screenRow;
        const qreal screenY1 = screenRow + 1.0;

        const qreal d0 = screenY0 - horizonY;
        const qreal d1 = screenY1 - horizonY;

        if (d1 <= 0.0)
            continue;

        // Inverse perspective projection:
        // screen distance d = c / (phase - sourceY)
        const qreal sourceY0 =
            (d0 > 0.0) ? phase - c / d0 : -1.0;

        const qreal sourceY1 = phase - c / d1;

        if (sourceY1 <= 0.0)
            continue;

        if (sourceY0 >= imageHeight)
            break;

        const qreal sourceTop = std::max(
            0.0,
            sourceY0
            );

        const qreal sourceBottom = std::min(
            imageHeight,
            sourceY1
            );

        if (sourceBottom <= sourceTop)
            continue;

        // Project the clipped source interval back onto the screen.
        const qreal projectedY0 =
            horizonY + c / (phase - sourceTop);

        const qreal projectedY1 =
            horizonY + c / (phase - sourceBottom);

        const qreal destinationY0 = std::max(
            screenY0,
            projectedY0
            );

        const qreal destinationY1 = std::min(
            screenY1,
            projectedY1
            );

        if (destinationY1 <= destinationY0)
            continue;

        const qreal screenY =
            (destinationY0 + destinationY1) * 0.5;

        const qreal distanceFromHorizon =
            screenY - horizonY;

        if (distanceFromHorizon <= 0.0)
            continue;

        // Horizontal scaling shrinks towards the vanishing point.
        const qreal horizontalScale =
            scaleAtBottom *
            distanceFromHorizon /
            distanceToBottom;

        const qreal targetWidth =
            imageWidth * horizontalScale;

        if (targetWidth <= 0.0)
            continue;

        const qreal targetX =
            (viewportW - targetWidth) * 0.5;

        // Fade quadraticly towards the horizon.
        const qreal fade = qBound(
            0.0,
            distanceFromHorizon / fadeDistance,
            1.0
            );

        const qreal opacity = fade * fade;

        // Avoid drawing imperceptible strips at the horizon.
        if (opacity <= 0.001)
            continue;

        const QRectF sourceRect(
            0.0,
            sourceTop,
            imageWidth,
            sourceBottom - sourceTop
            );

        const QRectF targetRect(
            targetX,
            destinationY0,
            targetWidth,
            destinationY1 - destinationY0
            );

        painter.setOpacity(opacity);

        painter.drawImage(
            targetRect,
            textImage_,
            sourceRect
            );
    }

    painter.restore();
}
void StarWarsCrawl::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    started_ = false;
    finishedEmitted_ = false;
    progress_ = 0.0;
    timer_.stop();
    update();
}