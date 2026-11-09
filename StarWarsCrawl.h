#pragma once

#include <QWidget>
#include <QImage>
#include <QTextDocument>
#include <QTimer>
#include <QElapsedTimer>
#include <QRectF>

class QPaintEvent;
class QResizeEvent;
class QShowEvent;

class StarWarsCrawl : public QWidget
{
    Q_OBJECT

public:
    explicit StarWarsCrawl(QWidget *parent = nullptr);

signals:
    void finished();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    // Geometry and perspective
    QRectF crawlViewport() const;
    qreal bottomScale() const;
    qreal perspectiveConstant() const;
    qreal phaseForProgress() const;

    // Rendering and animation
    void renderTextImage();
    void startCrawl();

    QTextDocument document_;
    QImage textImage_;

    QTimer timer_;
    QElapsedTimer elapsed_;

    qreal progress_ = 0.0;

    bool started_ = false;
    bool finishedEmitted_ = false;

    // Animation duration
    int durationMs_ = 30000; // 30 seconds

    // Width of the text document before perspective transformation
    static constexpr int TextWidth = 800;
};
