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
#include <termios.h>

#define DROPLET_IP "138.68.40.50"
#define DROPLET_PORT 80

// Global FD for Arduino
int arduinofd = -1;

/*
 * 'open_port()' - Open serial port to connect to arduino.
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
        options.c_cc[VMIN] = 1;
        options.c_cc[VTIME] = 2;
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
    z_retract = 4,
    y_up_block = 5,
    y_down_block = 6,
    z_extend_block = 7,
    z_retract_block = 8
};

enum class jetson_cmd_id
{
    invalid = -1,
    get = 0,
    put = 1,
    id = 2
};

struct jetson_cmd
{
    jetson_cmd_id id;
    int arg;
};

#define PUT_CMD "<put>"
#define GET_CMD "<get>"
#define ID_CMD  "<id>"

jetson_cmd parse_server(char * cmd_server)
{
    printf("recv cmd:<%s>\n",cmd_server);
    jetson_cmd parsed_cmd;
    if(!strncmp(cmd_server, PUT_CMD, strlen(PUT_CMD)))
    {
        parsed_cmd.id = jetson_cmd_id::put;
        parsed_cmd.arg = atoi(cmd_server + strlen(PUT_CMD));
    }
    else if(!strncmp(cmd_server, GET_CMD, strlen(GET_CMD)))
    {
        parsed_cmd.id = jetson_cmd_id::get;
        parsed_cmd.arg = atoi(cmd_server + strlen(GET_CMD));
    }
    else if(!strncmp(cmd_server, ID_CMD, strlen(ID_CMD)))
    {
        parsed_cmd.id = jetson_cmd_id::id;
    }
    else
    {
        parsed_cmd.id = jetson_cmd_id::invalid;
    }
    return parsed_cmd;
}

void execute_arduino_cmd(arduino_cmd a_cmd, int arg = 0)
{
    char read_buff[3] = {0};
    char cmd[6];
    switch(a_cmd)
    {
        case arduino_cmd::x :
            if (arg < 1000 && arg > -1000)
            {
                if (arg < 0)
                    sprintf(cmd, "x-%03d\n", -arg); 
                else 
                    sprintf(cmd, "x+%03d\n", arg); 
                printf("Writing to Arduino:%s", cmd);
                write(arduinofd, cmd, 6);
                
                printf("Waiting for arduino response...\n"); 
                read(arduinofd, read_buff, 2);
                printf("Received Arduino:%s", read_buff); 
            } else {
                printf("invalid number of revolutions\n");
            }
            break;
        case arduino_cmd::y_up :
            sprintf(cmd, "yU\n"); 
            printf("Writing to Arduino:%s", cmd);
            write(arduinofd, cmd, 3);
            
            printf("Waiting for arduino response...\n"); 
            read(arduinofd, read_buff, 2);
            printf("Received Arduino:%s", read_buff); 
            break;
        case arduino_cmd::y_down :
            sprintf(cmd, "yD\n"); 
            printf("Writing to Arduino:%s", cmd);
            write(arduinofd, cmd, 3);
            
            printf("Waiting for arduino response...\n"); 
            read(arduinofd, read_buff, 2);
            printf("Received Arduino:%s", read_buff); 
            break;
        case arduino_cmd::z_extend :
            sprintf(cmd, "zE\n"); 
            printf("Writing to Arduino:%s", cmd);
            write(arduinofd, cmd, 3);
            
            printf("Waiting for arduino response...\n"); 
            read(arduinofd, read_buff, 2);
            printf("Received Arduino:%s", read_buff); 
            break;
        case arduino_cmd::z_retract :
            sprintf(cmd, "zR\n"); 
            printf("Writing to Arduino:%s", cmd);
            write(arduinofd, cmd, 3);
            
            printf("Waiting for arduino response...\n"); 
            read(arduinofd, read_buff, 2);
            printf("Received Arduino:%s", read_buff); 
            break;
        case arduino_cmd::y_up_block :
            sprintf(cmd, "yUB\n"); 
            printf("Writing to Arduino:%s", cmd);
            write(arduinofd, cmd, 4);
            
            printf("Waiting for arduino response...\n"); 
            read(arduinofd, read_buff, 2);
            printf("Received Arduino:%s", read_buff); 
            break;
        case arduino_cmd::y_down_block :
            sprintf(cmd, "yDB\n"); 
            printf("Writing to Arduino:%s", cmd);
            write(arduinofd, cmd, 4);
            
            printf("Waiting for arduino response...\n"); 
            read(arduinofd, read_buff, 2);
            printf("Received Arduino:%s", read_buff); 
            break;
        case arduino_cmd::z_extend_block :
            sprintf(cmd, "zEB\n"); 
            printf("Writing to Arduino:%s", cmd);
            write(arduinofd, cmd, 4);
            
            printf("Waiting for arduino response...\n"); 
            read(arduinofd, read_buff, 2);
            printf("Received Arduino:%s", read_buff); 
            break;
        case arduino_cmd::z_retract_block :
            sprintf(cmd, "zRB\n"); 
            printf("Writing to Arduino:%s", cmd);
            write(arduinofd, cmd, 4);
            
            printf("Waiting for arduino response...\n"); 
            read(arduinofd, read_buff, 2);
            printf("Received Arduino:%s", read_buff); 
            break;
    }

    fsync(arduinofd);
}

int calculate_x_movement(int current_location, int desired_pos)
{
    int movement = 0;

    // -1 location not exactly offset from first notch
    if (current_location == -1 )
    {
        movement += 45; // 1 turn
        current_location = 0; 
    }
    if (desired_pos == -1 )
    {
        movement -= 45; // 1 turn
        current_location -= 1;
    }
 
    movement += 51 * (desired_pos - current_location); // 1.25 turns between notches
    
    return movement;
}

void execute_jetson_cmd(jetson_cmd cmd)
{
    static int current_location = -1;
    printf("Executing Following CMD:\n");
    if(cmd.id == jetson_cmd_id::invalid)
    {
        printf("Invalid CMD\n");
    }
    else if(cmd.id == jetson_cmd_id::get and cmd.arg >= 0 and cmd.arg <= 9)
    {
        printf("GET CMD%d\n", cmd.arg);
        execute_arduino_cmd(arduino_cmd::x, calculate_x_movement(current_location, cmd.arg));
        current_location = cmd.arg;
       
        execute_arduino_cmd(arduino_cmd::z_retract_block);
        execute_arduino_cmd(arduino_cmd::y_up_block);
        execute_arduino_cmd(arduino_cmd::z_extend);
        sleep(2);
        execute_arduino_cmd(arduino_cmd::y_down);
        execute_arduino_cmd(arduino_cmd::z_extend_block);
        
        execute_arduino_cmd(arduino_cmd::x, calculate_x_movement(current_location, -1));
        current_location = -1;
    }
    else if(cmd.id == jetson_cmd_id::put)
    {
        printf("PUT CMD%d\n", cmd.arg);
        
        // Move to storage spot
        execute_arduino_cmd(arduino_cmd::y_up);
        execute_arduino_cmd(arduino_cmd::x, calculate_x_movement(current_location, cmd.arg));
        current_location = cmd.arg;

        // Place Article
        execute_arduino_cmd(arduino_cmd::z_retract_block);
        execute_arduino_cmd(arduino_cmd::y_down_block);
      
        // return to original position 
        execute_arduino_cmd(arduino_cmd::z_extend_block); 
        execute_arduino_cmd(arduino_cmd::x, calculate_x_movement(current_location, -1));
        current_location = -1;
    }
    else if(cmd.id == jetson_cmd_id::id)
    {
        printf("ID CMD\n");
    }
    
}

int main(int argc, char *argv[])
{
    int sockfd = 0;

    // Establish Serial Connection with Arduino
    arduinofd = open_arduino_port();
    printf("Establishing Connection to Arduino...\n");
    sleep(2);
    if(arduinofd != -1)
    {
        printf("Successfully connected to Arduino\n");
    } else {
        printf("\n Error : Failed to connect to Arduino \n");
        return -1;
    }

/*    
    jetson_cmd cmd;
    // Test sequence of Position Commands
    cmd.arg = 0;
    cmd.id = jetson_cmd_id::put;
    execute_jetson_cmd(cmd);
    sleep(2);

    cmd.id = jetson_cmd_id::get;
    execute_jetson_cmd(cmd);
    sleep(2);
*/    
    
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
        printf("\n Error : Failed to connect to Droplet \n");
        return 1;
    } else {
        printf("Successfully connected to Droplet\n");
    } 
    signal(SIGPIPE, SIG_IGN);

    // Wait for lambda function requests 
    for(;;)
    {
        char c_recv[1024];
        memset(c_recv, 0, sizeof(c_recv));
        int res = read(sockfd, &c_recv, sizeof(c_recv));
        if(res == 0)
        {
            printf("READ ERROR: connection to droplet most likely died\n");
            return -1;
        }
        if(res > 0)
        {
            jetson_cmd cmd = parse_server(c_recv);
            execute_jetson_cmd(cmd);
            write(sockfd, "<done>\n", strlen("<done>\n"));
        }
    }
    
    printf("\n Closing Connection \n");
    close(sockfd);

    return 0;
}

