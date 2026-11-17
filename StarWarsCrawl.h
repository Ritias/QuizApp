#pragma once

#include <QWidget>
#include <QImage>
#include <QTextDocument>
#include <QTimer>
#include <QElapsedTimer>
#include <QRectF>
#include <QString>

class QPaintEvent;
class QResizeEvent;
class QShowEvent;
class QHideEvent;

class StarWarsCrawl : public QWidget
{
    Q_OBJECT

public:
    explicit StarWarsCrawl(QWidget *parent = nullptr);
    void setCrawlHtml(const QString &html);
    void restart();

signals:
    void finished();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private:
    QRectF crawlViewport() const;
    qreal bottomScale() const;
    qreal perspectiveConstant() const;
    qreal phaseForProgress() const;
    void renderTextImage();
    void startCrawl();

    QTextDocument document_;
    QImage textImage_;
    QTimer timer_;
    QElapsedTimer elapsed_;
    qreal progress_ = 0.0;
    bool started_ = false;
    bool finishedEmitted_ = false;
    int durationMs_ = 30000;
    static constexpr int TextWidth = 800;
};
