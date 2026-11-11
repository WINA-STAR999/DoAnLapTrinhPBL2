#include "../../include/UI/AppInterface.h"
#include "../../include/Utils/FileLogger.h"
#include "../../include/Utils/ConfigManager.h"
#include "../../include/Utils/DateUtils.h"
#include "../../include/Factories/UserFactory.h"
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

// Ham khoi tao nhan cac phu thuoc qua Dependency Injection
AppInterface::AppInterface(BookService* bService, ReaderService* rService, BorrowService* brService,
                           BookRepository* bRepo, ReaderRepository* rRepo, TransactionRepository* tRepo)
    : bookService(bService), readerService(rService), borrowService(brService),
      bookRepo(bRepo), readerRepo(rRepo), transRepo(tRepo), currentUser(nullptr) {}

// In tieu de hop vien dep mat
void AppInterface::printBanner(const string& title) const {
    cout << "\n========================================================================================\n";
    cout << "   " << title << "\n";
    cout << "========================================================================================\n";
}

// In tieu de bang danh sach sach
void AppInterface::printTableBookHeader() const {
    cout << string(106, '-') << "\n";
    cout << "| " << left << setw(8) << "Ma Sach"
         << "| " << left << setw(32) << "Ten Sach"
         << "| " << left << setw(20) << "Tac Gia"
         << "| " << left << setw(14) << "The Loai"
         << "| " << right << setw(5) << "Nam"
         << " | " << right << setw(4) << "Tong"
         << " | " << right << setw(4) << "Con"
         << " | " << left << setw(7) << "Vi Tri" << " |\n";
    cout << string(106, '-') << "\n";
}

// In mot dong thong tin sach
void AppInterface::printBookRow(const Sach& s) const {
    string ten = s.getTenSach();
    if (ten.length() > 30) ten = ten.substr(0, 27) + "...";

    string tg = s.getTacGia();
    if (tg.length() > 18) tg = tg.substr(0, 15) + "...";

    string tl = s.getTheLoai();
    if (tl.length() > 13) tl = tl.substr(0, 11) + "...";

    cout << "| " << left << setw(8) << s.getMaSach()
         << "| " << left << setw(32) << ten
         << "| " << left << setw(20) << tg
         << "| " << left << setw(14) << tl
         << "| " << right << setw(5) << s.getNamXB()
         << " | " << right << setw(4) << s.getTongSoLuong()
         << " | " << right << setw(4) << s.getSoLuongHienCo()
         << " | " << left << setw(7) << s.getViTri() << " |\n";
}

// Dung man hinh cho nguoi dung nhan Enter
void AppInterface::pauseScreen() const {
    cout << "\n>> Nhan Enter de tiep tuc...";
    string dummy;
    getline(cin, dummy);
}

// Doc chuoi nhap vao tu ban phim an toan
string AppInterface::readInput(const string& prompt) const {
    cout << prompt;
    string input;
    getline(cin, input);
    if (!input.empty() && input.back() == '\r') {
        input.pop_back();
    }
    return input;
}

// Doc lua chon so nguyen trong khoang [minVal, maxVal]
int AppInterface::readMenuChoice(int minVal, int maxVal) const {
    while (true) {
        cout << ">> Nhap lua chon cua ban [" << minVal << " - " << maxVal << "]: ";
        string line = readInput("");
        try {
            int val = stoi(line);
            if (val >= minVal && val <= maxVal) {
                return val;
            }
        } catch (...) {}
        cout << "[!] Lua chon khong hop le. Vui long nhap lai!\n";
    }
}

// === VONG LAP CHINH CUA UNG DUNG ===
void AppInterface::run() {
    bool tiepTuc = true;
    while (tiepTuc) {
        if (currentUser == nullptr) {
            // Chua dang nhap: hien thi man hinh Login SCR-00
            bool loginSuccess = showLoginScreen();
            if (!loginSuccess) {
                // Nguoi dung chon thoat he thong
                cout << "\nCam on ban da su dung He thong Quan ly Thu vien PBL 2. Tam biet!\n";
                break;
            }
        } else {
            // Da dang nhap: dieu huong theo vai tro
            if (currentUser->layVaiTro() == "ADMIN") {
                showAdminDashboard();
            } else if (currentUser->layVaiTro() == "READER") {
                showReaderPortal();
            } else {
                currentUser = nullptr;
            }
        }
    }
}

// === SCR-00: MAN HINH DANG NHAP ===
bool AppInterface::showLoginScreen() {
    printBanner("HE THONG QUAN LY THU VIEN (PBL 2) - DANG NHAP HE THONG (SCR-00)");
    cout << "1. Dang nhap tai khoan (Thu thu / Doc gia)\n";
    cout << "0. Thoat ung dung\n";
    cout << string(88, '-') << "\n";

    int choice = readMenuChoice(0, 1);
    if (choice == 0) {
        return false;
    }

    cout << "\n--- THONG TIN DANG NHAP ---\n";
    string maND = readInput(">> Nhap Ma nguoi dung (VD: AD001, RD001): ");
    string matKhau = readInput(">> Nhap Mat khau: ");

    try {
        currentUser = readerService->authenticate(maND, matKhau);
        cout << "\n[OK] Dang nhap thanh cong! Xin chao: " << currentUser->getHoTen()
             << " (" << currentUser->layVaiTro() << ")\n";
        pauseScreen();
        return true;
    } catch (const LibraryException& e) {
        cout << "\n" << e.what() << "\n";
        pauseScreen();
        return true; // Quay lai vong lap de thu lai
    } catch (const exception& e) {
        cout << "\n[!] Loi ngoai le he thong: " << e.what() << "\n";
        pauseScreen();
        return true;
    }
}

// === SCR-A0: DASHBOARD QUAN TRI VIEN (ADMIN) ===
void AppInterface::showAdminDashboard() {
    bool dangOAdmin = true;
    while (dangOAdmin) {
        ThuThu* tt = dynamic_cast<ThuThu*>(currentUser);
        string ca = (tt != nullptr) ? tt->getCaTruc() : "Ca Sang";

        printBanner("DASHBOARD QUAN TRI VIEN (SCR-A0) - XIN CHAO: " + currentUser->getHoTen() + " [" + ca + "]");
        cout << "1. Quan ly Kho Sach (SCR-A1)\n";
        cout << "2. Quan ly Doc Gia & Nguoi dung (SCR-A2)\n";
        cout << "3. Quan ly Muon / Tra & Hang doi cho (SCR-A3)\n";
        cout << "4. He Thong, Ngan xep Lich su, Log & Cau hinh (SCR-A4)\n";
        cout << "0. Dang xuat (Logout)\n";
        cout << string(88, '-') << "\n";

        int choice = readMenuChoice(0, 4);
        switch (choice) {
            case 1:
                showBookManagementScreen();
                break;
            case 2:
                showReaderManagementScreen();
                break;
            case 3:
                showBorrowReturnManagementScreen();
                break;
            case 4:
                showSystemScreen();
                break;
            case 0:
                currentUser = nullptr;
                dangOAdmin = false;
                cout << "\n[OK] Da dang xuat tai khoan thanh cong!\n";
                pauseScreen();
                break;
        }
    }
}

