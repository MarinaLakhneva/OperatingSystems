#pragma once

#include "node_coarse.h"
#include <pthread.h>

class SetCoarse {
  public:
    SetCoarse() {
        head = nullptr;
        pthread_mutex_init(&mutex, NULL);
    }

    ~SetCoarse() {
        NodeCoarse *node = head;
        while (node) {
            NodeCoarse *next = node->next;
            delete node;
            node = next;
        }

        pthread_mutex_destroy(&mutex);
    }

    bool add(int val) {
        pthread_mutex_lock(&mutex);
        bool res = add_priv(val);
        if (res)
            ++set_size;
        pthread_mutex_unlock(&mutex);
        return res;
    }

    bool remove(int val) {
        pthread_mutex_lock(&mutex);
        bool res = remove_priv(val);
        if (res)
            --set_size;
        pthread_mutex_unlock(&mutex);
        return res;
    }

    bool contains(int val) {
        pthread_mutex_lock(&mutex);
        bool res = contains_priv(val);
        pthread_mutex_unlock(&mutex);
        return res;
    }
    size_t size() const { return set_size; }

  private:
    pthread_mutex_t mutex;
    NodeCoarse *head, *tail;
    size_t set_size = 0;

    bool add_priv(int val) {
        if (head == nullptr) {
            head = new NodeCoarse(val);
            tail = head;
            return true;
        }

        NodeCoarse *node = head;
        while (node != tail) {
            if (node->val() == val)
                return false;
            else if (node->val() < val && node->next->val() > val) {
                NodeCoarse *new_node = new NodeCoarse(val, node, node->next);
                node->next = new_node;
                return true;
            } else
                node = node->next;
        }

        if (node->val() == val)
            return false;
        else {
            node->next = new NodeCoarse(val, node);
            tail = node->next;
            return true;
        }
    }

    bool remove_priv(int val) {
        if (head == nullptr)
            return false;

        NodeCoarse *node = head;
        while (node) {
            if (node->val() == val) {
                if (!node->prev && !node->next) {
                    head = nullptr;
                    tail = nullptr;
                } else if (!node->prev) {
                    head = head->next;
                    head->prev = nullptr;
                } else if (!node->next) {
                    tail = tail->prev;
                    tail->next = nullptr;
                } else {
                    node->prev->next = node->next;
                    node->next->prev = node->prev;
                }
                delete node;
                return true;
            } else
                node = node->next;
        }

        return false;
    }

    bool contains_priv(int val) {
        if (head == nullptr)
            return false;

        NodeCoarse *node = head;
        while (node) {
            if (node->val() == val)
                return true;
            else
                node = node->next;
        }

        return false;
    }
};