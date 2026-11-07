#ifndef DOUBLY_LINKED_LIST_HPP
#define DOUBLY_LINKED_LIST_HPP

#include "../Interfaces/ISortStrategy.h"
#include <iostream>

using namespace std;

// Cau truc Node dai dien cho mot phan tu trong danh sach lien ket doi
template <typename T>
struct Node {
    T data;             // Du lieu luu tru ben trong node
    Node<T>* prev;      // Con tro tro den node phia truoc
    Node<T>* next;      // Con tro tro den node phia sau

    // Ham tao khoi tao node voi du lieu va hai con tro ban dau la nullptr
    Node(const T& value) : data(value), prev(nullptr), next(nullptr) {}
};

// Lop danh sach lien ket doi tu cai dat 100% dung con tro tho
template <typename T>
class DoublyLinkedList {
private:
    Node<T>* head;      // Con tro quan ly node dau danh sach
    Node<T>* tail;      // Con tro quan ly node cuoi danh sach
    int count;          // So luong phan tu hien co trong danh sach

    // Ham tach danh sach lam doi su dung ky thuat hai con tro fast va slow
    Node<T>* split(Node<T>* source) {
        if (source == nullptr || source->next == nullptr) {
            return nullptr;
        }

        Node<T>* fast = source;
        Node<T>* slow = source;

        // Con tro fast di chuyen 2 buoc moi lan, slow di chuyen 1 buoc
        while (fast->next != nullptr && fast->next->next != nullptr) {
            fast = fast->next->next;
            slow = slow->next;
        }

        // Diem cat danh sach chinh la node tiep theo cua slow
        Node<T>* secondHalf = slow->next;
        slow->next = nullptr;
        if (secondHalf != nullptr) {
            secondHalf->prev = nullptr;
        }

        return secondHalf;
    }

    // Ham tron hai danh sach da sap xep bang cach noi lai cac con tro next va prev tai cho (in-place)
    // Tuyet doi khong hoan doi (swap) gia tri data cua cac node de giu nguyen dia chi bo nho
    Node<T>* merge(Node<T>* first, Node<T>* second, const ISortStrategy<T>& strategy) {
        if (first == nullptr) return second;
        if (second == nullptr) return first;

        Node<T>* resultHead = nullptr;
        Node<T>* current = nullptr;

        // Chon node dau tien phu hop theo chien luoc sap xep
        if (strategy.shouldComeBefore(first->data, second->data)) {
            resultHead = first;
            first = first->next;
        } else {
            resultHead = second;
            second = second->next;
        }
        resultHead->prev = nullptr;
        current = resultHead;

        // Duyet va noi lan luot tung node cua ca hai nua vao chuoi ket qua
        while (first != nullptr && second != nullptr) {
            if (strategy.shouldComeBefore(first->data, second->data)) {
                current->next = first;
                first->prev = current;
                current = first;
                first = first->next;
            } else {
                current->next = second;
                second->prev = current;
                current = second;
                second = second->next;
            }
        }

        // Noi phan con lai neu mot trong hai nua van con node
        if (first != nullptr) {
            current->next = first;
            first->prev = current;
        } else if (second != nullptr) {
            current->next = second;
            second->prev = current;
        }

        return resultHead;
    }

    // Ham de quy sap xep tron theo giai thuat Merge Sort
    Node<T>* mergeSortHelper(Node<T>* source, const ISortStrategy<T>& strategy) {
        // Truong hop co so: danh sach rong hoac chi co 1 node
        if (source == nullptr || source->next == nullptr) {
            return source;
        }

        // Buoc 1: Chia doi danh sach tai vi tri con tro slow
        Node<T>* secondHalf = split(source);

        // Buoc 2: De quy sap xep tren tung nua
        source = mergeSortHelper(source, strategy);
        secondHalf = mergeSortHelper(secondHalf, strategy);

        // Buoc 3: Tron hai nua da sap xep lai voi nhau
        return merge(source, secondHalf, strategy);
    }

public:
    // Ham tao khoi tao danh sach rong
    DoublyLinkedList() : head(nullptr), tail(nullptr), count(0) {}

