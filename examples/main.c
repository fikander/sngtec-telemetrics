#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <string.h>

#include <sys/time.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1         //POSIX compliant source
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;

void signal_handler_IO (int status);    //definition of signal handler
int wait_flag=TRUE;                     //TRUE while no signal received
char devicename[80];
long Baud_Rate = 38400;         // default Baud Rate (110 through 38400)
long BAUD;                      // derived baud rate from command line
long DATABITS;
long STOPBITS;
long PARITYON;
long PARITY;
int Data_Bits = 8;              // Number of data bits
int Stop_Bits = 1;              // Number of stop bits
int Parity = 0;                 // Parity as follows:
// 00 = NONE, 01 = Odd, 02 = Even, 03 = Mark, 04 = Space
int Format = 4;
FILE *input;
FILE *output;
int status;

struct timeval lastTime;
unsigned char sendData[64];
int sendDataSize = 0;

void parseHexData(char *hexString) 
{
    int readBytes = 1;

    char *ptr = hexString;
    uint value;

    sendDataSize = 0;
    while ( (readBytes > 0) ) {
        readBytes = sscanf(ptr, "\\x%2x", &value);
        if (readBytes > 0) {
            sendData[sendDataSize] = (unsigned char)value;
            ++sendDataSize;
        }
        ptr += 4;
    }

    printf("Will send ");
    int i = 0;
    for (i = 0; i < sendDataSize; ++i) {
        printf("0x%x ",sendData[i]);
    }
    printf("\r\n");
}

