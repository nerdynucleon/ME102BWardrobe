#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <signal.h>

#include <fcntl.h>   /* File control definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <pthread.h> /* POSIX threading library*/

#define DROPLET_IP "138.68.40.50"
#define DROPLET_PORT 80

// Global FD for Arduino
int arduinofd = STDOUT_FILENO;
int arduinofd_in = STDIN_FILENO;

/* 'open_port()' - Open serial port to Arduino.
 *
 * Returns the file descriptor on success or -1 on error.
 */
int open_arduino_port(void)
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

enum class arduino_cmd{
  invalid = -1,
  x = 0,
  y_up = 1,
  y_down = 2,
  z_extend = 3,
  z_retract = 4
};

arduino_cmd parse_server(char * cmd_server)
{
  printf("jetson recv:<%s>\n",cmd_server);
  if(strcmp("get", cmd_server) == 0)
    return arduino_cmd::x;
  return arduino_cmd::invalid;
}

void execute_arduino_cmd(arduino_cmd a_cmd, int arg)
{
  char read_buff[3] = {0};
  char cmd[6];
  switch(a_cmd)
  {
    case arduino_cmd::x :
      if (arg < 10000 && arg >= 0)
      {
        sprintf(cmd, "x%04d\n", arg); 
        printf("Writing to Arduino:%s\n", cmd);
        write(arduinofd, cmd, 6);
        if(read(arduinofd_in, read_buff, 2) > 0)
          printf("Received Arduino:%s\n", read_buff); 
      } else {
        printf("invalid number of revolutions\n");
      }
      break;
    case arduino_cmd::y_up :
      sprintf(cmd, "yU\n"); 
      printf("Writing to Arduino:%s\n", cmd);
      write(arduinofd, cmd, 3);
      if(read(arduinofd_in, read_buff, 2) > 0)
        printf("Received Arduino:%s\n", read_buff); 
      break;
    case arduino_cmd::y_down :
      sprintf(cmd, "yD\n"); 
      printf("Writing to Arduino:%s\n", cmd);
      write(arduinofd, cmd, 3);
      if(read(arduinofd_in, read_buff, 2) > 0)
        printf("Received Arduino:%s\n", read_buff); 
      break;
    case arduino_cmd::z_extend :
      sprintf(cmd, "zE\n"); 
      printf("Writing to Arduino:%s\n", cmd);
      write(arduinofd, cmd, 3);
      if(read(arduinofd_in, read_buff, 2) > 0)
        printf("Received Arduino:%s\n", read_buff); 
      break;
    case arduino_cmd::z_retract :
      sprintf(cmd, "zR\n"); 
      printf("Writing to Arduino:%s\n", cmd);
      write(arduinofd, cmd, 3);
      if(read(arduinofd_in, read_buff, 2) > 0)
        printf("Received Arduino:%s\n", read_buff); 
      break;
  }
  
  fsync(arduinofd);
}

void *jetson_to_server(void * fd_ptr)
{
  int fd = *((int *) fd_ptr);
  for(;;)
  {
    int c_send = getchar();
    if(c_send != -1)
    {
      if(write(fd, &c_send, 1) == -1)
      {
        printf("write error\n");
        pthread_exit( NULL );
      }
    }
  }
}

void *server_to_jetson(void * fd_ptr)
{
  int fd = *((int *) fd_ptr);
  for(;;)
  {
    char c_recv[1024];
    memset(c_recv, 0, sizeof(c_recv));
    int res = read(fd, &c_recv, sizeof(c_recv));
    if(res == 0)
    {
      printf("read error\n");
      pthread_exit( NULL );
    }
    if(res > 0)
    {
      arduino_cmd cmd = parse_server(c_recv);
      //execute_arduino_cmd(cmd);
    }
  }
}

int main(int argc, char *argv[])
{
    int sockfd = 0;

    // Establish Serial Connection with Arduino
    //*********arduinofd = open_arduino_port();
    if(arduinofd != -1)
    {
	printf("Successfully connected to Arduino\n");
    } else {
        printf("\n Error : Failed to connect to Arduino \n");
        return -1;
    }
    execute_arduino_cmd(arduino_cmd::x, -1);
    execute_arduino_cmd(arduino_cmd::x, 10000);
    execute_arduino_cmd(arduino_cmd::x, 0);
    execute_arduino_cmd(arduino_cmd::x, 2);
    execute_arduino_cmd(arduino_cmd::x, 5);
    execute_arduino_cmd(arduino_cmd::y_up, -1);
    execute_arduino_cmd(arduino_cmd::y_up, -1);
    execute_arduino_cmd(arduino_cmd::y_down, -1);
    execute_arduino_cmd(arduino_cmd::y_down, -1);
    execute_arduino_cmd(arduino_cmd::z_extend, -1);
    execute_arduino_cmd(arduino_cmd::z_extend, -1);
    execute_arduino_cmd(arduino_cmd::z_retract, -1);
    execute_arduino_cmd(arduino_cmd::z_retract, -1);

/*
    struct sockaddr_in serv_addr; 
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(DROPLET_PORT); 
    inet_aton(DROPLET_IP, &serv_addr.sin_addr);

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } else {
	printf("Successfully connected to Droplet\n");
    } 
    signal(SIGPIPE, SIG_IGN);

    // Create Threads for Sending and Receiving
    pthread_t send_thread, recv_thread;
    pthread_create(&send_thread, NULL, jetson_to_server, &sockfd);
    pthread_create(&recv_thread, NULL, server_to_jetson, &sockfd);
    
    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);
    
    printf("\n Closing Connection \n");
    close(sockfd);
 
    pthread_exit(NULL);
    return 0;
*/
}
