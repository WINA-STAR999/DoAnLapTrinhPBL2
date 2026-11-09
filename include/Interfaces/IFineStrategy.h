#ifndef IFINE_STRATEGY_H
#define IFINE_STRATEGY_H

using namespace std;

// Giao dien chien luoc tinh tien phat ap dung Strategy Pattern
class IFineStrategy {
public:
    virtual ~IFineStrategy() = default;

    // Phuong thuc tinh so tien phat dua tren so ngay qua han (overdueDays)
    virtual double calculateFine(int overdueDays) const = 0;
};

#endif // IFINE_STRATEGY_H
