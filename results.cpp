#include "results.h"
#include<QDebug>
void Results::start(int number){
    current.quizNumber = number;
    time.start();
    current.countWrong = 0;
}

void Results::end(){
    current.elapsed = time.elapsed();
    rows.push_back(current);
}

void Results::addWrong(){
    current.countWrong++;
}
const QVector<Result>& Results::getRows(){
    return rows;
}