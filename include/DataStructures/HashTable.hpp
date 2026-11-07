#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <iostream>
#include <string>

using namespace std;

// Cau truc HashNode luu cap khoa va gia tri trong danh sach lien ket cua tung bucket
template <typename K, typename V>
struct HashNode {
    K key;                  // Khoa tim kiem (vi du: maSach, maDocGia, maND)
    V value;                // Gia tri luu tru (vi du: con tro Sach*, NguoiDung*)
    HashNode<K, V>* next;   // Con tro tro den node tiep theo khi xay ra dung do (Separate Chaining)

    // Ham tao khoi tao mot node bam moi
    HashNode(const K& k, const V& val) : key(k), value(val), next(nullptr) {}
};

// Lop bang bam HashTable tu cai dat 100% dung phuong phap noi ket rieng biet (Separate Chaining)
template <typename K, typename V>
class HashTable {
private:
    HashNode<K, V>** table; // Mang dong chua cac con tro tro den dau cac bucket chain
    int tableSize;          // Kich thuoc cua bang bam (mac dinh 101 tu ConfigManager)
    int itemCount;          // So luong phan tu hien co trong bang bam

public:
    // Ham tao bang bam voi kich thuoc truyen vao
    HashTable(int size = 101) : tableSize(size), itemCount(0) {
        table = new HashNode<K, V>*[tableSize];
        // Khoi tao tat ca cac bucket ban dau deu la nullptr
        for (int i = 0; i < tableSize; i++) {
            table[i] = nullptr;
        }
    }

    // Ham huy giai phong toan bo bo nho cac HashNode va mang table
    ~HashTable() {
        clear();
        delete[] table;
        table = nullptr;
    }

    // Cam phep sao chep de dam bao an toan bo nho con tro tho
    HashTable(const HashTable<K, V>&) = delete;
    HashTable<K, V>& operator=(const HashTable<K, V>&) = delete;

    // Ham bam chuoi da thuc: h = (h * 31 + c) % M
    int hashFunction(const string& key) const {
        unsigned long hash = 0;
        for (int i = 0; i < (int)key.length(); i++) {
            unsigned char c = (unsigned char)key[i];
            hash = (hash * 31 + c) % tableSize;
        }
        return (int)hash;
    }

    // Them hoac cap nhat mot cap (key, value) vao bang bam
    void insert(const K& key, V value) {
        int index = hashFunction(key);

        // Kiem tra xem key da ton tai trong bucket hay chua
        HashNode<K, V>* curr = table[index];
        while (curr != nullptr) {
            if (curr->key == key) {
                // Neu key da ton tai thi cap nhat lai value
                curr->value = value;
                return;
            }
            curr = curr->next;
        }

        // Neu chua ton tai thi chen node moi vao dau bucket chain (O(1))
        HashNode<K, V>* newNode = new HashNode<K, V>(key, value);
        newNode->next = table[index];
        table[index] = newNode;
        itemCount++;
    }

    // Tra cuu gia tri theo key trong O(1) thoi gian trung binh
    // Tra ve nullptr neu khong tim thay
    V search(const K& key) const {
        int index = hashFunction(key);

        HashNode<K, V>* curr = table[index];
        while (curr != nullptr) {
            if (curr->key == key) {
                return curr->value;
            }
            curr = curr->next;
        }

        // Khong tim thay khoa trong bang bam
        return nullptr;
    }

    // Xoa phan tu co khoa key khoi bang bam va giai phong HashNode
    bool remove(const K& key) {
        int index = hashFunction(key);

        HashNode<K, V>* curr = table[index];
        HashNode<K, V>* prev = nullptr;

        while (curr != nullptr) {
            if (curr->key == key) {
                // Neu node can xoa o ngay dau bucket
                if (prev == nullptr) {
                    table[index] = curr->next;
                } else {
                    prev->next = curr->next;
                }

                delete curr;
                itemCount--;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }

        // Khong tim thay de xoa
        return false;
    }

    // Xoa toan bo cac node trong tat ca cac bucket
    void clear() {
        for (int i = 0; i < tableSize; i++) {
            HashNode<K, V>* curr = table[i];
            while (curr != nullptr) {
                HashNode<K, V>* temp = curr;
                curr = curr->next;
                delete temp;
            }
            table[i] = nullptr;
        }
        itemCount = 0;
    }

    // Lay so luong phan tu hien co
    int size() const {
        return itemCount;
    }

    // Lay kich thuoc cua bang bam
    int getTableSize() const {
        return tableSize;
    }

    // Kiem tra bang bam co rong hay khong
    bool isEmpty() const {
        return itemCount == 0;
    }
};

#endif // HASH_TABLE_HPP
