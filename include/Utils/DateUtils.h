#ifndef DATE_UTILS_H
#define DATE_UTILS_H

#include <string>

using namespace std;

// Lop tien ich xu ly va tinh toan ngay thang theo dinh dang DD/MM/YYYY
class DateUtils {
public:
    // Kiem tra nam nhuan
    static bool isLeapYear(int year);

    // Lay so ngay toi da trong mot thang cua mot nam
    static int daysInMonth(int month, int year);

    // Kiem tra chuoi co dung dinh dang DD/MM/YYYY va hop le hay khong
    static bool isValidDate(const string& dateStr);

    // Tach chuoi DD/MM/YYYY thanh 3 so nguyen: ngay, thang, nam
    static bool parseDate(const string& dateStr, int& day, int& month, int& year);

    // Chuyen doi ngay/thang/nam thanh tong so ngay ke tu moc nam 1 (phuc vu tinh khoang cach ngay)
    static long long dateToDays(int day, int month, int year);

    // Lay ngay hien tai cua he thong theo dinh dang DD/MM/YYYY
    static string getCurrentDate();

    // Lay thoi gian hien tai theo dinh dang YYYY-MM-DD HH:MM:SS
    static string getCurrentTimestamp();

    // Cong them mot so ngay vao ngay cho truoc de tinh han tra sach
    static string addDays(const string& dateStr, int days);

    // Tinh so ngay qua han giua han tra va ngay tra thuc te (tra ve > 0 neu tre, nguoc lai tra ve 0)
    static int diffDays(const string& dueDate, const string& actualReturnDate);
};

#endif // DATE_UTILS_H
