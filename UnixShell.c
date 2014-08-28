//Kasra kazemi
//Shell project
//This Project modelize an unix terminal

#include <stdio.h>
#include <unistd.h>                     //fork()
#include <sys/types.h>                  //WUNTRACED
#include <sys/wait.h>                   //waitpid()
#include <string.h>
#include <stdlib.h>
#include <termios.h>

void PrintPrompt();                 //print the current directory
void AcceptCommand(char * commandline, char * commands [], char * history []);        //accept and ecute the command line

int main(){
//------------------------------------------------------------------------------------
    struct termios origConfig;
    tcgetattr(0, &origConfig);      //get original configuration

    struct termios newConfig = origConfig;  // copy original config
    newConfig.c_lflag &= ~(ICANON|ECHO);    // Non-canonical mode; disable echo
    newConfig.c_cc[VMIN] = 1;
    newConfig.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &newConfig);      //set new config
//------------------------------------------------------------------
    int fd [2];                                 //file descriptor for pipe
    int pid, pid2, status,status2, statusDirctory;       //P ID, status for cd
    char * commands[256];
    char * commands2[256];
    char * CommandLine;
    char c;
    char backspace = '\010', DeletBoutton = '\177', newLine = '\012';         //for reading commandLine char by bhar
    char left = '\103', right = '\104', Up = '\101', Down = '\102';                                          //history count
    char del = '\10';
    char sp = ' ';

    char * HistoryElem;
    char * History[10];
    int historyCount=0;
    int UpCount =0, UpUse =0;
    int len=0;
    int HistoryLocation=0;
    int last;
    int i =0;                           //just fixe a big problem that I cannot figure out why it happends

    while (1) {
        PrintPrompt();

        pipe(fd);
        commands[0] = NULL;
        commands2[0] = NULL;



        CommandLine = (char*) calloc(256, sizeof(char));
        HistoryElem = (char*) calloc(256, sizeof(char));

        len =0;
        historyCount=0;
        UpCount=0;

        if(HistoryLocation > 0){
            last = HistoryLocation-1;
        }

        while(1){
            read(0, &c, 1);                        //read the command Char by Char
            UpUse =0;
            if(c == newLine){
                break;
            }
//---------------------------------------------------
            else if(c == '\033'){}
            else if(c == '\133'){}

            else if (c == left || c == right) {}      //left & rights
//-------------------------------------------------------------------------


            else if(c == Up){                               //Up


                if(UpUse >= 0 && UpUse <= 8){UpUse++;}
                if(UpUse > 8){UpUse =0;
                UpUse++;
                }

                if(UpUse != 0){
                if(i == 0){}
                else if(History[0] != NULL){
                    if(historyCount != 9){
                        if(len != 0){
                            len--;
                            while(len >= 0){
                                CommandLine[len] = '\0';
                                write(1, &del, 1);
                                write(1, &sp, 1);
                                write(1, &del, 1);
                                len--;
                            }
                        }
                        if(last != 0){
                            if(History[last] != NULL){
                            if(UpCount == 1){
                                last--;
                                historyCount++;
                            }

                           write(1, History[last], strlen(History[last]));
                            strcat(CommandLine, History[last]);
                            len = strlen(History[last]);}
                        }
                        else {
                            if (History[last] != NULL){
                            if(History[9] != NULL){
                            if(UpCount ==1){
                                last =9;
                                historyCount++;
                            }
                           write(1, History[last], strlen(History[last]));
                            strcat(CommandLine, History[last]);
                            len = strlen(History[last]);
                        }
                            }
                        else if (History[last] != NULL){
                           write(1, History[last], strlen(History[last]));
                            strcat(CommandLine, History[last]);
                            len = strlen(History[last]);
                        }
                        }

                    }

                }
                UpCount =1;
                }
            }
//--------------------------------------------------------------------------
            else if(c == Down){                           //Down
                if (History[0] != NULL) {
                    if (historyCount != 0) {   // Prevent more than 10 forward history recalls
                        if (len != 0) {   // Clear any characters typed at prompt
                            len--;

                            while (len >= 0) {
                                CommandLine[len] = '\0';
                                write(1, &del, 1);
                                write(1, &sp, 1);
                                write(1, &del, 1);

                                len--;
                            }
                        }
                        if (last != 9) {  // Return forward history recall
                            last++;
                            write(1, History[last], strlen(History[last]));
                            strcat(CommandLine, History[last]);
                            len = strlen(History[last]);
                            historyCount--;
                        }
                        else {
                            if (History[9] != NULL) {
                                // Jump to bottom of full history array after top element is passed
                                write(1, History[0], strlen(History[0]));
                                strcat(CommandLine, History[0]);
                                last = 0;
                                historyCount--;
                            }
                        }
                    } // Do nothing if completed 10 forward history recalls
                    }


            }



// Delete a character--------------------------------------------------------------

            else if ((c == backspace) || (c == DeletBoutton)) {  // Backspace or delete

                    if(len > 0){
                    CommandLine[--len] = '\0';

                    write(1, &del, 1);
                    write(1, &sp, 1);
                    write(1, &del, 1);

                }
                else if (i <= 0){}  //do nothing if it is first letter
            }
//------------------------------------------------------------------------no special case
            else{
                i =1;
                CommandLine[len] = c;             //Make CommandLine ready
                write(1, &CommandLine[len], 1);
                len++;
            }

        }

//------------------------------------------------------------------

        printf("\n");               //equilalent to break key
        strcpy(HistoryElem, CommandLine);

        //if user doesn't enter any command but only enter line, then it simply print the prompt again.

        if(CommandLine[0] == '\0'){}
        else{

            if(HistoryLocation != 9){ History[HistoryLocation] = HistoryElem;
                HistoryLocation++;
            }
            else{ History[HistoryLocation] = HistoryElem;
                HistoryLocation=0;
            }
            AcceptCommand(CommandLine, commands, commands2);   //accept input

        }

//execution ----------------------------------------------------------------------------------------------
            if(commands[0] == NULL){}

            else if(strcmp(commands[0],"exit" )== 0){        //exit command
                break;
            }

            else if(strcmp(commands[0],"cd" )== 0){          //cd command

                statusDirctory = chdir(commands[1]);
                if (statusDirctory == -1) {
                    perror(NULL);
                }
            }

            pid = fork();
            pid2 = fork();
            if(pid2 == 0){
                close(fd[1]);
                dup2(fd[0],0);
                close(fd[0]);
                execvp(commands2[0], commands2);

                perror(NULL);
                exit(0);
            }

            if(pid == 0){
                if(strcmp(commands[0],"cd" )== 0){          //cd command
                exit(0);
            }
            else if(commands2[0] != NULL){
                close(fd[0]);
                dup2(fd[1],1);
                close(fd[1]);


                execvp(commands[0], commands);              //execute the commands
                perror(NULL);                               //error in case of invalid command
                exit(0);
                }

                execvp(commands[0], commands);              //execute the commands
                perror(NULL);                               //error in case of invalid command
                exit(0);
                               //if after an invalid command, there is exit it makes it work
            }
            else{
                close(fd[1]);

            waitpid(pid, &status, WUNTRACED);           //wait for process id

            waitpid(pid2, &status2, WUNTRACED);
            }

        }



    tcsetattr(0, TCSANOW, &origConfig);     // Restore config
    return 0;               //doooooooooooooooooooooooooooooone!!!!!!!!!!!!!!!

}

