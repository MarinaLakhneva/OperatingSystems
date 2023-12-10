#pragma once
#include <ctime>
#include <semaphore.h>
#include <stdexcept>

void wait_sem_with_timeout(sem_t *sem, size_t timeout);