#ifndef BOOK_SORT_STRATEGIES_H
#define BOOK_SORT_STRATEGIES_H

#include "../Interfaces/ISortStrategy.h"
#include "../Models/Sach.h"
#include <string>

using namespace std;

// 1. Chien luoc sap xep sach theo Tua de (A-Z tang dan)
class SortByTitleStrategy : public ISortStrategy<Sach> {
public:
    bool shouldComeBefore(const Sach& a, const Sach& b) const override {
        return a.getTenSach() < b.getTenSach();
    }
};

// 2. Chien luoc sap xep sach theo Tac gia (A-Z tang dan)
class SortByAuthorStrategy : public ISortStrategy<Sach> {
public:
    bool shouldComeBefore(const Sach& a, const Sach& b) const override {
        return a.getTacGia() < b.getTacGia();
    }
};

// 3. Chien luoc sap xep sach theo Nam xuat ban (mac dinh giam dan - sach moi nhat len dau)
class SortByYearStrategy : public ISortStrategy<Sach> {
private:
    bool ascending; // true neu muon xep tang dan, false neu giam dan

public:
    SortByYearStrategy(bool asc = false) : ascending(asc) {}

    bool shouldComeBefore(const Sach& a, const Sach& b) const override {
        if (ascending) {
            return a.getNamXB() < b.getNamXB();
        } else {
            return a.getNamXB() > b.getNamXB();
        }
    }
};

// 4. Chien luoc sap xep sach theo So luong hien co (giam dan - uu tien sach con nhieu tren gia)
class SortByQuantityStrategy : public ISortStrategy<Sach> {
public:
    bool shouldComeBefore(const Sach& a, const Sach& b) const override {
        return a.getSoLuongHienCo() > b.getSoLuongHienCo();
    }
};

// Factory Method de khoi tao con tro chien luoc sap xep dua theo lua chon menu
class SortStrategyFactory {
public:
    static ISortStrategy<Sach>* createStrategy(int choice) {
        switch (choice) {
            case 1:
                return new SortByTitleStrategy();
            case 2:
                return new SortByAuthorStrategy();
            case 3:
                return new SortByYearStrategy(false); // Giam dan theo nam
            case 4:
                return new SortByQuantityStrategy();
            default:
                return new SortByTitleStrategy();
        }
    }
};

#endif // BOOK_SORT_STRATEGIES_H
