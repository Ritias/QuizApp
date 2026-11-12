#ifndef RESULTS_H
#define RESULTS_H
#include <QVector>
#include<QElapsedTimer>
#include<QDebug>
struct Result{
    int quizNumber = 0;
    int countWrong = 0;
    qint64 elapsed = 0;
    QString timeString() const {
        qint64 totalSec = elapsed / 1000;
        int m = totalSec / 60;
        int s = totalSec % 60;
        return QString("%1:%2").arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
    }
};
inline QDebug operator<<(QDebug dbg, const Result &r) {
    QDebugStateSaver s(dbg);
    dbg.nospace() << "Result(quiz=" << r.quizNumber
                  << ", wrong=" << r.countWrong
                  << ", ms=" << r.elapsed << ")";
    return dbg;
}
class Results
{
public:
    void start(int);
    void end();
    void addWrong();
    const QVector<Result>& getRows();
    int getCount(){return current.countWrong;};
    qint64 currentElapsed() const { return time.elapsed(); }
    void giveTime();
private:
    Result current;
    QVector<Result> rows;
    QElapsedTimer time;
};

#endif // RESULTS_H
