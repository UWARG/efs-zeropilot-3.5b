#pragma once

#include <gmock/gmock.h>
#include "queue_iface.hpp"

template <typename T>
class Node {
    private:
        Node<T> *next;
        T *data;

    public:
        Node() {
            data = NULL;
            next = NULL; 
        }
        Node(T* entry) {
            data = entry;
            next = NULL;
        }

        Node<T>* getNext() {
            return next;
        }

        void setNext(Node<T> *nextNode) {
            next = nextNode;
        }

        T* getData() {
            return data;
        }

        void setData(T *newData) {
            data = newData;
        }
};

template <typename T>
class FakeMessageQueue : public IMessageQueue<T> {
    private:
        int msgCount;
        Node<T>* head;
    public:
        FakeMessageQueue() {
            msgCount = 0;
            head = NULL;
        };
        int count() override {
            return msgCount;
        }

        int push(T *message) override {
            if (head == NULL) {
                head = new Node<T>(message);
            } else {
                Node<T> *curNode;
                Node<T> *nextNode;

                nextNode = head;

                while(nextNode != NULL) {
                    curNode = nextNode;
                    nextNode = curNode->getNext();
                }

                nextNode = new Node<T>(message);
                curNode->setNext(nextNode);
            }
            msgCount++;
            return 0;
        }

        int get(T *message) override {
            if (head == NULL) {
                return -1;
            }
            message = head->getData();
            Node<T> *temp = head->getNext();

            delete head;
            head = temp;

            msgCount--;
            return 0;
        }

        int remainingCapacity() override {
            return 16 - msgCount;
        }
};