// === SCR-A1: QUAN LY KHO SACH ===
void AppInterface::showBookManagementScreen() {
    bool quayLai = false;
    while (!quayLai) {
        printBanner("QUAN LY KHO SACH (SCR-A1) - TONG SO DAU SACH: " + to_string(bookRepo->getCount()));
        cout << "1. Xem danh sach sach (Duyet tien / Duyet lui voi DoublyLinkedList)\n";
        cout << "2. Tra cuu sach theo Ma Sach (Bang bam HashTable O(1))\n";
        cout << "3. Tim kiem sach theo Tu khoa (Tuyen tinh O(n) ten, tac gia, the loai)\n";
        cout << "4. Sap xep sach (Merge Sort: Tieu de, Tac gia, Nam XB, So luong)\n";
        cout << "5. Them sach moi vao kho\n";
        cout << "6. Cap nhat thong tin sach\n";
        cout << "7. Xoa sach khoi kho (Kiem tra rang buoc khong xoa sach dang muon)\n";
        cout << "8. Hoan tac thao tac sach vua thuc hien (Undo Stack: Them, Sua, Xoa)\n";
        cout << "0. Quay lai Dashboard Quan tri\n";
        cout << string(88, '-') << "\n";

        int choice = readMenuChoice(0, 8);
        switch (choice) {
            case 1: handleViewBooks(); break;
            case 2: handleSearchBookById(); break;
            case 3: handleSearchBookByKeyword(); break;
            case 4: handleSortBooks(); break;
            case 5: handleAddBook(); break;
            case 6: handleUpdateBook(); break;
            case 7: handleDeleteBook(); break;
            case 8: handleUndoBookAction(); break;
            case 0: quayLai = true; break;
        }
    }
}

void AppInterface::handleViewBooks() {
    printBanner("DANH SACH SACH TRONG KHO");
    cout << "Chon chieu duyet danh sach lien ket doi:\n";
    cout << "1. Duyet xuoi (Tu Dau den Cuoi - Forward Traversal)\n";
    cout << "2. Duyet nguoc (Tu Cuoi ve Dau - Backward Traversal)\n";
    cout << "0. Quay lai\n";
    int dir = readMenuChoice(0, 2);
    if (dir == 0) return;

    printTableBookHeader();
    if (dir == 1) {
        Node<Sach>* curr = bookRepo->getBookList().getHead();
        while (curr != nullptr) {
            printBookRow(curr->data);
            curr = curr->next;
        }
    } else {
        Node<Sach>* curr = bookRepo->getBookList().getTail();
        while (curr != nullptr) {
            printBookRow(curr->data);
            curr = curr->prev;
        }
    }
    cout << string(106, '-') << "\n";
    cout << "Tong cong: " << bookRepo->getCount() << " dau sach.\n";
    pauseScreen();
}

void AppInterface::handleSearchBookById() {
    printBanner("TRA CUU SACH THEO MA (HASH TABLE O(1))");
    string ma = readInput(">> Nhap Ma sach can tra cuu (VD: BK001): ");

    try {
        Sach* s = bookService->searchById(ma);
        if (s == nullptr) {
            cout << "\n[!] Khong tim thay sach voi ma: " << ma << "\n";
        } else {
            cout << "\n--- THONG TIN CHI TIET CUON SACH ---\n";
            printTableBookHeader();
            printBookRow(*s);
            cout << string(106, '-') << "\n";
        }
    } catch (const LibraryException& e) {
        cout << "\n" << e.what() << "\n";
    }
    pauseScreen();
}

void AppInterface::handleSearchBookByKeyword() {
    printBanner("TIM KIEM SACH THEO TU KHOA (TUYEN TINH O(n))");
    string kw = readInput(">> Nhap tu khoa tim kiem (Ten sach, Tac gia hoac The loai): ");

    try {
        DoublyLinkedList<Sach> ketQua;
        bookService->searchByKeyword(kw, ketQua);

        if (ketQua.isEmpty()) {
            cout << "\n[!] Khong tim thay sach nao phu hop voi tu khoa: \"" << kw << "\"\n";
        } else {
            cout << "\n--- KET QUA TIM KIEM (" << ketQua.size() << " cuon sach phu hop) ---\n";
            printTableBookHeader();
            Node<Sach>* curr = ketQua.getHead();
            while (curr != nullptr) {
                printBookRow(curr->data);
                curr = curr->next;
            }
            cout << string(106, '-') << "\n";
        }
    } catch (const LibraryException& e) {
        cout << "\n" << e.what() << "\n";
    }
    pauseScreen();
}

void AppInterface::handleSortBooks() {
    printBanner("SAP XEP KHO SACH (MERGE SORT IN-PLACE)");
    cout << "1. Sap xep theo Tua de sach (A - Z)\n";
    cout << "2. Sap xep theo Tac gia (A - Z)\n";
    cout << "3. Sap xep theo Nam xuat ban (Giam dan - Sach moi nhat len truoc)\n";
    cout << "4. Sap xep theo So luong hien co (Giam dan)\n";
    cout << "0. Quay lai\n";

    int choice = readMenuChoice(0, 4);
    if (choice == 0) return;

    try {
        bookService->sortBooks(choice);
        cout << "\n[OK] Sap xep thanh cong! Duoi day la danh sach sau khi sap xep:\n";
        printTableBookHeader();
        Node<Sach>* curr = bookRepo->getBookList().getHead();
        while (curr != nullptr) {
            printBookRow(curr->data);
            curr = curr->next;
        }
        cout << string(106, '-') << "\n";
    } catch (const LibraryException& e) {
        cout << "\n" << e.what() << "\n";
    }
    pauseScreen();
}

