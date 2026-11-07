#include "../../include/Utils/DateUtils.h"
#include "../../include/Exceptions/LibraryException.h"
#include <ctime>
#include <sstream>
#include <iomanip>

using namespace std;

// Kiem tra nam nhuan theo quy tac lich Gregorian
bool DateUtils::isLeapYear(int year) {
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        return true;
    }
    return false;
}

// Lay so ngay trong thang
int DateUtils::daysInMonth(int month, int year) {
    if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
        return 31;
    } else if (month == 4 || month == 6 || month == 9 || month == 11) {
        return 30;
    } else if (month == 2) {
        // Thang 2 co 29 ngay neu la nam nhuan, nguoc lai co 28 ngay
        if (isLeapYear(year)) {
            return 29;
        } else {
            return 28;
        }
    }
    return 0;
}

// Tach chuoi DD/MM/YYYY thanh cac so nguyen ngay, thang, nam
bool DateUtils::parseDate(const string& dateStr, int& day, int& month, int& year) {
    // Chuoi phai co do dai dung 10 ky tu (vi du: 01/10/2026)
    if (dateStr.length() != 10) {
        return false;
    }

    // Ky tu thu 2 va thu 5 phai la dau gach cheo '/'
    if (dateStr[2] != '/' || dateStr[5] != '/') {
        return false;
    }

    // Cac vi tri con lai phai la chu so tu '0' den '9'
    for (int i = 0; i < (int)dateStr.length(); i++) {
        if (i == 2 || i == 5) continue;
        if (dateStr[i] < '0' || dateStr[i] > '9') {
            return false;
        }
    }

    // Chuyen cac phan chuoi con thanh so nguyen
    try {
        day = stoi(dateStr.substr(0, 2));
        month = stoi(dateStr.substr(3, 2));
        year = stoi(dateStr.substr(6, 4));
    } catch (...) {
        return false;
    }

    // Kiem tra khoang gia tri hop le
    if (year < 1 || year > 9999) {
        return false;
    }

    if (month < 1 || month > 12) {
        return false;
    }

    int soNgayToiDa = daysInMonth(month, year);
    if (day < 1 || day > soNgayToiDa) {
        return false;
    }

    return true;
}

// Kiem tra tinh hop le cua chuoi ngay
bool DateUtils::isValidDate(const string& dateStr) {
    int ngay = 0, thang = 0, nam = 0;
    return parseDate(dateStr, ngay, thang, nam);
}

// Chuyen ngay/thang/nam ve tong so ngay ke tu nam 1
long long DateUtils::dateToDays(int day, int month, int year) {
    // Tinh so ngay cua tat ca cac nam truoc (cong them 1 ngay cho moi nam nhuan)
    long long tongNgay = (long long)(year - 1) * 365;
    tongNgay += (year - 1) / 4 - (year - 1) / 100 + (year - 1) / 400;

    // Cong so ngay cua cac thang truoc trong nam hien tai
    for (int m = 1; m < month; m++) {
        tongNgay += daysInMonth(m, year);
    }

    // Cong so ngay cua thang hien tai
    tongNgay += day;
    return tongNgay;
}

// Lay ngay hien tai cua he thong theo dinh dang DD/MM/YYYY
string DateUtils::getCurrentDate() {
    time_t hienTai = time(nullptr);
    tm thoiGian{};
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&thoiGian, &hienTai);
#else
    localtime_r(&hienTai, &thoiGian);
#endif
    ostringstream oss;
    oss << setfill('0') << setw(2) << thoiGian.tm_mday << "/"
        << setfill('0') << setw(2) << (thoiGian.tm_mon + 1) << "/"
        << (thoiGian.tm_year + 1900);
    return oss.str();
}

// Lay thoi diem hien tai theo dinh dang YYYY-MM-DD HH:MM:SS
string DateUtils::getCurrentTimestamp() {
    time_t hienTai = time(nullptr);
    tm thoiGian{};
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&thoiGian, &hienTai);
#else
    localtime_r(&hienTai, &thoiGian);
#endif
    char boDem[30];
    strftime(boDem, sizeof(boDem), "%Y-%m-%d %H:%M:%S", &thoiGian);
    return string(boDem);
}

// Cong them days ngay vao mot ngay cu the (dung vong lap de chuyen thang/nam khi vuot qua so ngay trong thang)
string DateUtils::addDays(const string& dateStr, int days) {
    int ngay = 0, thang = 0, nam = 0;
    if (!parseDate(dateStr, ngay, thang, nam)) {
        throw ValidationException("Dinh dang ngay khong hop le (yeu cau DD/MM/YYYY): " + dateStr, "DateUtils");
    }

    ngay += days;

    // Xu ly truong hop ngay vuot qua so ngay cua thang hien tai
    while (ngay > daysInMonth(thang, nam)) {
        ngay -= daysInMonth(thang, nam);
        thang++;
        if (thang > 12) {
            thang = 1;
            nam++;
        }
    }

    // Xu ly truong hop days la so am lam ngay nho hon 1
    while (ngay < 1) {
        thang--;
        if (thang < 1) {
            thang = 12;
            nam--;
        }
        ngay += daysInMonth(thang, nam);
    }

    // Dinh dang lai thanh chuoi DD/MM/YYYY du 2 chu so
    ostringstream oss;
    oss << setfill('0') << setw(2) << ngay << "/"
        << setfill('0') << setw(2) << thang << "/"
        << setfill('0') << setw(4) << nam;
    return oss.str();
}

// Tinh so ngay muon sach bi qua han
int DateUtils::diffDays(const string& dueDate, const string& actualReturnDate) {
    int d1 = 0, m1 = 0, y1 = 0;
    if (!parseDate(dueDate, d1, m1, y1)) {
        throw ValidationException("Ngay han tra khong hop le (yeu cau DD/MM/YYYY): " + dueDate, "DateUtils");
    }

    int d2 = 0, m2 = 0, y2 = 0;
    if (!parseDate(actualReturnDate, d2, m2, y2)) {
        throw ValidationException("Ngay tra thuc te khong hop le (yeu cau DD/MM/YYYY): " + actualReturnDate, "DateUtils");
    }

    // Chuyen ca hai moc thoi gian ve tong so ngay de tru cho nhau
    long long tongNgayHanTra = dateToDays(d1, m1, y1);
    long long tongNgayTraThucTe = dateToDays(d2, m2, y2);

    long long chenhLech = tongNgayTraThucTe - tongNgayHanTra;

    // Neu tra tre (ngay tra thuc te > han tra), tra ve so ngay qua han
    if (chenhLech > 0) {
        return (int)chenhLech;
    }
    // Neu tra dung han hoac tra som hon, tien phat la 0 dong (0 ngay tre)
    return 0;
}