int main(int argc, char *argv[])
{
    char version[80] = "       POSIX compliant Communications test program version 1.00 4-25-1999\r\n";
    char version1[80] = "          Copyright(C) Mark Zehner/Peter Baumann 1999\r\n";
    char version2[80] = " This code is based on a DOS based test program by Mark Zehner and a Serial\r\n";
    char version3[80] = " Programming POSIX howto by Peter Baumann, integrated by Mark Zehner\r\n";
    char version4[80] = " This program allows you to send characters out the specified port by typing\r\n";
    char version5[80] = " on the keyboard.  Characters typed will be echoed to the console, and \r\n";
    char version6[80] = " characters received will be echoed to the console.\r\n";
    char version7[80] = " The setup parameters for the device name, receive data format, baud rate\r\n";
    char version8[80] = " and other serial port parameters must be entered on the command line \r\n";
    char version9[80] = " To see how to do this, just type the name of this program. \r\n";
    char version10[80] = " This program is free software; you can redistribute it and/or modify it\r\n";
    char version11[80] = " under the terms of the GNU General Public License as published by the \r\n";
    char version12[80] = " Free Software Foundation, version 2.\r\n";
    char version13[80] = " This program comes with ABSOLUTELY NO WARRANTY.\r\n";
    char instr[100] ="\r\nOn the command you must include six items in the following order, they are:\r\n";
    char instr1[80] ="   1.  The device name      Ex: ttyS0 for com1, ttyS1 for com2, etc\r\n";
    char instr2[80] ="   2.  Baud Rate            Ex: 38400 \r\n";
    char instr3[80] ="   3.  Number of Data Bits  Ex: 8 \r\n";
    char instr4[80] ="   4.  Number of Stop Bits  Ex: 0 or 1\r\n";
    char instr5[80] ="   5.  Parity               Ex: 0=none, 1=odd, 2=even\r\n";
    char instr6[80] ="   6.  Format of data received:  1=hex, 2=dec, 3=hex/asc, 4=dec/asc, 5=asc\r\n";
    char instr7[80] ="   7.  (optional) Hexadecimal data to be sent, e.g. \"\\x01\\x23\r\n\"";
    char instr8[80] =" Example command line:  com ttyS0 38400 8 0 0 4 [\"\\x12\\x34\\x56\\x78\"]\r\n";
    char Param_strings[7][80];
    char message[90];



    int fd, tty, res, i, error;
    char In1, Key;
    struct termios oldtio, newtio;       //place for old and new port settings for serial port
    struct termios oldkey, newkey;       //place tor old and new port settings for keyboard teletype
    char buf[255];                       //buffer for where data is put

    input = fopen("/dev/tty", "r");      //open the terminal keyboard
    output = fopen("/dev/tty", "w");     //open the terminal screen

    if (!input || !output)
    {
        fprintf(stderr, "Unable to open /dev/tty\n");
        return(1);
    }

    error=0;
    fputs(version,output);               //display the program introduction
    fputs(version1,output);
    fputs(version2,output);
    fputs(version3,output);
    fputs(version4,output);
    fputs(version5,output);
    fputs(version6,output);
    fputs(version7,output);
    fputs(version8,output);
    fputs(version9,output);
    fputs(version10,output);
    fputs(version11,output);
    fputs(version12,output);
    fputs(version13,output);
    //read the parameters from the command line
    int correctNumOfParams = ((argc==7) || (argc==8));
    if ( correctNumOfParams )  //if there are the right number of parameters on the command line
    {
        for (i=1; i<argc; i++)  // for all wild search parameters
        {
            strcpy(Param_strings[i-1],argv[i]);
        }
        i=sscanf(Param_strings[0],"%s",devicename);
        if (i != 1) error=1;
        i=sscanf(Param_strings[1],"%li",&Baud_Rate);
        if (i != 1) error=1;
        i=sscanf(Param_strings[2],"%i",&Data_Bits);
        if (i != 1) error=1;
        i=sscanf(Param_strings[3],"%i",&Stop_Bits);
        if (i != 1) error=1;
        i=sscanf(Param_strings[4],"%i",&Parity);
        if (i != 1) error=1;
        i=sscanf(Param_strings[5],"%i",&Format);
        if (i != 1) error=1;
        sprintf(message,"Device=%s, Baud=%li\r\n",devicename, Baud_Rate); //output the received setup parameters
        fputs(message,output);
        sprintf(message,"Data Bits=%i  Stop Bits=%i  Parity=%i  Format=%i\r\n",Data_Bits, Stop_Bits, Parity, Format);
        fputs(message,output);
    }  //end of if param_count==7

    if ( correctNumOfParams && (error==0))  //if the command line entries were correct
    {                                    //run the program
        tty = open("/dev/tty", O_RDWR | O_NOCTTY | O_NONBLOCK); //set the user console port up
        tcgetattr(tty,&oldkey); // save current port settings   //so commands are interpreted right for this program
        // set new port settings for non-canonical input processing  //must be NOCTTY
        newkey.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
        newkey.c_iflag = IGNPAR;
        newkey.c_oflag = 0;
        newkey.c_lflag = 0;       //ICANON;
        newkey.c_cc[VMIN]=1;
        newkey.c_cc[VTIME]=0;
        tcflush(tty, TCIFLUSH);
        tcsetattr(tty,TCSANOW,&newkey);

        switch (Baud_Rate)
        {
        case 38400:
        default:
            BAUD = B38400;
            break;
        case 19200:
            BAUD  = B19200;
            break;
        case 9600:
            BAUD  = B9600;
            break;
        case 4800:
            BAUD  = B4800;
            break;
        case 2400:
            BAUD  = B2400;
            break;
        case 1800:
            BAUD  = B1800;
            break;
        case 1200:
            BAUD  = B1200;
            break;
        case 600:
            BAUD  = B600;
            break;
        case 300:
            BAUD  = B300;
            break;
        case 200:
            BAUD  = B200;
            break;
        case 150:
            BAUD  = B150;
            break;
        case 134:
            BAUD  = B134;
            break;
        case 110:
            BAUD  = B110;
            break;
        case 75:
            BAUD  = B75;
            break;
        case 50:
            BAUD  = B50;
            break;
        }  //end of switch baud_rate
        switch (Data_Bits)
        {
        case 8:
        default:
            DATABITS = CS8;
            break;
        case 7:
            DATABITS = CS7;
            break;
        case 6:
            DATABITS = CS6;
            break;
        case 5:
            DATABITS = CS5;
            break;
        }  //end of switch data_bits
        switch (Stop_Bits)
        {
        case 1:
        default:
            STOPBITS = 0;
            break;
        case 2:
            STOPBITS = CSTOPB;
            break;
        }  //end of switch stop bits
        switch (Parity)
        {
        case 0:
        default:                       //none
            PARITYON = 0;
            PARITY = 0;
            break;
        case 1:                        //odd
            PARITYON = PARENB;
            PARITY = PARODD;
            break;
        case 2:                        //even
            PARITYON = PARENB;
            PARITY = 0;
            break;
        }  //end of switch parity

        //open the device(com port) to be non-blocking (read will return immediately)
        fd = open(devicename, O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (fd < 0)
        {
            perror(devicename);
            return(-1);
        }

        tcgetattr(fd,&oldtio); // save current port settings
        // set new port settings for canonical input processing
        newtio.c_cflag = BAUD | DATABITS | STOPBITS | PARITYON | PARITY | CLOCAL | CREAD;
        newtio.c_iflag = IGNPAR;
        newtio.c_oflag = 0;
        newtio.c_lflag = 0;       //ICANON;
        newtio.c_cc[VMIN]=1;
        newtio.c_cc[VTIME]=0;
        tcflush(fd, TCIFLUSH);
        tcsetattr(fd,TCSANOW,&newtio);


	//check if we have a command to be sent
        if (8 == argc) {
            parseHexData(argv[7]);
            int sent = write(fd, sendData, sendDataSize);
            sprintf(message, "Sending data of size %d, where %d successfully sent\r\n", sendDataSize, sent);
            fputs(message, output);
        }

        //our descriptors for select are fd and fileno(input)
        fd_set readfs;//read set
        int fd2 = fileno(input);
        int max_fd = ( (fd > fd2) ? fd : fd2 ) + 1;


        // loop while waiting for input. normally we would do something useful here
        while (STOP==FALSE)
        {
            FD_SET(fd, &readfs);
            FD_SET(fd2, &readfs);

            sprintf(message, "Waiting for input on rs485 or stdin...\r\n");
            fputs(message, output);
            select(max_fd, &readfs, NULL, NULL, NULL);

            if (FD_ISSET(fd2, &readfs)) {
                status = fread(&Key,1,1,input);
                if (status==1)  //if a key was hit
                {
                    switch (Key)
                    { /* branch to appropiate key handler */
                    case 0x1b: /* Esc */
                        STOP=TRUE;
                        break;
                    default:
                        //                  fputc((int) Key,output);
                        //                  sprintf(message,"%x ",Key);  //debug
                        //                  fputs(message,output);
                        //                  write(fd,&Key,1);          //write 1 byte to the port
                        break;
                    }  //end of switch key
                }  //end if a key was hit
            }

            // after receiving SIGIO, wait_flag = FALSE, input is available and can be read
            if (FD_ISSET(fd, &readfs))  //if input is available
            {
                res = read(fd,buf,255);

                if (res > 0)
                {
                    for (i=0; i<res; i++)  //for all chars in string
                    {
                        In1 = buf[i];
                        switch (Format)
                        {
                        case 1:         //hex
                            sprintf(message,"%2x ",In1);
                            fputs(message,output);
                            break;
                        case 2:         //decimal
                            sprintf(message,"%d ",In1);
                            fputs(message,output);
                            break;
                        case 3:         //hex and asc
                            if ((In1<32) || (In1>125))
                            {
                                sprintf(message,"%x",In1);
                                fputs(message,output);
                            }
                            else fputc ((int) In1, output);
                            break;
                        case 4:         //decimal and asc
                        default:
                            if ((In1<32) || (In1>125))
                            {
                                sprintf(message,"%d",In1);
                                fputs(message,output);
                            }
                            else fputc ((int) In1, output);
                            break;
                        case 5:         //asc
                            fputc ((int) In1, output);
                            break;
                        }  //end of switch format
                    }  //end of for all chars in string
                    fputs("\r\n",output);
                }  //end if resɘ
                //            buf[res]=0;
                //            printf(":%s:%d\n", buf, res);
                //            if (res==1) STOP=TRUE; /* stop loop if only a CR was input */
                wait_flag = TRUE;      /* wait for new input */
            }  //end if wait flag == FALSE

        }  //while stop==FALSE
        // restore old port settings
        tcsetattr(fd,TCSANOW,&oldtio);
        tcsetattr(tty,TCSANOW,&oldkey);
        close(tty);
        close(fd);        //close the com port
    }  //end if command line entrys were correct
    else  //give instructions on how to use the command line
    {
        fputs(instr,output);
        fputs(instr1,output);
        fputs(instr2,output);
        fputs(instr3,output);
        fputs(instr4,output);
        fputs(instr5,output);
        fputs(instr6,output);
        fputs(instr7,output);
        fputs(instr8,output);
    }
    fclose(input);
    fclose(output);

    return 0;
}  //end of main


