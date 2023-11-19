#include<sys/epoll.h>
#include<fcntl.h>
#include<unistd.h>
#include <stdio.h>
#include <stdlib.h>

void handle_in_event(int fd) {
  char buffer[1024];
  ssize_t bytesRead = read(fd, &buffer, sizeof(buffer));

  if (bytesRead == -1) {
    fprintf(stderr, "error reading data from fd\n");
    exit(1);
  }

  if (bytesRead == 0) {
    fprintf(stdout,"fd closed\n");
    close(fd);
    exit(1);
  }

  fprintf(stdout, "read %ld bytes from the fd\n", bytesRead);
  buffer[bytesRead] = '\0';
  fprintf(stdout,"data from fd:%s\n", buffer);
}

int main() {
  // create an epoll instance that monitors only 1 file descriptor
  // not that "size" is just a hint of the number of fds
  int epoll_fd = epoll_create(1);

  // check for errors
  if (epoll_fd == -1) {
    fprintf(stderr, "error creating the epoll instance\n");
    exit(1);
  }

  // add file descriptor to the epoll instance
  struct epoll_event event;
  // set the desired events that we want to listen to
  event.events = EPOLLIN;
  // set the file descriptor that we want to read from
  event.data.fd = 0;
  
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0 , &event) == -1){
    fprintf(stderr, "error adding fd to epoll\n");
    exit(1);
  }

  int MAX_EVENTS = 100;
  struct epoll_event events[MAX_EVENTS];

  fprintf(stdout,"waiting for event on stdout ...\n");
  // wait untill there is data to read
  int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

  if(num_events == -1) {
    fprintf(stderr, "error waiting for events from fd list\n");
    exit(1);
  } 
  
  fprintf(stdout, "got an event from %d fds\n", num_events);

  // iterate through all events
  for (int a = 0; a < num_events; a++) {
    // check what kind of event we have
    int current_fd = events[a].data.fd;
    if (events[a].events & EPOLLIN) {
      fprintf(stdout, "got an EPOLLIN event\n");
      handle_in_event(current_fd);
    } else {
      fprintf(stdout,"got another event\n");
    }
  }
  return 0;
}