/* Accept the command line already read in main
it takes the string passed by value,
transform a single string into an array of string
using strtok to reconize arguments */
void AcceptCommand(char * commandline, char * commands [], char * commands2[]){

    int i =0;
    int l =0;

    //using strtok to reconize arguments
        commands[0] = (char *) strtok(commandline, " \n");

        if(commands[0] == NULL){}       //if press enter, just do nothing, IMPORTANT without doesn't work

        else if(strcmp(commands[0], "cd") == 0){
            commands[1] = (char*) strtok(NULL, " \n");
        }

        else{
            while(commands[i] != NULL){
                i++;                                    //fill out the array of string by the argument given
                commands[i] = (char*) strtok(NULL, " \n");
                if(commands[i] != NULL){
                    if(strcmp(commands[i], "|") == 0){
                        commands[i] = NULL;
                        commands2[0] = (char *) strtok(NULL, " \n");

                        while(commands2[l] != NULL){
                            l++;
                            commands2[l] = (char *) strtok(NULL, " \n");
                        }
                        commands2[l] = NULL;
                    }
                }

            }
            commands[i] = NULL;

        }

}

/* Print current directory */
void PrintPrompt(){

    char *dirVar = (char*) calloc(256, sizeof(char)); //string for dir command

    getcwd(dirVar, 256);                //get the current directory
    dirVar[strlen(dirVar)] = '-';
    dirVar[strlen(dirVar)] = '>';

    write(1,dirVar , 256);            //display the directory followed by ->

}
