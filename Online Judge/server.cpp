//Adesh Sharma
//Roll no :20CS60R44

#include <bits/stdc++.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fstream>
#include <unistd.h>
#include <errno.h>  
#include <arpa/inet.h>    
#include <sys/socket.h>  
#include <sys/time.h> 
using namespace std;

#define MAXLINE 4096
using namespace std;


struct command_info     //struct variable to store info about command
{
    string filename;
    string cmd_name;
    bool isvalid;
    bool isfile;

};
void error(char *msg)       //To output error message
{
    perror(msg);
    exit(0);
}

string remove_space(string expr)        //removes space and \n from string
{
    int l =0,r = expr.length()-1;string filtr = "";
    for(;l<expr.length() && (expr[l] ==' ' || expr[l] == '\n' || expr[l] == '\t') ; l++);//left strip

    if( l < expr.length()){
        
        for(;r>=0 && (expr[r] ==' ' || expr[r] == '\n' || expr[r] == '\t') ; r--);//right strip

        for(int i = l;i <=r ;i++)
            filtr += expr[i];
    }
    
    return filtr;
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
command_info get_command(string input , int id =0)  //To know the command name,filename,validity of command
{
    struct command_info c_info;string comm;
    if(input.length() > 8){

        comm = substring(input , 0,7);
        for(int i = 0;i<7; i++)
            comm[i] = tolower(input[i]);

        if(comm == "codejud" ){

            if(input[7] != ' '){
                c_info.isvalid = 0;
                return c_info;
            }
            else{
                c_info.isvalid = 1;
                c_info.cmd_name  = comm;
                c_info.isfile =1;    
                c_info.filename =  to_string(id) + substring(input , 8 , input.length() - 8);
                return c_info;
            }
        }
    }
    
    comm = substring(input , 0,4);

    for(int i = 0;i< 4; i++)
        comm[i] = tolower(input[i]);

    if(input.length() > 5 && (comm =="stor" ||comm =="retr"||comm =="dele")){

        if(input[4] != ' '){
            c_info.isvalid = 0;
            return c_info;
        }
        else{
            c_info.isvalid = 1;
            c_info.cmd_name  = comm; 
            c_info.isfile =1;    
            c_info.filename =  substring(input , 5 , input.length() - 5);
        }
    }
    else{
        if(input.length() == 4&& (comm == "list" ||comm == "quit")){
            
            c_info.isvalid = 1;
            c_info.cmd_name  = comm; 
            c_info.isfile = 0;
        }
        else
            c_info.isvalid = 0;
    }
    return c_info;
}
bool is_valid_extension(string fn)  //To know whether the file extension is valid(.c , .cpp) or not
{
    int i =fn.length()-1;
    for(;i >= 0;i--)
        if(fn[i] == '.')
            break;
    
    if(i < 0 || i == (fn.length() - 1))
        return 0;
    
    i += 1;
    string ext = substring(fn , i , fn.length() -i);    //extension
    if(ext == "c" || ext == "c++" || ext == "cpp")
        return 1;
    
    return 0;
}

bool compareFiles(FILE *file1, FILE *file2){        //comapare two files char by char

   char ch1 = getc(file1);
   char ch2 = getc(file2);
   
    while (ch1 != EOF && ch2 != EOF){
      
        while(ch1 == '\r' ) //To avoid overwriting
            ch1 = getc(file1);

        while(ch2 == '\r')
            ch2 = getc(file2);

        if (ch1 != ch2){
            return 0;
        }
        ch1 = getc(file1);
        ch2 = getc(file2);
    }
    while(ch1 == '\n')  //If any blankline at teh end
    {
        ch1 = getc(file1);
    }
    while(ch2 == '\n')
    {
        ch2 = getc(file2);
    }
    if (ch1 == EOF && ch2 == EOF)
        return 1;
    else
       return 0;
}
void delete_file(string file1,string file2) //To delete file
{
    remove(file1.c_str());
    remove(file2.c_str());
    
}
int run(string file_name , int id)  //To compile and run the program
{
    string compile_command;int error_no;
    int i =file_name.length()-1;

    for(;i >= 0;i--)
        if(file_name[i] == '.')
            break;
    i++;
    string ext = substring(file_name , i , file_name.length() -i);  //Extension
    string fn = substring(file_name , 0 , i-1);

    if(ext == "c")
        compile_command = "gcc";
    else
        compile_command = "g++";

    compile_command += " -o a.out " +to_string(id) + file_name;
    

    if(system(compile_command.c_str()) != 0){   //Compilation

        remove((to_string(id) + file_name).c_str());    //deleting the sent cpp file
        return 1;
    }
    
    string input_file = "input_" + fn +".txt";
    string output_file = "output_"  + to_string(id) +fn +".txt";
    string input_temp_file = "input_temp_"  + to_string(id) +fn +".txt";//Temp file to store one testcase at a time

    ifstream fin;fin.open(input_file.c_str());
    string testcase;ofstream fout;

    if(fin){    //Whether the program requires an input
        while(getline(fin , testcase )){

            fout.open(input_temp_file , ios::out) ;
            fout << testcase;
            fout.close();

            string run_command = "timeout 1 ./a.out < " + input_temp_file + " >> " + output_file;

            if((error_no =system(run_command.c_str()) )!=0){    //Running sent cpp program
                
                delete_file((to_string(id) +file_name)  , output_file);//deleting the sent cpp file,created input temp file and output file
                remove(input_temp_file.c_str());

                if(error_no == 31744 || error_no == 36608)   //TLE
                    return 3;
                return 2;       //Run time error
            }
        }
        fin.close();remove(input_temp_file.c_str());
    }
    else{//when no input requires
        string run_command = "timeout 1 ./a.out >> " + output_file;

        if((error_no =system(run_command.c_str()) )!=0){    //Running sent cpp program
                
            delete_file((to_string(id) +file_name)  , output_file);//deleting the sent cpp file, output file
            if(error_no == 31744 || error_no == 36608)
                return 3;

            return 2;
        }

    }
   
    string testcase_file = "testcase_" + fn + ".txt";
    FILE *file1 = fopen(output_file.c_str(), "r");
    FILE *file2 = fopen(testcase_file.c_str(), "r");

    if(file1 ==NULL){
    
        cout<<"Output file creation unsucessful"<<endl;return -1;
    }
    if (file2 == NULL){

        delete_file((to_string(id) +file_name)  , output_file);//deleting the sent cpp file,created input temp file and output file
        printf("Error : Testcase file not open\n");
        return -1;
    }
    bool res = compareFiles(file1, file2);
    if(file1)
        fclose(file1);
    if(file2)
        fclose(file2);

    if(file1 && file2)
        delete_file(to_string(id) +file_name  , output_file);//deleting the sent cpp file,created input temp file and output file

    if(res) //ACCPETED
        return 4;
    else
        return 5;   //some testcases failed
}
int main(int argc, char **argv)
{
    if (argc != 2){         

        cout<<"Please provide port number";
        exit(0);
    }
    
    int opt = 1;int portno;char send_buffer[MAXLINE];char recv_buffer[MAXLINE];
    int master_socket , addrlen , controlfd , client_socket[30] , max_clients = 30 , activity, i , valread , sd;
    
    int max_sd;   int n;
    struct sockaddr_in address;   
         
    char buffer[MAXLINE];

    bzero(buffer,MAXLINE);
    
    int file_size,num_line,byte_remain;  
         
    //set of socket descriptors  
    fd_set readfds;   
      
      
    portno =atoi(argv[1]);
    
    //initialise all client_socket[] to 0 so not checked  
    for (i = 0; i < max_clients; i++)   
    {   
        client_socket[i] = 0;   
    }   
         
    //create a master socket  
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)   
    {   
        perror("socket failed");   
        exit(EXIT_FAILURE);   
    }   
     
    //set master socket to allow multiple connections ,  
    //this is just a good habit, it will work without this  
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,sizeof(opt)) < 0 )  
           
    {   
        perror("setsockopt");   
        exit(EXIT_FAILURE);   
    }   
     
    //type of socket created  
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons(portno);
         
    //bind the socket to localhost port 8888  
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)   
    {   
        perror("bind failed");   
        exit(EXIT_FAILURE);   
    }     
         
    //try to specify maximum of 3 pending connections for the master socket  
    if (listen(master_socket, 3) < 0)   
    {   
        perror("listen");   
        exit(EXIT_FAILURE);   
    }   
         
    //accept the incoming connection  
    addrlen = sizeof(address);   
    puts("Waiting for connections ...");   
    
    while(1)   
    {   
        //clear the socket set  
        
        FD_ZERO(&readfds);   
     
        //add master socket to set  
        FD_SET(master_socket, &readfds);   
        max_sd = master_socket;   
             
        //add child sockets to set  
        for ( i = 0 ; i < max_clients ; i++)   
        {   
            //socket descriptor  
            sd = client_socket[i];   
                 
            //if valid socket descriptor then add to read list  
            if(sd > 0)   
                FD_SET( sd , &readfds);   
                 
            //highest file descriptor number, need it for the select function  
            if(sd > max_sd)   
                max_sd = sd;   
        }   
     
        //wait for an activity on one of the sockets , timeout is NULL ,  
        //so wait indefinitely  
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);   
       
        if ((activity < 0) && (errno!=EINTR))   
        {   
            printf("select error");   
        }   
             
        //If something happened on the master socket ,  
        //then its an incoming connection  
        if (FD_ISSET(master_socket, &readfds))   

        {   if ((controlfd = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)  
                      
            {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }   
            
            cout<<"Connected with client socket number "<<controlfd<<endl;
            bzero(send_buffer,MAXLINE);strcpy(send_buffer,"Connected to server");
            //send new connection greeting message  
            if( send(controlfd, send_buffer, MAXLINE, 0))   
            {   
                perror("send");   
            }  
                 
            //add new socket to array of sockets  
            for (i = 0; i < max_clients; i++)   
            {   
                //if position is empty  
                if( client_socket[i] == 0 )   
                {   
                    client_socket[i] = controlfd;   
                    //printf("Adding to list of sockets as %d\n" , i);   
                         
                    break;   
                }   
            }   
        } 
        for (i = 0; i < max_clients; i++)   
        {   
            sd = client_socket[i];
                 
            if (FD_ISSET( sd , &readfds))   
            {   
                bzero(buffer,MAXLINE);   
                //Check if it was for closing , and also read the  
                //incoming message  
                if ((valread = recv(sd, buffer, MAXLINE, 0)) == 0) { 
					// somebody disconnected, get his details and print 
					
					cout << "Client with socket fd "<<sd <<" disconnected "<< endl; 
						
					// close the socket and mark as 0 in list for reuse 
					close(sd); 
					client_socket[i] = 0; 
				} 
                else{
                    string command;
                    string file_name;
                    command_info cmd;
                    const char *fn;

                    command = buffer;
                    bzero(buffer,MAXLINE);
                    command = remove_space(command);

                    cout<<"client  socket "<<sd <<" sent message :"<<command<<endl;
                    
                    cmd = get_command(command ,sd);bzero(send_buffer,MAXLINE);
                    bzero(send_buffer,MAXLINE);
                    
                    if(cmd.isvalid ==1){

                        strcpy(send_buffer,"1");
                        send(sd,send_buffer,MAXLINE,0);
                        command = cmd.cmd_name;
                        char com[command.length() + 1];

                        int i=0;
                        for(;i < command.length() ;i++)
                            com[i] = command[i];
                        
                        com[i] = '\0';
                        send(sd , com,MAXLINE,0);
                    }
                    else{
                        strcpy(send_buffer,"0");
                        send(sd,send_buffer,MAXLINE,0);
                        continue;
                    }
                    if(cmd.isfile == 1){
                        
                        file_name = cmd.filename;
                        fn = file_name.c_str();
                         
                    }
                    bzero(send_buffer,MAXLINE);
                    if(command == "codejud"){

                        
                        if(!is_valid_extension(file_name)){
                            strcpy(send_buffer,"0");
                            n = send(sd,send_buffer,MAXLINE,0);  //sending the file extension is invalid,only .c or .cpp allowed
                            continue;
                        }
                        else{
                            strcpy(send_buffer,"1");
                            n = send(sd,send_buffer,MAXLINE,0);
                        }

                        if (n < 0)
                            error((char*)"ERROR reading from socket");
                    }
                    if(command == "stor"){
                        
                        
                        char message[MAXLINE],data_buffer[MAXLINE],num_lines[MAXLINE],remain_bytes[MAXLINE];

                        FILE *fp;
                        n = send(sd,fn,MAXLINE,0);  //sending filename
                        if (n < 0)
                            error((char*)"ERROR reading from socket");

                        bzero(message,MAXLINE);
                        n = recv(sd,message,MAXLINE,0);// 1 indicates that subsequently client going to send some file data
                        if (n < 0)
                            error((char*)"ERROR reading from socket");

                        if(strcmp("1",message)==0){     

                            cout<<"Receiving file "<<fn<<" from client with socket nummber "<<sd<<endl;

                            if((fp = fopen(fn,"w"))==NULL){

                                cout<<"Error in creating file\n";
                                bzero(send_buffer,MAXLINE);
                                strcpy(send_buffer , "0");

                                n = send(sd , send_buffer,MAXLINE,0);
                                if( n<0 )   
                                    error((char*)"ERROR writing to socket");
                            }

                            else
                            {
                                bzero(send_buffer,MAXLINE);
                                strcpy(send_buffer , "1");

                                send(sd , send_buffer,MAXLINE,0);       //indicating succesful opening of file 
                                n = recv(sd, num_lines, MAXLINE,0);       //number of lines in file
                                
                                if (n < 0)
                                    error((char*)"ERROR reading from socket");
                                num_line = atoi(num_lines);

                                for(int i= 0; i < num_line; i++) { 

                                    n = recv(sd, data_buffer, MAXLINE,0);
                                    if (n < 0)
                                        error((char*)"ERROR reading from socket");
                                    fwrite(data_buffer,sizeof(char),MAXLINE,fp);
                                
                                }

                                n = recv(sd, remain_bytes, MAXLINE,0);//Remaining bytes
                                if (n < 0)
                                     error((char*)"ERROR reading from socket");  
                                byte_remain = atoi(remain_bytes);

                                if (byte_remain > 0) { 

                                    n = recv(sd, data_buffer, MAXLINE,0);
                                    if (n < 0)
                                        error((char*)"ERROR reading from socket");
                                    fwrite(data_buffer,sizeof(char),byte_remain,fp);
                                    
                                }
                                fclose(fp);
                                cout<<"Received file "<<fn<<endl;
                            }
                        }
                        else
                            cout<<"File opening error at client side"<<endl;
                        
                        continue;
                    }
                
                    else{
                        if(command == "retr"){
                            
                            
                            char data_buffer[MAXLINE],num_lines[MAXLINE],remain_bytes[MAXLINE],message[MAXLINE];
                            FILE *fp;
                            n = send(sd,fn,MAXLINE,0);  //sending filename
                            if (n < 0)
                                error((char*)"ERROR reading from socket");

                            bzero(message,MAXLINE);
                            if ((fp = fopen(fn,"r"))!=NULL)
                            {
                                cout<<"Sending file "<<fn<<" to client with socket nummber "<<sd<<endl;

                                bzero(send_buffer,MAXLINE);
                                strcpy(send_buffer , "1");

                                n = send(sd,send_buffer,MAXLINE,0);      // 1 indicates that subsequently client going to send some file data
                                if( n<0 )   
                                    error((char*)"ERROR writing to socket");

                                recv(sd,message,MAXLINE,0);      //To check if file at client end opened successfully
                                
                                if (strcmp("1", message) == 0){
                                    fseek (fp , 0 , SEEK_END);

                                    file_size = ftell (fp);
                                    rewind (fp);

                                    num_line = file_size / MAXLINE;
                                    byte_remain = file_size % MAXLINE;

                                    sprintf(num_lines,"%d",num_line);
                                    send(sd, num_lines, MAXLINE,0);
                                    

                                    for(int i= 0; i < num_line; i++) { 

                                        fread (data_buffer,sizeof(char),MAXLINE,fp);
                                        send(sd, data_buffer, MAXLINE,0);
                                        
                                    }
                                    sprintf(remain_bytes,"%d",byte_remain);
                                    send(sd, remain_bytes, MAXLINE,0);

                                    if (byte_remain > 0) { 
                                        fread (data_buffer,sizeof(char),byte_remain,fp);
                                        send(sd, data_buffer, MAXLINE,0);
                                        
                                    }
                                    fclose(fp);
                                    cout<<"Sent file "<<fn<<endl;
                                }
                                else
                                    cout<<"File openiing problem at client side"<<endl;
                            }
                            else{
                                bzero(send_buffer,MAXLINE);
                                strcpy(send_buffer , "0");

                                send(sd,send_buffer,MAXLINE,0);      //File does not exist at server
                                cout<<"File opening problem,file does not exist"<<endl;
                            }
                            continue;
                            
                        }
                        else{
                            
                            if( command == "list"){
                                
                                char data_buffer[MAXLINE];
                                
                                FILE *input;
                                
                                if(!(input = popen("ls", "r"))){
                                    cout<<"error"<<endl;
                                }
                                cout<<"Sending list of files to client with socket nummber "<<sd<<endl;
                                
                                while(fgets(data_buffer, sizeof(data_buffer), input)!=NULL){

                                    bzero(send_buffer,MAXLINE);
                                    strcpy(send_buffer , "1");

                                    send(sd,send_buffer,MAXLINE,0);
                                    send(sd, data_buffer, MAXLINE,0);
                                    
                                }
                                bzero(send_buffer,MAXLINE);
                                strcpy(send_buffer , "0");

                                send(sd,send_buffer,MAXLINE,0);
                                pclose(input);
                                cout<<"Sent list of files"<<endl;      //successfully sent files
                                continue;
                            }
                            else{
                                
                                if(command == "quit"){
                                    cout<<"client with socket number "<<sd<<" disconnected"<<endl;
                                    close(sd);
                                    client_socket[i] = 0;
                                }
                                else{
                                    if(command == "dele"){

                                        bzero(send_buffer,MAXLINE);
                                        
                                        if(remove(fn) ==0){
                                            strcpy(send_buffer , "File deleted");
                                            send(sd,send_buffer,MAXLINE,0);
                                            cout<<"Deleted file "<<fn<<endl;
                                        }
                                        
                                        else{
                                            strcpy(send_buffer , "File not exist");
                                            send(sd,send_buffer,MAXLINE,0);
                                            cout<<"File does not  exist"<<endl;
                                        }
                                        
                                        continue;
                                    }
                                    else if(command == "codejud"){

                                        char message[MAXLINE],data_buffer[MAXLINE],num_lines[MAXLINE],remain_bytes[MAXLINE];
                                        const char *filename;
                                        
                                        string f = substring(file_name , 1 , file_name.length()-1);     //f willl contain cpp filename without socket fd
                                        filename = f.c_str();

                                        n = send(sd,filename,MAXLINE,0);  //sending filename
                                        if (n < 0)
                                            error((char*)"ERROR reading from socket");
                                        
                 
                                        FILE *fp;
                                        bzero(message,MAXLINE);
                                        n = recv(sd,message,MAXLINE,0);
                                        if (n < 0)
                                            error((char*)"ERROR reading from socket");

                                        if(strcmp("1",message)==0){     // 1 indicates that subsequently client going to send some file data

                                            cout<<"Receiving file "<<f<<" from client with socket nummber "<<sd<<endl;

                                            if((fp = fopen(fn,"w"))==NULL){

                                                cout<<"Error in creating file\n";
                                                bzero(send_buffer,MAXLINE);
                                                strcpy(send_buffer , "0");

                                                n = send(sd , send_buffer,MAXLINE,0);
                                                if( n<0 )   
                                                    error((char*)"ERROR writing to socket");
                                            }

                                            else
                                            {
                                                bzero(send_buffer,MAXLINE);
                                                strcpy(send_buffer , "1");

                                                send(sd , send_buffer,MAXLINE,0);       //indicating succesful opening of file 
                                               
                                                n = recv(sd, num_lines, MAXLINE,0);       //number of lines in file
                                                if (n < 0)
                                                    error((char*)"ERROR reading from socket");

                                                num_line = atoi(num_lines);

                                                for(int i= 0; i < num_line; i++) { 

                                                    n = recv(sd, data_buffer, MAXLINE,0);
                                                    if (n < 0)
                                                        error((char*)"ERROR reading from socket");
                                                    fwrite(data_buffer,sizeof(char),MAXLINE,fp);
                                                
                                                }

                                                n = recv(sd, remain_bytes, MAXLINE,0);
                                                if (n < 0)
                                                    error((char*)"ERROR reading from socket");  
                                                byte_remain = atoi(remain_bytes);

                                                if (byte_remain > 0) { 

                                                    n = recv(sd, data_buffer, MAXLINE,0);
                                                    if (n < 0)
                                                        error((char*)"ERROR reading from socket");
                                                    fwrite(data_buffer,sizeof(char),byte_remain,fp);
                                                    
                                                }
                                                fclose(fp);
                                                cout<<"Received file "<<f<<endl;

                                            }
                                            
                                            int check = run(f,sd);      //compile and run program
                                            if(check == -1){
                                                bzero(send_buffer,MAXLINE);
                                                strcpy(send_buffer , "Problem occured at server side");

                                                send(sd, send_buffer, MAXLINE,0) ;
                                                continue;
                                            }
                                            if(check ==1){
                                                bzero(send_buffer,MAXLINE);
                                                strcpy(send_buffer , "COMPILE_ERROR");

                                                send(sd, send_buffer, MAXLINE,0) ;
                                            }
                                            else{
                                                bzero(send_buffer,MAXLINE);
                                                strcpy(send_buffer , "COMPILE_SUCCESS");
                                                send(sd, send_buffer, MAXLINE,0) ;

                                                if(check ==2){
                                                    bzero(send_buffer,MAXLINE);
                                                    strcpy(send_buffer , "RUN_ERROR");
                                                    send(sd, send_buffer, MAXLINE,0) ;
                                                }
                                                else{
                                                    if(check ==3){
                                                        bzero(send_buffer,MAXLINE);
                                                        strcpy(send_buffer , "TIME LIMIT EXCEEDED");
                                                        send(sd,send_buffer , MAXLINE,0) ;
                                                    }
                                                    else{
                                                        bzero(send_buffer,MAXLINE);
                                                        strcpy(send_buffer , "RUN_SUCCESS");

                                                        send(sd,send_buffer , MAXLINE,0) ;
                                                        if(check ==5){
                                                            bzero(send_buffer,MAXLINE);
                                                            strcpy(send_buffer , "WRONG_ANSWER");
                                                            send(sd, send_buffer, MAXLINE,0) ;
                                                        }
                                                        else if(check ==4){
                                                            bzero(send_buffer,MAXLINE);
                                                            strcpy(send_buffer , "ACCEPTED");
                                                            send(sd, send_buffer, MAXLINE,0) ;
                                                        }
                                                        
                                                    }
                                                }
                                            }

                                        }
                                        else
                                            cout<<"File opening error at client side"<<endl;
                                        
                                        continue;
                                    }
                                }
                                
                            }

                        }
                    }
        //---------------------------------Done ------------------------------------------------------------
                }    
            }
                   
        }   
    }   
    return 0;  

}  

    
  
        
       