void AppInterface::handleAddBook() {
    printBanner("THEM SACH MOI VAO KHO");
    try {
        string ma = readInput(">> Nhap Ma sach (VD: BK006): ");
        string ten = readInput(">> Nhap Ten sach: ");
        string tg = readInput(">> Nhap Tac gia: ");
        string tl = readInput(">> Nhap The loai: ");
        int nam = stoi(readInput(">> Nhap Nam xuat ban: "));
        int tongSL = stoi(readInput(">> Nhap Tong so luong: "));
        int slHienCo = stoi(readInput(">> Nhap So luong hien co tren gia: "));
        string vt = readInput(">> Nhap Vi tri ke sach (Dung dinh dang [A-Z][1-9]-T[1-9], VD: A1-T2): ");

        Sach s(ma, ten, tg, tl, nam, tongSL, slHienCo, vt);
        bookService->addBook(s);
        cout << "\n[OK] Them sach moi thanh cong!\n";
    } catch (const LibraryException& e) {
        cout << "\n" << e.what() << "\n";
    } catch (const exception& e) {
        cout << "\n[!] Loi nhap lieu: " << e.what() << "\n";
    }
    pauseScreen();
}

void AppInterface::handleUpdateBook() {
    printBanner("CAP NHAT THONG TIN SACH");
    string ma = readInput(">> Nhap Ma sach can sua: ");

    Sach* s = bookService->searchById(ma);
    if (s == nullptr) {
        cout << "\n[!] Khong tim thay sach voi ma: " << ma << "\n";
        pauseScreen();
        return;
    }

    cout << "\nThong tin hien tai cua sach:\n";
    printTableBookHeader();
    printBookRow(*s);
    cout << string(106, '-') << "\n";

    try {
        cout << "\nNhap thong tin moi (de trong de giu nguyen thong tin cu):\n";
        string ten = readInput(">> Ten sach moi: ");
        if (ten.empty()) ten = s->getTenSach();

        string tg = readInput(">> Tac gia moi: ");
        if (tg.empty()) tg = s->getTacGia();

        string tl = readInput(">> The loai moi: ");
        if (tl.empty()) tl = s->getTheLoai();

        string namStr = readInput(">> Nam xuat ban moi: ");
        int nam = namStr.empty() ? s->getNamXB() : stoi(namStr);

        string tongStr = readInput(">> Tong so luong moi: ");
        int tongSL = tongStr.empty() ? s->getTongSoLuong() : stoi(tongStr);

        string hienCoStr = readInput(">> So luong hien co moi: ");
        int slHienCo = hienCoStr.empty() ? s->getSoLuongHienCo() : stoi(hienCoStr);

        string vt = readInput(">> Vi tri ke sach moi (VD: A1-T2): ");
        if (vt.empty()) vt = s->getViTri();

        Sach sUpdate(ma, ten, tg, tl, nam, tongSL, slHienCo, vt);
        bookService->updateBook(sUpdate);
        cout << "\n[OK] Cap nhat thong tin sach thanh cong!\n";
    } catch (const LibraryException& e) {
        cout << "\n" << e.what() << "\n";
    } catch (const exception& e) {
        cout << "\n[!] Loi nhap lieu: " << e.what() << "\n";
    }
    pauseScreen();
}

void AppInterface::handleDeleteBook() {
    printBanner("XOA SACH KHOI KHO");
    string ma = readInput(">> Nhap Ma sach can xoa: ");

    try {
        bookService->deleteBook(ma);
        cout << "\n[OK] Xoa sach " << ma << " thanh cong!\n";
    } catch (const LibraryException& e) {
        cout << "\n" << e.what() << "\n";
    }
    pauseScreen();
}

void AppInterface::handleUndoBookAction() {
    printBanner("HOAN TAC THAO TAC KHO SACH (UNDO STACK)");
    if (!bookService->canUndo()) {
        cout << "\n[!] Khong co thao tac nao trong Undo Stack de hoan tac!\n";
        pauseScreen();
        return;
    }

    cout << "So luong hanh dong co the Undo: " << bookService->getUndoCount() << "\n";
    string confirm = readInput(">> Ban co chac chan muon hoan tac thao tac gan nhat khong? (Y/N): ");
    if (confirm == "Y" || confirm == "y") {
        try {
            bookService->undoLastAction();
            cout << "\n[OK] Hoan tac thanh cong thao tac gan nhat!\n";
        } catch (const LibraryException& e) {
            cout << "\n" << e.what() << "\n";
        }
    } else {
        cout << "\n[!] Da huy bo thao tac Undo.\n";
    }
    pauseScreen();
}

// === SCR-A2: QUAN LY DOC GIA & NGUOI DUNG ===
void AppInterface::showReaderManagementScreen() {
    bool quayLai = false;
    while (!quayLai) {
        printBanner("QUAN LY DOC GIA & NGUOI DUNG (SCR-A2)");
        cout << "1. Xem danh sach toan bo nguoi dung trong he thong\n";
        cout << "2. Tra cuu nguoi dung theo Ma (Hash Table O(1))\n";
        cout << "3. Them tai khoan nguoi dung moi (Thu thu / Doc gia)\n";
        cout << "4. Cap nhat thong tin nguoi dung\n";
        cout << "5. Xoa tai khoan nguoi dung (Kiem tra rang buoc sach muon & tien phat)\n";
        cout << "6. Xem va thu tien phat cua doc gia\n";
        cout << "0. Quay lai Dashboard Quan tri\n";
        cout << string(88, '-') << "\n";

        int choice = readMenuChoice(0, 6);
        switch (choice) {
            case 1: handleViewReaders(); break;
            case 2: handleSearchReaderById(); break;
            case 3: handleAddReader(); break;
            case 4: handleUpdateReader(); break;
            case 5: handleDeleteReader(); break;
            case 6: handleManageFines(); break;
            case 0: quayLai = true; break;
        }
    }
}

void AppInterface::handleViewReaders() {
    printBanner("DANH SACH NGUOI DUNG TRONG HE THONG");
    cout << string(96, '-') << "\n";
    cout << "| " << left << setw(8) << "Ma ND"
         << "| " << left << setw(26) << "Ho Va Ten"
         << "| " << left << setw(14) << "So DT"
         << "| " << left << setw(10) << "Vai Tro"
         << "| " << left << setw(28) << "Thong Tin Bo Sung" << " |\n";
    cout << string(96, '-') << "\n";

    Node<NguoiDung*>* curr = readerRepo->getUserList().getHead();
    while (curr != nullptr) {
        if (curr->data != nullptr) {
            string extra = "";
            if (curr->data->layVaiTro() == "ADMIN") {
                ThuThu* tt = dynamic_cast<ThuThu*>(curr->data);
                extra = "Ca truc: " + ((tt != nullptr) ? tt->getCaTruc() : "Ca Sang");
            } else {
                DocGia* dg = dynamic_cast<DocGia*>(curr->data);
                extra = "Tien phat: " + to_string((long long)((dg != nullptr) ? dg->getTongTienPhat() : 0.0)) + " VND";
            }

            cout << "| " << left << setw(8) << curr->data->getMaND()
                 << "| " << left << setw(26) << curr->data->getHoTen()
                 << "| " << left << setw(14) << curr->data->getSoDienThoai()
                 << "| " << left << setw(10) << curr->data->layVaiTro()
                 << "| " << left << setw(28) << extra << " |\n";
        }
        curr = curr->next;
    }
    cout << string(96, '-') << "\n";
    cout << "Tong cong: " << readerRepo->getCount() << " tai khoan.\n";
    pauseScreen();
}

