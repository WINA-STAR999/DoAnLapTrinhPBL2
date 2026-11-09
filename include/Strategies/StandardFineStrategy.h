#ifndef STANDARD_FINE_STRATEGY_H
#define STANDARD_FINE_STRATEGY_H

#include "../Interfaces/IFineStrategy.h"
#include "../Utils/ConfigManager.h"

using namespace std;

// 1. Chien luoc tinh tien phat co dinh: soNgayTre * donGiaPhatNgay
class StandardFineStrategy : public IFineStrategy {
public:
    double calculateFine(int overdueDays) const override {
        if (overdueDays <= 0) {
            return 0.0;
        }
        double baseRate = ConfigManager::getInstance().getFinePerDay();
        return (double)overdueDays * baseRate;
    }
};

// 2. Chien luoc tinh tien phat luy tien bac thang:
// - 7 ngay dau tien: 1.0x don gia co ban
// - Tu ngay 8 den ngay 30: 1.5x don gia co ban
// - Tren 30 ngay: 2.0x don gia co ban
class ProgressiveFineStrategy : public IFineStrategy {
public:
    double calculateFine(int overdueDays) const override {
        if (overdueDays <= 0) {
            return 0.0;
        }

        double baseRate = ConfigManager::getInstance().getFinePerDay();
        double tongPhat = 0.0;

        if (overdueDays <= 7) {
            tongPhat = overdueDays * baseRate * 1.0;
        } else if (overdueDays <= 30) {
            tongPhat = 7 * baseRate * 1.0 + (overdueDays - 7) * baseRate * 1.5;
        } else {
            tongPhat = 7 * baseRate * 1.0 + 23 * baseRate * 1.5 + (overdueDays - 30) * baseRate * 2.0;
        }

        return tongPhat;
    }
};

#endif // STANDARD_FINE_STRATEGY_H
