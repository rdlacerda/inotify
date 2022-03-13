#include <sys/inotify.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

#define BUF_LEN (3 * (sizeof(struct inotify_event) + NAME_MAX + 1))
#define TRUE 1

void on_error(int descriptor, const char * error_message, int close_program)
{
  if(descriptor == -1) {
    puts(error_message);

    if(close_program)
      exit(EXIT_FAILURE);
  } //end if
} //end function

static void display_inotify_event(struct inotify_event * e)
{
  if(e->mask & IN_CLOSE_WRITE) {
    printf("The file %d was modified\n", e->wd);
  } //end if
} //end function

int main(int argc, char * argv[])
{
  int inotify_fd, watch;
  char buffer[BUF_LEN];
  ssize_t num_read;
  struct inotify_event * event;
  char *p;
  int j;

  inotify_fd = inotify_init();
  on_error(inotify_fd, "Err when creating inotify", 1);

  for(j=1; j < argc; j++) {
    watch = inotify_add_watch(inotify_fd, argv[j], IN_CLOSE_WRITE);
    on_error(inotify_fd, "Err when add inotify", 1);
  }

  while(TRUE) {
    num_read = read(inotify_fd, buffer, BUF_LEN);
    on_error(num_read, "Err when reading from inotify", 1);
    
    for(p=buffer; p < buffer + num_read; ) {
      event = (struct inotify_event *) p;
      display_inotify_event(event);
      p += sizeof(struct inotify_event) + event->len;
    } //end for
  } //end while

  exit(EXIT_SUCCESS);
} //end main