void AppInterface::handleSearchReaderById() {
    printBanner("TRA CUU NGUOI DUNG THEO MA (HASH TABLE O(1))");
    string ma = readInput(">> Nhap Ma nguoi dung (VD: RD001, AD001): ");

    NguoiDung* u = readerService->getReaderById(ma);
    if (u == nullptr) {
        cout << "\n[!] Khong tim thay nguoi dung voi ma: " << ma << "\n";
    } else {
        cout << "\n--- KET QUA TRA CUU ---\n";
        cout << "Ma nguoi dung : " << u->getMaND() << "\n";
        cout << "Ho va ten     : " << u->getHoTen() << "\n";
        cout << "So dien thoai : " << u->getSoDienThoai() << "\n";
        cout << "Vai tro       : " << u->layVaiTro() << "\n";
        if (u->layVaiTro() == "ADMIN") {
            ThuThu* tt = dynamic_cast<ThuThu*>(u);
            cout << "Ca truc       : " << ((tt != nullptr) ? tt->getCaTruc() : "") << "\n";
        } else {
            DocGia* dg = dynamic_cast<DocGia*>(u);
            cout << "Tien phat no  : " << to_string((long long)((dg != nullptr) ? dg->getTongTienPhat() : 0)) << " VND\n";
        }
    }
    pauseScreen();
}

void AppInterface::handleAddReader() {
    printBanner("THEM TAI KHOAN NGUOI DUNG MOI");
    cout << "Chon loai tai khoan can them:\n";
    cout << "1. Ban doc / Doc gia (READER)\n";
    cout << "2. Thu thu / Quan tri vien (ADMIN)\n";
    cout << "0. Quay lai\n";
    int type = readMenuChoice(0, 2);
    if (type == 0) return;

    try {
        string role = (type == 1) ? "READER" : "ADMIN";
        string ma = readInput(">> Nhap Ma nguoi dung (" + string(type == 1 ? "Bat dau bang RD, VD: RD010" : "Bat dau bang AD, VD: AD003") + "): ");
        string ten = readInput(">> Nhap Ho va ten: ");
        string sdt = readInput(">> Nhap So dien thoai (10 chu so bat dau bang 0): ");
        string pass = readInput(">> Nhap Mat khau dang nhap: ");
        string extra = "";
        if (type == 1) {
            extra = "0"; // Khoi tao tien phat ban dau la 0
        } else {
            extra = readInput(">> Nhap Ca lam viec (VD: Ca Sang, Ca Chieu): ");
            if (extra.empty()) extra = "Ca Sang";
        }

        NguoiDung* newUser = UserFactory::createUser(role, ma, ten, sdt, pass, extra);
        readerService->addReader(newUser);
        cout << "\n[OK] Them tai khoan " << ma << " thanh cong!\n";
    } catch (const LibraryException& e) {
        cout << "\n" << e.what() << "\n";
    } catch (const exception& e) {
        cout << "\n[!] Loi: " << e.what() << "\n";
    }
    pauseScreen();
}

void AppInterface::handleUpdateReader() {
    printBanner("CAP NHAT THONG TIN NGUOI DUNG");
    string ma = readInput(">> Nhap Ma nguoi dung can cap nhat: ");
    NguoiDung* u = readerService->getReaderById(ma);
    if (u == nullptr) {
        cout << "\n[!] Khong tim thay nguoi dung: " << ma << "\n";
        pauseScreen();
        return;
    }

    try {
        cout << "Thong tin hien tai: " << u->getHoTen() << " - SDT: " << u->getSoDienThoai() << "\n";
        string ten = readInput(">> Nhap Ho ten moi (de trong de giu nguyen): ");
        if (ten.empty()) ten = u->getHoTen();

        string sdt = readInput(">> Nhap So DT moi (de trong de giu nguyen): ");
        if (sdt.empty()) sdt = u->getSoDienThoai();

        string pass = readInput(">> Nhap Mat khau moi (de trong de giu nguyen): ");
        if (pass.empty()) pass = u->getMatKhau();

        string extra = "";
        if (u->layVaiTro() == "ADMIN") {
            ThuThu* tt = dynamic_cast<ThuThu*>(u);
            string caCu = (tt != nullptr) ? tt->getCaTruc() : "Ca Sang";
            extra = readInput(">> Nhap Ca truc moi (de trong de giu nguyen: " + caCu + "): ");
            if (extra.empty()) extra = caCu;
        } else {
            DocGia* dg = dynamic_cast<DocGia*>(u);
            extra = to_string((long long)((dg != nullptr) ? dg->getTongTienPhat() : 0.0));
        }

        NguoiDung* updatedUser = UserFactory::createUser(u->layVaiTro(), ma, ten, sdt, pass, extra);
        readerService->updateReader(updatedUser);
        cout << "\n[OK] Cap nhat thong tin nguoi dung thanh cong!\n";
    } catch (const LibraryException& e) {
        cout << "\n" << e.what() << "\n";
    }
    pauseScreen();
}

void AppInterface::handleDeleteReader() {
    printBanner("XOA TAI KHOAN NGUOI DUNG");
    string ma = readInput(">> Nhap Ma nguoi dung can xoa: ");
    try {
        readerService->deleteReader(ma);
        cout << "\n[OK] Xoa nguoi dung " << ma << " thanh cong!\n";
    } catch (const LibraryException& e) {
        cout << "\n" << e.what() << "\n";
    }
    pauseScreen();
}

