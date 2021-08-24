//Adesh Sharma
//Roll no :20CS60R44

#include    <sys/types.h>
#include    <sys/socket.h>
#include    <arpa/inet.h>
#include    <unistd.h>
#include    <netinet/in.h>
#include    <fcntl.h>
#include    <pthread.h>
#include    <sys/select.h>
#include    <sys/time.h>
#include <bits/stdc++.h> 
#include <netdb.h>

#define MAXLINE 4096
using namespace std;

int Error =0;           //Error,indicates any error in the command
void error(char *msg)       //To output error message
{
    perror(msg);
    exit(0);
}
string substring(string s,int ind,int len)  //Replacement of substr() of c++ stl
{
    string str ="";
    for(int i =ind;i<ind+len && i<s.length();i++)
    {
        str += s[i];
    }
    return str;
}

int main(int argc, char **argv)
{
    int controlfd;
    struct sockaddr_in servaddr;
    

    struct hostent *server;int n;char buffer[MAXLINE];
    
    bzero(buffer,MAXLINE);
   
    int file_size,num_line,byte_remain;
    
    if (argc != 3){         //command shud contain ip address and port no

        fprintf(stderr,"usage %s hostname port", argv[0]);
        exit(0);
    }

    int portno = atoi(argv[2]);
    
    controlfd = socket(AF_INET, SOCK_STREAM, 0);

    if (controlfd < 0)
        error((char*)"error opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr,"error, no such host");
        exit(0);
    }
    
    bzero((char *) &servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;

    bcopy((char *)server->h_addr,(char *)&servaddr.sin_addr.s_addr,server->h_length);//serv_addr.sin_addr.s_addr = INADDR_ANY;
    
    servaddr.sin_port = htons(portno);
    
    
    if (connect(controlfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)   // connection setup
        error((char*)"ERROR connecting");

    bzero(buffer,MAXLINE);
    n = recv(controlfd,buffer,MAXLINE,0);   //receiving status about connection

    if (n < 0)
        error((char*)"ERROR reading from socket");

    cout<<buffer;cout<<endl;
    
    while(1){
        
        printf("Please enter the message to the server: ");
        bzero(buffer,MAXLINE);
        fgets(buffer,MAXLINE,stdin);
        
        string command = buffer;
        if(command == "\n"){

            cout<<"Invalid command"<<endl;
            continue;
        }

        n = send(controlfd,buffer,MAXLINE,0);//sending command to server

        if (n < 0)
            error((char*)"ERROR writing to socket");

        bzero(buffer,MAXLINE);
        n = recv(controlfd,buffer,MAXLINE,0);   //to know whether command is valid
        if(n == 0){
            cout<<"Server got disconnected\n";
            exit(1);
        }
            

        if(strcmp("1" , buffer) == 0){  //valid comamnd
            bzero(buffer,MAXLINE);
            n = recv(controlfd,buffer,MAXLINE,0);
            command = (string)buffer;

        }
        else{
           cout<<"Invalid command"<<endl;
           continue;
        }
        bzero(buffer,MAXLINE);
        if(command == "codejud"){

            n = recv(controlfd,buffer,MAXLINE,0);//extension is valid or not
            if(strcmp("0" , buffer) == 0){

                cout<<"Invalid extension"<<endl;
                continue;
            }

        }
        bzero(buffer,MAXLINE);
        // -----------------------------Identify and execute command ------------------------------------
        
        if(command == "stor"){
            
            
            char message[MAXLINE];
            char data_buffer[MAXLINE],num_lines[MAXLINE],remain_bytes[MAXLINE];
            FILE *fp;

            bzero(message,MAXLINE);
            char fn[MAXLINE];
            n = recv(controlfd,fn,MAXLINE,0);   //Getting the filename

            if ((fp = fopen(fn,"r"))!=NULL)
            {
                //size of file
                
                n = send(controlfd,"1",MAXLINE,0);   //writing 1 indicates that subsequently we are going to send some file data
                if (n < 0)
                    error((char*)"ERROR writing to socket");

                n = recv(controlfd,message,MAXLINE,0);//To check if file at server end opened successfully
                if (n < 0)
                    error((char*)"ERROR reading from socket");

                if(strcmp("1" , message) == 0){

                    fseek (fp , 0 , SEEK_END);

                    file_size = ftell (fp);
                    rewind (fp);

                    num_line = file_size / MAXLINE;
                    byte_remain = file_size % MAXLINE;  //If some file data left
                    sprintf(num_lines,"%d",num_line);

                    n = send(controlfd, num_lines, MAXLINE,0);
                    if (n < 0)
                        error((char*)"ERROR writing to socket");

                    for(int i= 0; i < num_line; i++) { 

                        fread (data_buffer,sizeof(char),MAXLINE,fp);
                        n = send(controlfd, data_buffer, MAXLINE,0);
                        if (n < 0)
                            error((char*)"ERROR writing to socket");
                        
                    }
                    sprintf(remain_bytes,"%d",byte_remain); //remaining bytes or data
                    n = send(controlfd, remain_bytes, MAXLINE,0);
                    if (n < 0)
                        error((char*)"ERROR writing to socket");

                    if (byte_remain > 0) { 

                        fread (data_buffer,sizeof(char),byte_remain,fp);
                        n = send(controlfd, data_buffer, MAXLINE,0);
                        if (n < 0)
                            error((char*)"ERROR writing to socket");
                    }
                    fclose(fp);
                    cout<<"Sent file "<<fn<<endl;
                }
                else{
                    cout<<"file opening error at server\n";
                }
            }
            else{
                n = send(controlfd,"0",MAXLINE,0);       //File not exist,file opening error
                if (n < 0)
                    error((char*)"ERROR writing to socket");
                cout<<"Error in opening file. Check filename\n";
            }
            
            continue;
        }
            
        else{
            if(command == "retr"){
                
            
                char message[MAXLINE];

                char data_buffer[MAXLINE],num_lines[MAXLINE],remain_bytes[MAXLINE];

                FILE *fp;
                bzero(message,MAXLINE);
                char fn[MAXLINE];
                n = recv(controlfd,fn,MAXLINE,0);

                bzero(message,MAXLINE);
                n = recv(controlfd,message,MAXLINE,0);
                if (n < 0)
                    error((char*)"ERROR reading from socket");
                if(strcmp("1",message)==0){ // 1 indicates that subsequently server going to send some file data

                    if((fp = fopen(fn,"w"))==NULL){
                        cout<<"Error in creating file\n";
                        n = send(controlfd,"0",MAXLINE,0);
                        if (n < 0)
                            error((char*)"ERROR writing to socket");
                    }

                    else
                    {
                        n = send(controlfd, "1",MAXLINE, 0);
                        if (n < 0)
                            error((char*)"ERROR writing to socket");

                        n = recv(controlfd, num_lines, MAXLINE,0);     //number of lines in file
                        if (n < 0)
                            error((char*)"ERROR reading from socket");

                        num_line = atoi(num_lines);

                        for(int i= 0; i < num_line; i++) { 

                            n = recv(controlfd, data_buffer, MAXLINE,0);
                            if (n < 0)
                                error((char*)"ERROR reading from socket");
                            fwrite(data_buffer,sizeof(char),MAXLINE,fp);
                        
                        }

                        n = recv(controlfd, remain_bytes, MAXLINE,0);
                        if (n < 0)
                            error((char*)"ERROR reading from socket");
                        byte_remain = atoi(remain_bytes);

                        if (byte_remain > 0) { 

                            n = recv(controlfd, data_buffer, MAXLINE,0);
                            if (n < 0)
                                error((char*)"ERROR reading from socket");
                            fwrite(data_buffer,sizeof(char),byte_remain,fp);
                            
                        }
                        fclose(fp);
                        cout<<"Received file "<<fn<<endl;
                    }
                }
                else{
                    cout<<"Error in opening file.Please check whether file exist or not at server\n";
                   
                }
                continue;
                 
            }
            else{
                if(command == "list"){
                    
                    char data_buffer[MAXLINE];
                    
                    char is_over[MAXLINE]= "1";
                    
                    while(strcmp("1",is_over)==0){              //to indicate that more blocks are coming
                        
                        n = recv(controlfd,is_over,MAXLINE,0);
                        if (n < 0)
                            error((char*)"ERROR reading from socket");
                       
                        if(strcmp("0",is_over)==0)          //no more blocks of data
                            break;

                        n = recv(controlfd, data_buffer, MAXLINE,0);
                        if (n < 0)
                            error((char*)"ERROR reading from socket");
                        cout<<data_buffer;
                    }
                    cout<<endl;
                    continue;
                }
                else{
                    
                        if( command == "quit"){
                            
                            close(controlfd);
                            cout<<"Disconnected from server"<<endl;
                            exit(1);
                        }
                        else{
                            if(command == "dele"){

                                char temp[MAXLINE];
                                n = recv(controlfd, temp, MAXLINE,0);//File deletion status
                                if (n < 0)
                                    error((char*)"ERROR reading from socket");
                                cout<<temp<<endl;
                                continue;
                            }
                            else
                                if(command == "codejud"){

                                    char message[MAXLINE];
                                    char data_buffer[MAXLINE],num_lines[MAXLINE],remain_bytes[MAXLINE];
                                    FILE *fp;

                                    bzero(message,MAXLINE);
                                    char fn[MAXLINE];   //receiving filename
                                    n = recv(controlfd,fn,MAXLINE,0);

                                    if ((fp = fopen(fn,"r"))!=NULL)
                                    {
                                        //size of file
                                        
                                        n = send(controlfd,"1",MAXLINE,0);   //writing 1 indicates that subsequently we are going to send some file data
                                        if (n < 0)
                                            error((char*)"ERROR writing to socket");

                                        n = recv(controlfd,message,MAXLINE,0);//To check if file at server end opened successfully
                                        if (n < 0)
                                            error((char*)"ERROR reading from socket");

                                        if(strcmp("1" , message) == 0){

                                            fseek (fp , 0 , SEEK_END);

                                            file_size = ftell (fp);
                                            rewind (fp);

                                            num_line = file_size / MAXLINE;
                                            byte_remain = file_size % MAXLINE;  //If some file data left
                                            sprintf(num_lines,"%d",num_line);

                                            n = send(controlfd, num_lines, MAXLINE,0);
                                            if (n < 0)
                                                error((char*)"ERROR writing to socket");

                                            for(int i= 0; i < num_line; i++) { 

                                                fread (data_buffer,sizeof(char),MAXLINE,fp);
                                                n = send(controlfd, data_buffer, MAXLINE,0);
                                                if (n < 0)
                                                    error((char*)"ERROR writing to socket");
                                                
                                            }
                                            sprintf(remain_bytes,"%d",byte_remain);
                                            n = send(controlfd, remain_bytes, MAXLINE,0);
                                            if (n < 0)
                                                error((char*)"ERROR writing to socket");

                                            if (byte_remain > 0) { 

                                                fread (data_buffer,sizeof(char),byte_remain,fp);
                                                n = send(controlfd, data_buffer, MAXLINE,0);
                                                if (n < 0)
                                                    error((char*)"ERROR writing to socket");
                                            }
                                            fclose(fp);
                                            cout<<"Sent file "<<fn<<endl;
                                            bzero(message,MAXLINE);
                                            recv(controlfd,message,MAXLINE,0);//compilation status
                                            cout<<message<<endl;

                                            if(strcmp("Problem occured at server side", message) == 0)
                                                continue;
                                            if(strcmp("COMPILE_SUCCESS" , message) == 0) {

                                                recv(controlfd,message,MAXLINE,0);
                                                cout<<message<<endl;

                                                if(strcmp("RUN_SUCCESS" , message) == 0){

                                                    recv(controlfd,message,MAXLINE,0);//Matching status
                                                    cout<<message<<endl;

                                                }

                                            }                                           
                                        }
                                        else{
                                            cout<<"file opening error at server\n";
                                        }
                                    }
                                    else{
                                        n = send(controlfd,"0",MAXLINE,0);       //File not exist,file opening error
                                        if (n < 0)
                                            error((char*)"ERROR writing to socket");
                                        cout<<"Error in opening file. Check filename\n";
                                    }
                                    
                                    continue;
                                }
                                

                        }
                    
                }

            }
        }
    //---------------------------------Done ------------------------------------------------------------
       
    }
    close(controlfd);
    return 0;
}