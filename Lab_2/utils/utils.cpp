#include "utils.h"
#include <ctime>
#include <semaphore.h>
#include <stdexcept>
#include <sys/syslog.h>

void wait_sem_with_timeout(sem_t *sem, size_t timeout) {
  struct timespec ts;
  if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
    syslog(LOG_ERR, "ERROR: failed to get current time");
    std::exit(1);
  }
  ts.tv_sec += timeout;

  int s;
  while ((s = sem_timedwait(sem, &ts)) == -1 && errno == EINTR)
    continue;
  if (s == -1 && errno == ETIMEDOUT) {
    syslog(LOG_ERR, "ERROR: Semaphore wait timedout");
    std::exit(1);
  }
}