void AppInterface::handleManageFines() {
    printBanner("XEM VA THU TIEN PHAT CUA DOC GIA");
    string ma = readInput(">> Nhap Ma doc gia (RDxxx): ");

    try {
        double fine = readerService->getFineAmount(ma);
        cout << "Tong tien phat hien tai cua " << ma << " la: " << (long long)fine << " VND\n";

        if (fine <= 0.0) {
            cout << "Doc gia khong co khoan tien phat nao can thu.\n";
            pauseScreen();
            return;
        }

        string payStr = readInput(">> Nhap so tien doc gia muon thanh toan (VND): ");
        double amount = stod(payStr);
        if (amount <= 0.0) {
            cout << "[!] So tien thanh toan phai lon hon 0!\n";
            pauseScreen();
            return;
        }

        readerService->clearFine(ma, amount);
        cout << "\n[OK] Thu tien phat thanh cong! So tien con no: "
             << (long long)readerService->getFineAmount(ma) << " VND\n";
    } catch (const LibraryException& e) {
        cout << "\n" << e.what() << "\n";
    } catch (const exception& e) {
        cout << "\n[!] Loi nhap lieu: " << e.what() << "\n";
    }
    pauseScreen();
}

// === SCR-A3: QUAN LY MUON / TRA & HANG DOI CHO ===
void AppInterface::showBorrowReturnManagementScreen() {
    bool quayLai = false;
    while (!quayLai) {
        printBanner("QUAN LY MUON / TRA & HANG DOI CHO (SCR-A3)");
        cout << "1. Lap phieu muon sach moi (Tu dong xep vao Hang doi Queue neu het sach)\n";
        cout << "2. Xu ly tra sach & Tinh phat (Auto-Queue: Tu dong cap sach cho nguoi dang cho)\n";
        cout << "3. Xem danh sach cac giao dich dang muon (Chua tra)\n";
        cout << "4. Xem toan bo danh sach giao dich muon / tra\n";
        cout << "5. Xem Hang doi cho muon sach (std::queue FIFO)\n";
        cout << "0. Quay lai Dashboard Quan tri\n";
        cout << string(88, '-') << "\n";

        int choice = readMenuChoice(0, 5);
        switch (choice) {
            case 1: handleBorrowBook(); break;
            case 2: handleReturnBook(); break;
            case 3: handleViewActiveBorrowings(); break;
            case 4: handleViewAllTransactions(); break;
            case 5: handleViewWaitQueue(); break;
            case 0: quayLai = true; break;
        }
    }
}

void AppInterface::handleBorrowBook() {
    printBanner("LAP PHIEU MUON SACH");
    try {
        string maDG = readInput(">> Nhap Ma doc gia (RDxxx): ");
        string maSach = readInput(">> Nhap Ma sach can muon (BKxxx): ");
        string ngayMuon = readInput(">> Nhap Ngay muon (DD/MM/YYYY, de trong de lay ngay hien tai): ");
        if (ngayMuon.empty()) {
            ngayMuon = DateUtils::getCurrentDate();
        }

        GiaoDich* gd = borrowService->borrowBook(maDG, maSach, ngayMuon, true);
        if (gd != nullptr) {
            cout << "\n================ LAP PHIEU MUON THANH CONG ================\n";
            cout << "Ma giao dich : " << gd->getMaGD() << "\n";
            cout << "Doc gia      : " << gd->getMaDocGia() << "\n";
            cout << "Sach muon    : " << gd->getMaSach() << "\n";
            cout << "Ngay muon    : " << gd->getNgayMuon() << "\n";
            cout << "Han tra      : " << gd->getHanTra() << " (Toi da " << ConfigManager::getInstance().getMaxBorrowDays() << " ngay)\n";
            cout << "Trang thai   : " << gd->getTrangThai() << "\n";
            cout << "===========================================================\n";
        } else {
            cout << "\n[CHU Y] Sach " << maSach << " da het hang tren gia!\n";
            cout << "-> He thong da tu dong them yeu cau cua ban doc " << maDG << " vao Hang doi cho (FIFO)!\n";
            cout << "-> Khi co doc gia khac tra sach, sach se duoc tu dong uu tien cap cho ban doc nay.\n";
        }
    } catch (const LibraryException& e) {
        cout << "\n" << e.what() << "\n";
    }
    pauseScreen();
}

void AppInterface::handleReturnBook() {
    printBanner("XU LY TRA SACH & TINH PHAT");
    try {
        string maGD = readInput(">> Nhap Ma giao dich can tra (GDxxx): ");
        string ngayTra = readInput(">> Nhap Ngay tra thuc te (DD/MM/YYYY, de trong de lay ngay hien tai): ");
        if (ngayTra.empty()) {
            ngayTra = DateUtils::getCurrentDate();
        }

        double fine = borrowService->returnBook(maGD, ngayTra);
        cout << "\n================ XU LY TRA SACH THANH CONG ================\n";
        cout << "Ma giao dich : " << maGD << "\n";
        cout << "Ngay tra     : " << ngayTra << "\n";
        if (fine > 0.0) {
            cout << "Ket qua      : TRA TRE HAN!\n";
            cout << "Tien phat    : " << (long long)fine << " VND (Da duoc tu dong cong vao ho so doc gia)\n";
        } else {
            cout << "Ket qua      : Tra dung han. Tien phat: 0 VND.\n";
        }
        cout << "===========================================================\n";
    } catch (const LibraryException& e) {
        cout << "\n" << e.what() << "\n";
    }
    pauseScreen();
}

void AppInterface::handleViewActiveBorrowings() {
    printBanner("DANH SACH SACH DANG DUOC MUON (CHUA TRA)");
    DoublyLinkedList<GiaoDich> activeList;
    borrowService->getActiveTransactions(activeList);

    if (activeList.isEmpty()) {
        cout << "Hien khong co giao dich muon sach nao chua tra.\n";
    } else {
        cout << string(98, '-') << "\n";
        cout << "| " << left << setw(8) << "Ma GD"
             << "| " << left << setw(9) << "Ma DG"
             << "| " << left << setw(8) << "Ma Sach"
             << "| " << left << setw(12) << "Ngay Muon"
             << "| " << left << setw(12) << "Han Tra"
             << "| " << left << setw(12) << "Ngay Tra TT"
             << "| " << left << setw(12) << "Trang Thai"
             << "| " << right << setw(11) << "Tien Phat" << " |\n";
        cout << string(98, '-') << "\n";

        Node<GiaoDich>* curr = activeList.getHead();
        while (curr != nullptr) {
            cout << "| " << left << setw(8) << curr->data.getMaGD()
                 << "| " << left << setw(9) << curr->data.getMaDocGia()
                 << "| " << left << setw(8) << curr->data.getMaSach()
                 << "| " << left << setw(12) << curr->data.getNgayMuon()
                 << "| " << left << setw(12) << curr->data.getHanTra()
                 << "| " << left << setw(12) << curr->data.getNgayTraThucTe()
                 << "| " << left << setw(12) << curr->data.getTrangThai()
                 << "| " << right << setw(7) << (long long)curr->data.getTienPhat() << " VND |\n";
            curr = curr->next;
        }
        cout << string(98, '-') << "\n";
        cout << "Tong so luot dang muon: " << activeList.size() << "\n";
    }
    pauseScreen();
}