    // Ham huy giai phong toan bo bo nho cac node da cap phat dong
    ~DoublyLinkedList() {
        clear();
    }

    // Khong cho phep sao chep nong de tranh loi con tro treo (dangling pointer)
    DoublyLinkedList(const DoublyLinkedList<T>&) = delete;
    DoublyLinkedList<T>& operator=(const DoublyLinkedList<T>&) = delete;

    // Chen mot phan tu vao cuoi danh sach, tra ve con tro T* tro thang vao data ben trong node
    // Con tro nay se duoc luu vao HashTable de tra cuu O(1)
    T* insertTail(const T& value) {
        Node<T>* newNode = new Node<T>(value);

        if (head == nullptr) {
            // Truong hop danh sach dang rong
            head = newNode;
            tail = newNode;
        } else {
            // Gan node moi vao sau tail hien tai
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }

        count++;
        return &(newNode->data);
    }

    // Chen mot phan tu vao dau danh sach
    T* insertHead(const T& value) {
        Node<T>* newNode = new Node<T>(value);

        if (head == nullptr) {
            head = newNode;
            tail = newNode;
        } else {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }

        count++;
        return &(newNode->data);
    }

    // Xoa mot node cu the khoi danh sach lien ket doi va giai phong bo nho bang delete
    bool removeNode(Node<T>* target) {
        if (target == nullptr || head == nullptr) {
            return false;
        }

        // Neu target la node dau tien
        if (target == head) {
            head = target->next;
            if (head != nullptr) {
                head->prev = nullptr;
            } else {
                tail = nullptr;
            }
        }
        // Neu target la node cuoi cung
        else if (target == tail) {
            tail = target->prev;
            if (tail != nullptr) {
                tail->next = nullptr;
            } else {
                head = nullptr;
            }
        }
        // Neu target o vi tri giua danh sach
        else {
            if (target->prev != nullptr) {
                target->prev->next = target->next;
            }
            if (target->next != nullptr) {
                target->next->prev = target->prev;
            }
        }

        delete target;
        count--;
        return true;
    }

    // Xoa phan tu dau tien thoa man dieu kien kiem tra (predicate)
    template <typename Predicate>
    bool removeIf(Predicate pred) {
        Node<T>* curr = head;
        while (curr != nullptr) {
            if (pred(curr->data)) {
                return removeNode(curr);
            }
            curr = curr->next;
        }
        return false;
    }

    // Thuc hien Merge Sort tai cho dua tren chien luoc duoc truyen vao
    void mergeSort(const ISortStrategy<T>& strategy) {
        if (head == nullptr || head->next == nullptr) {
            return;
        }

        // Goi ham de quy sap xep va cap nhat lai head
        head = mergeSortHelper(head, strategy);

        // Duyet tu head den node cuoi cung de cap nhat lai con tro tail chinh xac
        Node<T>* curr = head;
        while (curr != nullptr && curr->next != nullptr) {
            curr = curr->next;
        }
        tail = curr;
    }

    // Xoa toan bo cac node va dua danh sach ve trang thai rong
    void clear() {
        Node<T>* curr = head;
        while (curr != nullptr) {
            Node<T>* temp = curr;
            curr = curr->next;
            delete temp;
        }
        head = nullptr;
        tail = nullptr;
        count = 0;
    }

    // Lay con tro dau danh sach
    Node<T>* getHead() const {
        return head;
    }

    // Lay con tro cuoi danh sach
    Node<T>* getTail() const {
        return tail;
    }

    // Lay so luong phan tu trong danh sach
    int size() const {
        return count;
    }

    // Kiem tra danh sach co rong hay khong
    bool isEmpty() const {
        return count == 0;
    }
};

#endif // DOUBLY_LINKED_LIST_HPP
