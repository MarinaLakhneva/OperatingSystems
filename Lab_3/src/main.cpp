#include "set_coarse.h"
#include <cassert>
#include <iostream>
#include <pthread.h>
#include <vector>

void *write_to_set(void *set) {
    int stop = 500;
    for (int idx = 0; idx < stop; ++idx)
        ((SetCoarse *)set)->add(idx);

    pthread_exit(NULL);
}

void *read_from_set(void *set) {
    int stop = 450;
    for (int idx = 0; idx < stop; ++idx)
        ((SetCoarse *)set)->remove(idx);

    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    size_t writers = 50;
    size_t readers = 50;

    SetCoarse set_rough;

    std::cout << "Start writers test on Coarse Set" << std::endl;
    std::vector<pthread_t> write_threads(writers);

    for (auto &writer : write_threads)
        pthread_create(&writer, NULL, write_to_set, &set_rough);

    for (auto &writer : write_threads)
        pthread_join(writer, NULL);

    for (size_t idx = 0; idx < 500; ++idx)
        assert(set_rough.contains(idx));
    std::cout << "Writers test on Coarse Set successful" << std::endl;

    std::cout << "Start readers test on Coarse Set" << std::endl;
    std::vector<pthread_t> read_threads(readers);

    for (auto &reader : read_threads)
        pthread_create(&reader, NULL, read_from_set, &set_rough);

    for (auto &reader : read_threads)
        pthread_join(reader, NULL);

    for (size_t idx = 450; idx < 500; ++idx)
        assert(set_rough.contains(idx));
    std::cout << "Readers test on Coarse Set successful" << std::endl;

    return 0;
}