void AppInterface::handleViewAllTransactions() {
    printBanner("LICH SU TOAN BO GIAO DICH MUON / TRA");
    Node<GiaoDich>* curr = transRepo->getTransactionList().getHead();

    if (curr == nullptr) {
        cout << "Chua co giao dich nao duoc ghi nhan.\n";
    } else {
        cout << string(98, '-') << "\n";
        cout << "| " << left << setw(8) << "Ma GD"
             << "| " << left << setw(9) << "Ma DG"
             << "| " << left << setw(8) << "Ma Sach"
             << "| " << left << setw(12) << "Ngay Muon"
             << "| " << left << setw(12) << "Han Tra"
             << "| " << left << setw(12) << "Ngay Tra TT"
             << "| " << left << setw(12) << "Trang Thai"
             << "| " << right << setw(11) << "Tien Phat" << " |\n";
        cout << string(98, '-') << "\n";

        while (curr != nullptr) {
            cout << "| " << left << setw(8) << curr->data.getMaGD()
                 << "| " << left << setw(9) << curr->data.getMaDocGia()
                 << "| " << left << setw(8) << curr->data.getMaSach()
                 << "| " << left << setw(12) << curr->data.getNgayMuon()
                 << "| " << left << setw(12) << curr->data.getHanTra()
                 << "| " << left << setw(12) << curr->data.getNgayTraThucTe()
                 << "| " << left << setw(12) << curr->data.getTrangThai()
                 << "| " << right << setw(7) << (long long)curr->data.getTienPhat() << " VND |\n";
            curr = curr->next;
        }
        cout << string(98, '-') << "\n";
        cout << "Tong so giao dich: " << transRepo->getTransactionCount() << "\n";
    }
    pauseScreen();
}

void AppInterface::handleViewWaitQueue() {
    printBanner("HANG DOI CHO MUON SACH (STD::QUEUE FIFO)");
    queue<YeuCauMuon> q = transRepo->getWaitQueue();

    if (q.empty()) {
        cout << "Hien khong co ban doc nao trong hang doi cho sach.\n";
    } else {
        cout << string(58, '-') << "\n";
        cout << "| " << left << setw(6) << "Thu Tu"
             << "| " << left << setw(14) << "Ma Doc Gia"
             << "| " << left << setw(14) << "Ma Sach Cho"
             << "| " << left << setw(16) << "Ngay Dang Ky" << " |\n";
        cout << string(58, '-') << "\n";

        int stt = 1;
        while (!q.empty()) {
            YeuCauMuon req = q.front();
            cout << "| " << left << setw(6) << stt++
                 << "| " << left << setw(14) << req.maDocGia
                 << "| " << left << setw(14) << req.maSach
                 << "| " << left << setw(16) << req.ngayDangKy << " |\n";
            q.pop();
        }
        cout << string(58, '-') << "\n";
        cout << "Tong so yeu cau dang cho: " << transRepo->getWaitQueueSize() << "\n";
    }
    pauseScreen();
}

// === SCR-A4: HE THONG, NGAN XEP LICH SU, LOG & CAU HINH ===
void AppInterface::showSystemScreen() {
    bool quayLai = false;
    while (!quayLai) {
        printBanner("HE THONG, NGAN XEP LICH SU, LOG & CAU HINH (SCR-A4)");
        cout << "1. Xem Ngan xep Lich su Giao dich toan he thong (std::stack LIFO)\n";
        cout << "2. Xem Nhat ky he thong gan nhat (data/system.log)\n";
        cout << "3. Xem va chinh sua Tham so cau hinh he thong (data/config.ini)\n";
        cout << "4. Sao luu du lieu thu cong (Manual Backup)\n";
        cout << "0. Quay lai Dashboard Quan tri\n";
        cout << string(88, '-') << "\n";

        int choice = readMenuChoice(0, 4);
        switch (choice) {
            case 1: handleViewHistoryStack(); break;
            case 2: handleViewSystemLogs(); break;
            case 3: handleViewEditConfig(); break;
            case 4: handleManualBackup(); break;
            case 0: quayLai = true; break;
        }
    }
}

void AppInterface::handleViewHistoryStack() {
    printBanner("NGAN XEP LICH SU HE THONG (STD::STACK LIFO - MOI NHAT LEN DAU)");
    stack<LichSuGiaoDich> s = transRepo->getHistoryStack();

    if (s.empty()) {
        cout << "Ngan xep lich su dang rong.\n";
    } else {
        cout << string(80, '-') << "\n";
        cout << "| " << left << setw(8) << "Ma Log"
             << "| " << left << setw(24) << "Loai Giao Dich"
             << "| " << left << setw(10) << "Ma Sach"
             << "| " << left << setw(14) << "Ma Doc Gia"
             << "| " << left << setw(16) << "Ngay Thuc Hien" << " |\n";
        cout << string(80, '-') << "\n";

        while (!s.empty()) {
            const LichSuGiaoDich& log = s.top();
            cout << "| " << left << setw(8) << log.maLog
                 << "| " << left << setw(24) << log.loaiGiaoDich
                 << "| " << left << setw(10) << log.maSach
                 << "| " << left << setw(14) << log.maDocGia
                 << "| " << left << setw(16) << log.ngayThucHien << " |\n";
            s.pop();
        }
        cout << string(80, '-') << "\n";
        cout << "Tong so ban ghi lich su: " << transRepo->getHistoryStackSize() << "\n";
    }
    pauseScreen();
}

void AppInterface::handleViewSystemLogs() {
    printBanner("NHAT KY HE THONG GAN NHAT (DATA/SYSTEM.LOG)");
    string logs = FileLogger::getInstance().readRecentLogs(35);
    cout << logs << "\n";
    pauseScreen();
}

