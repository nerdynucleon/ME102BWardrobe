#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <pthread.h> /* POSIX threading library*/

/*
 * 'open_port()' - Open serial port 1.
 *
 * Returns the file descriptor on success or -1 on error.
 */
int open_port(void)
{
  int fd; /* File descriptor for the port */

  fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1)
  {
    // Could not open the port.
    perror("open_port: Unable to open /dev/ttyACM0 - ");
  }
  else
  {
    fcntl(fd, F_SETFL, 0);
    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    tcsetattr(fd, TCSANOW, &options);
  }
  return (fd);
}

void *user_to_device(void * fd_ptr)
{
  int fd = *((int *) fd_ptr);
  for(;;)
  {
    int c_send = getchar();
    if(c_send != -1)
      write(fd, &c_send, 1);
  }
}

void *device_to_user(void * fd_ptr)
{
  int fd = *((int *) fd_ptr);
  for(;;)
  {
    char c_recv;
    int res = read(fd, &c_recv, 1);
    if(res > 0)
    {
      putchar(c_recv);
      fflush(stdout);
    }
  }
}

int main(int argc, char ** argv)
{
  // Turn off Echo for STDIN
  struct termios term_prop;
  tcgetattr(STDIN_FILENO, &term_prop);
  term_prop.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &term_prop);
  
  int fd = open_port();
  sleep(2);
  if (fd != -1) 
    printf("open port success!\n");
  pthread_t send_thread, recv_thread;
  pthread_create(&send_thread, NULL, user_to_device, &fd);
  pthread_create(&recv_thread, NULL, device_to_user, &fd);

  pthread_join(send_thread, NULL);
  pthread_join(recv_thread, NULL);
}
