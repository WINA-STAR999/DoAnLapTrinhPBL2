#ifndef ISORT_STRATEGY_H
#define ISORT_STRATEGY_H

using namespace std;

// Giao dien chien luoc sap xep ap dung Strategy Pattern
template <typename T>
class ISortStrategy {
public:
    virtual ~ISortStrategy() = default;

    // Phuong thuc so sanh tra ve true neu phan tu a nen dung truoc phan tu b trong danh sach
    virtual bool shouldComeBefore(const T& a, const T& b) const = 0;
};

#endif // ISORT_STRATEGY_H