void AppInterface::handleViewEditConfig() {
    printBanner("XEM & CHINH SUA CAU HINH HE THONG (DATA/CONFIG.INI)");
    ConfigManager& cfg = ConfigManager::getInstance();

    cout << "Tham so cau hinh hien tai:\n";
    cout << "1. FINE_PER_DAY     : " << (long long)cfg.getFinePerDay() << " VND / ngay\n";
    cout << "2. MAX_BORROW_DAYS  : " << cfg.getMaxBorrowDays() << " ngay\n";
    cout << "3. HASH_TABLE_SIZE  : " << cfg.getHashTableSize() << "\n";
    cout << "4. MIN_YEAR         : " << cfg.getMinYear() << "\n";
    cout << "5. MAX_YEAR         : " << cfg.getMaxYear() << "\n";
    cout << "0. Khong thay doi va quay lai\n";

    int choice = readMenuChoice(0, 5);
    if (choice == 0) return;

    try {
        if (choice == 1) {
            double fine = stod(readInput(">> Nhap FINE_PER_DAY moi (VND): "));
            if (fine >= 0) cfg.setFinePerDay(fine);
        } else if (choice == 2) {
            int days = stoi(readInput(">> Nhap MAX_BORROW_DAYS moi (ngay): "));
            if (days > 0) cfg.setMaxBorrowDays(days);
        } else if (choice == 3) {
            int sz = stoi(readInput(">> Nhap HASH_TABLE_SIZE moi: "));
            if (sz > 0) cfg.setHashTableSize(sz);
        } else if (choice == 4) {
            int y = stoi(readInput(">> Nhap MIN_YEAR moi: "));
            cfg.setMinYear(y);
        } else if (choice == 5) {
            int y = stoi(readInput(">> Nhap MAX_YEAR moi: "));
            cfg.setMaxYear(y);
        }

        cfg.saveToFile();
        cout << "\n[OK] Cap nhat cau hinh thanh cong va da dong bo xuong data/config.ini!\n";
    } catch (const exception& e) {
        cout << "\n[!] Loi cap nhat: " << e.what() << "\n";
    }
    pauseScreen();
}

void AppInterface::handleManualBackup() {
    printBanner("SAO LUU DU LIEU THU CONG (MANUAL BACKUP)");
    cout << "He thong se sao chep cac file du lieu sang thu muc data/backup_...\n";
    try {
        bookRepo->saveToFile();
        readerRepo->saveToFile();
        transRepo->saveToFile();
        cout << "[OK] Toan bo du lieu RAM da duoc dong bo an toan xuong dia!\n";
    } catch (const LibraryException& e) {
        cout << "\n" << e.what() << "\n";
    }
    pauseScreen();
}

// === SCR-R0: PORTAL BAN DOC (READER) ===
void AppInterface::showReaderPortal() {
    bool dangOBanDoc = true;
    while (dangOBanDoc) {
        DocGia* dg = dynamic_cast<DocGia*>(currentUser);
        double fine = (dg != nullptr) ? dg->getTongTienPhat() : 0.0;

        printBanner("PORTAL BAN DOC (SCR-R0) - XIN CHAO: " + currentUser->getHoTen() + " [Ma: " + currentUser->getMaND() + "]");
        if (fine > 0.0) {
            cout << "[!] CANH BAO: Ban dang co tien phat qua han chua thanh toan: " << (long long)fine << " VND!\n";
            cout << "    (Vui long thanh toan tai ban thu thu de co the tiep tuc muon sach)\n";
            cout << string(88, '-') << "\n";
        }

        cout << "1. Tra cuu Kho Sach & Dang ky cho muon (SCR-R1)\n";
        cout << "2. Sach cua toi & Ho so ca nhan (SCR-R2)\n";
        cout << "0. Dang xuat (Logout)\n";
        cout << string(88, '-') << "\n";

        int choice = readMenuChoice(0, 2);
        switch (choice) {
            case 1:
                showReaderBookSearchScreen();
                break;
            case 2:
                showMyProfileScreen();
                break;
            case 0:
                currentUser = nullptr;
                dangOBanDoc = false;
                cout << "\n[OK] Da dang xuat tai khoan thanh cong!\n";
                pauseScreen();
                break;
        }
    }
}

// === SCR-R1: TRA CUU SACH & DANG KY HANG DOI CHO (READER) ===
void AppInterface::showReaderBookSearchScreen() {
    bool quayLai = false;
    while (!quayLai) {
        printBanner("TRA CUU SACH & DANG KY CHO MUON (SCR-R1)");
        cout << "1. Xem toan bo danh sach sach trong thu vien\n";
        cout << "2. Tra cuu sach theo Ma (O(1))\n";
        cout << "3. Tim kiem sach theo Tu khoa (O(n))\n";
        cout << "4. Sap xep sach theo Tieu de / Tac gia / Nam XB\n";
        cout << "5. Dang ky vao Hang doi cho muon sach (khi sach da het tren gia)\n";
        cout << "0. Quay lai Portal Ban doc\n";
        cout << string(88, '-') << "\n";

        int choice = readMenuChoice(0, 5);
        switch (choice) {
            case 1: handleViewBooks(); break;
            case 2: handleSearchBookById(); break;
            case 3: handleSearchBookByKeyword(); break;
            case 4: handleSortBooks(); break;
            case 5: handleReaderRegisterWaitlist(); break;
            case 0: quayLai = true; break;
        }
    }
}

void AppInterface::handleReaderRegisterWaitlist() {
    printBanner("DANG KY VAO HANG DOI CHO MUON SACH");
    string maSach = readInput(">> Nhap Ma sach ban muon dang ky cho (VD: BK002): ");

    try {
        Sach* s = bookService->searchById(maSach);
        if (s == nullptr) {
            cout << "\n[!] Khong tim thay sach voi ma: " << maSach << "\n";
            pauseScreen();
            return;
        }

        if (s->getSoLuongHienCo() > 0) {
            cout << "\n[THONG BAO] Cuon sach nay hien DANG CON " << s->getSoLuongHienCo()
                 << " quyen tren gia tai vi tri: " << s->getViTri() << "!\n";
            cout << "-> Ban khong can vao hang doi cho ma co the truc tiep lien he Thu thu de muon ngay.\n";
            pauseScreen();
            return;
        }

        // Sach da het hang tren gia: tien hanh dang ky vao hang doi FIFO
        string ngay = DateUtils::getCurrentDate();
        borrowService->borrowBook(currentUser->getMaND(), maSach, ngay, true);
        cout << "\n[OK] Ban da duoc xep vao Hang doi cho cuon sach " << maSach << " thanh cong!\n";
        cout << "Khi co ban doc tra sach nay, he thong se tu dong uu tien giu sach cho ban.\n";
    } catch (const LibraryException& e) {
        cout << "\n" << e.what() << "\n";
    }
    pauseScreen();
}

