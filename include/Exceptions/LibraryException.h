#ifndef LIBRARY_EXCEPTION_H
#define LIBRARY_EXCEPTION_H

#include <iostream>
#include <exception>
#include <string>
#include <ctime>

using namespace std;

// Lop cha LibraryException quan ly cac loi trong he thong thu vien
class LibraryException : public exception {
protected:
    int errorCode;              // Ma loi (vi du: 100, 200, 300, 400)
    string errorMessage;        // Noi dung chi tiet cua thong bao loi
    string moduleName;          // Ten module phat sinh loi (BLL, Repository, Validator...)
    string timestamp;           // Thoi diem xay ra loi (dinh dang YYYY-MM-DD HH:MM:SS)
    string formattedMessage;    // Chuoi thong bao day du de tra ve trong ham what()

public:
    // Ham khoi tao nhan ma loi, thong bao loi va ten module
    LibraryException(int code, const string& message, const string& module = "General")
        : errorCode(code), errorMessage(message), moduleName(module) {
        
        // Lay thoi gian he thong hien tai
        time_t hienTai = time(nullptr);
        tm thoiGian{};
#if defined(_WIN32) || defined(_WIN64)
        localtime_s(&thoiGian, &hienTai);
#else
        localtime_r(&hienTai, &thoiGian);
#endif
        char boDem[30];
        strftime(boDem, sizeof(boDem), "%Y-%m-%d %H:%M:%S", &thoiGian);
        timestamp = string(boDem);

        // Dinh dang chuoi thong bao loi theo chuan he thong
        formattedMessage = "[" + timestamp + "] [ERROR " + to_string(errorCode) + "] [" + moduleName + "] " + errorMessage;
    }

    virtual ~LibraryException() noexcept override = default;

    // Ghi de phuong thuc what() cua std::exception de in thong bao loi
    const char* what() const noexcept override {
        return formattedMessage.c_str();
    }

    // Cac ham getter lay thong tin chi tiet ve loi
    int getErrorCode() const noexcept {
        return errorCode;
    }

    const string& getErrorMessage() const noexcept {
        return errorMessage;
    }

    const string& getModuleName() const noexcept {
        return moduleName;
    }

    const string& getTimestamp() const noexcept {
        return timestamp;
    }
};

// 1. ValidationException (Ma 100): Bat loi khi du lieu dau vao sai dinh dang (ma sach, so dien thoai, nam xuat ban...)
class ValidationException : public LibraryException {
public:
    explicit ValidationException(const string& message, const string& module = "Validator")
        : LibraryException(100, message, module) {}
};

// 2. EntityNotFoundException (Ma 200): Bat loi khi tim kiem trong bang bam hoac giao dich tra ve nullptr
class EntityNotFoundException : public LibraryException {
public:
    explicit EntityNotFoundException(const string& message, const string& module = "Repository")
        : LibraryException(200, message, module) {}
};

// 3. BusinessRuleException (Ma 300): Bat loi vi pham quy tac nghiep vu (trung khoa, xoa sach dang co nguoi muon...)
class BusinessRuleException : public LibraryException {
public:
    explicit BusinessRuleException(const string& message, const string& module = "BLL")
        : LibraryException(300, message, module) {}
};

// 4. FileIOException (Ma 400): Bat loi khi khong the doc hoac ghi file (.txt, .ini, .log)
class FileIOException : public LibraryException {
public:
    explicit FileIOException(const string& message, const string& module = "FileIO")
        : LibraryException(400, message, module) {}
};

#endif // LIBRARY_EXCEPTION_H