// === SCR-R2: SACH CUA TOI & HO SO CA NHAN (READER) ===
void AppInterface::showMyProfileScreen() {
    bool quayLai = false;
    while (!quayLai) {
        printBanner("SACH CUA TOI & HO SO CA NHAN (SCR-R2)");
        cout << "1. Xem danh sach sach ban dang muon va han tra\n";
        cout << "2. Xem chi tiet tien phat qua han tich luy\n";
        cout << "3. Xem lich su muon / tra ca nhan cua ban\n";
        cout << "4. Cap nhat thong tin ca nhan (Ho ten, So dien thoai)\n";
        cout << "5. Doi mat khau dang nhap\n";
        cout << "0. Quay lai Portal Ban doc\n";
        cout << string(88, '-') << "\n";

        int choice = readMenuChoice(0, 5);
        switch (choice) {
            case 1: handleViewMyBorrowedBooks(); break;
            case 2: handleViewMyFines(); break;
            case 3: handleViewMyHistory(); break;
            case 4: handleUpdateMyProfile(); break;
            case 5: handleChangeMyPassword(); break;
            case 0: quayLai = true; break;
        }
    }
}

void AppInterface::handleViewMyBorrowedBooks() {
    printBanner("DANH SACH SACH TOI DANG MUON");
    DoublyLinkedList<GiaoDich> myTrans;
    borrowService->getTransactionsByReader(currentUser->getMaND(), myTrans);

    bool coSachMuon = false;
    cout << string(86, '-') << "\n";
    cout << "| " << left << setw(8) << "Ma GD"
         << "| " << left << setw(10) << "Ma Sach"
         << "| " << left << setw(14) << "Ngay Muon"
         << "| " << left << setw(14) << "Han Tra"
         << "| " << left << setw(14) << "Trang Thai"
         << "| " << right << setw(17) << "Ghi Chu" << " |\n";
    cout << string(86, '-') << "\n";

    Node<GiaoDich>* curr = myTrans.getHead();
    while (curr != nullptr) {
        if (curr->data.getTrangThai() == "DANG_MUON") {
            coSachMuon = true;
            int tre = DateUtils::diffDays(curr->data.getHanTra(), DateUtils::getCurrentDate());
            string ghiChu = (tre > 0) ? ("TRE " + to_string(tre) + " NGAY!") : "Con trong han";

            cout << "| " << left << setw(8) << curr->data.getMaGD()
                 << "| " << left << setw(10) << curr->data.getMaSach()
                 << "| " << left << setw(14) << curr->data.getNgayMuon()
                 << "| " << left << setw(14) << curr->data.getHanTra()
                 << "| " << left << setw(14) << curr->data.getTrangThai()
                 << "| " << right << setw(17) << ghiChu << " |\n";
        }
        curr = curr->next;
    }
    cout << string(86, '-') << "\n";

    if (!coSachMuon) {
        cout << "Hien tai ban khong co cuon sach nao dang muon.\n";
    }
    pauseScreen();
}

void AppInterface::handleViewMyFines() {
    printBanner("TIEN PHAT QUA HAN TICH LUY");
    DocGia* dg = dynamic_cast<DocGia*>(currentUser);
    double fine = (dg != nullptr) ? dg->getTongTienPhat() : 0.0;

    cout << "Doc gia           : " << currentUser->getHoTen() << " (" << currentUser->getMaND() << ")\n";
    cout << "Tong tien phat no : " << (long long)fine << " VND\n";

    if (fine > 0.0) {
        cout << "\n[CHU Y] Vui long mang tien den quay thu thu de thanh toan som nhat co the!\n";
    } else {
        cout << "\n[OK] Ban khong co bat ky khoan no tien phat nao. Xin cam on ban!\n";
    }
    pauseScreen();
}

void AppInterface::handleViewMyHistory() {
    printBanner("LICH SU MUON / TRA CA NHAN");
    DoublyLinkedList<LichSuGiaoDich> myLogs;
    borrowService->getHistoryByReader(currentUser->getMaND(), myLogs);

    if (myLogs.isEmpty()) {
        cout << "Chua co hoat dong giao dich nao cua ban duoc ghi nhan.\n";
    } else {
        cout << string(68, '-') << "\n";
        cout << "| " << left << setw(10) << "Ma Log"
             << "| " << left << setw(26) << "Hanh Dong"
             << "| " << left << setw(12) << "Ma Sach"
             << "| " << left << setw(14) << "Ngay Thuc Hien" << " |\n";
        cout << string(68, '-') << "\n";

        Node<LichSuGiaoDich>* curr = myLogs.getHead();
        while (curr != nullptr) {
            cout << "| " << left << setw(10) << curr->data.maLog
                 << "| " << left << setw(26) << curr->data.loaiGiaoDich
                 << "| " << left << setw(12) << curr->data.maSach
                 << "| " << left << setw(14) << curr->data.ngayThucHien << " |\n";
            curr = curr->next;
        }
        cout << string(68, '-') << "\n";
    }
    pauseScreen();
}

void AppInterface::handleUpdateMyProfile() {
    printBanner("CAP NHAT HO SO CA NHAN");
    try {
        cout << "Ho ten hien tai : " << currentUser->getHoTen() << "\n";
        cout << "So DT hien tai  : " << currentUser->getSoDienThoai() << "\n";

        string tenMoi = readInput(">> Nhap Ho ten moi (de trong de giu nguyen): ");
        if (tenMoi.empty()) tenMoi = currentUser->getHoTen();

        string sdtMoi = readInput(">> Nhap So DT moi (de trong de giu nguyen): ");
        if (sdtMoi.empty()) sdtMoi = currentUser->getSoDienThoai();

        readerService->updateProfile(currentUser->getMaND(), tenMoi, sdtMoi);
        cout << "\n[OK] Ho so ca nhan cua ban da duoc cap nhat thanh cong!\n";
    } catch (const LibraryException& e) {
        cout << "\n" << e.what() << "\n";
    }
    pauseScreen();
}

void AppInterface::handleChangeMyPassword() {
    printBanner("DOI MAT KHAU DANG NHAP");
    try {
        string oldPass = readInput(">> Nhap Mat khau cu: ");
        string newPass = readInput(">> Nhap Mat khau moi: ");
        string confirm = readInput(">> Xac nhan lai Mat khau moi: ");

        if (newPass != confirm) {
            cout << "\n[!] Xac nhan mat khau khong khop! Thao tac bi huy.\n";
            pauseScreen();
            return;
        }

        readerService->changePassword(currentUser->getMaND(), oldPass, newPass);
        cout << "\n[OK] Doi mat khau thanh cong!\n";
    } catch (const LibraryException& e) {
        cout << "\n" << e.what() << "\n";
    }
    pauseScreen();
}
