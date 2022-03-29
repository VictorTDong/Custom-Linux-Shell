/*
Purpose: Create a functioning shell handling simple commands along with redirection, pipes and a specified alternate pipe
Limitations and assumption: None
Known bugs: None
Bonus: Can handle an arbitrary amount of commands

Usage: To make and run code
            make all
            ./MyShell
            
       To clean files
            make clean
*/

#include <shell.h>

int main()
{
    printf("\nShell has started\n\n");
    shell();
    return 0;
}

/*
Function: shell
Purpose: The driver for the shell
Method: Runs through various checks and will send commands to their corresponding functions
Input: N/A
Output: N/A
*/
void shell()
{
    string background = "&";
    char input[MAX_SIZE];
    char inputForRedirection[MAX_SIZE];

    int direction;
    int dontWait;

    vector<char*> tokens;
    vector<char*> left;
    vector<char*> right;

	while(true)
    {
        printf("myShell$ ");

        fgets(input,MAX_SIZE,stdin);
        strncpy(inputForRedirection, input, MAX_SIZE);

        int isAlternate = parseAlternatePipe(input, left, right);
        if(isAlternate == 1)
        {
            alternatePipe(left, right);
        }
        else
        {
            int isPipe = parsePipe(input, tokens, left, right);
            if(strstr(tokens[0],"exit"))
            {
                printf("Shell exiting\n");
                break;
            }
            // Checks to see if the program needs to run the next command in the background
            if(tokens.size() != 0 && tokens.back() == background)
            {
                dontWait = 1;
                tokens.pop_back();
                tokens.shrink_to_fit();
            }
            if(right.size() != 0 && right.back() == background)
            {
                dontWait = 1;
                right.pop_back();
                right.shrink_to_fit();
            }

            if(isPipe == 1)
            {
                if(right.empty() != 1 && left.empty() != 1)
                {
                    pipedCommands(left, right, dontWait);
                }
                else if(left.empty())
                {
                    printf("Left hand of pipe cannot be empty \n");
                }
                else 
                {
                    printf("Right hand of pipe cannot be empty \n");
                }
            }

            direction = parseRedirection(inputForRedirection, left, right);
            if(direction != 0)
            {
                handleDirection(direction, left, right, dontWait);
            }

            if(isPipe == 0 && direction == 0)
            {   
                singleCommand(tokens, dontWait);
            }
        }
        
        // Clears the vectors and allocates the memory to nothing
        vector<char *>().swap(tokens);
        vector<char *>().swap(left);
        vector<char *>().swap(right);

        direction = 0;
        dontWait = 0;
	}
}

/*
Function: parseRedirection
Purpose: To handle the server response and send back to the browser
Method: This function will parse the input based on the user input, it will look for one of the redirection symbols (<, >).
        After it finds this symbol, it will split the input into a command and a filename to be used in the redirection.
Input: char* to represent the user input
Output: int to represent if a redirection is present in the input
*/
int parseRedirection(char* input, vector<char*> &command, vector<char*> &filename)
{
    char redirection[] = "><";
    vector<char*> tokens;
    char* token;
    int direction = 0;

    for(int i = 0; input[i] != '\0'; i++)
    {
        if(input[i] == '>')
        {
            direction = -1;
            break;
        }
        if(input[i] == '<')
        {
            direction = 1;
            break;
        }
    }
    if(direction != 0)
    {
        token = strtok(input,redirection);

        while(token != 0)
        {
            tokens.push_back(token);
            token = strtok(NULL,redirection);
        }
        if(tokens.size() > 1)
        {
            parseSpace(tokens[0],command);
            parseSpace(tokens[1],filename);
        }
    }
    return direction;
}

/*
Function: parseAlternatePipe
Purpose: To parse input to be used in the alternate pipe command
Method: The function will parse the input based on the alternate pipe symbol and then will parse the sides based on the 
        delimiter " "
Input: char* to represent the user input
       Two char* vectors for each side of the pipe
Output: int to represent if the $ (alternate pipe) is present in the input
*/
int parseAlternatePipe(char* input, vector<char*> &leftPipe, vector<char*> &rightPipe)
{
    vector<char*> tokens;
    const char pipe[] = "$";
    char* token;
    char* left;
    char* right;

    token = strtok(input,pipe);

    while(token != 0)
    {
        tokens.push_back(token);
        token = strtok(NULL,pipe);
    }
    if(tokens.size() > 1)
    {
        parseSpace(tokens[0],leftPipe);
        parseSpace(tokens[1],rightPipe);
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
Function: parsePipe
Purpose: To parse input to be used in the pipe command
Method: The function will parse the input based on whether the pipe symbol is present. If the symbol is present, the input
        will be split into two sides and will be further parsed by the delimiter " ". If no pipe symbol is present, the input
        will just be parsed based on " ".
Input: char* to represent the user input
       char* vector to represent the tokens 
       Two char* vectors for each side of the pipe
Output: N/A
*/
int parsePipe(char* input, vector<char*> &tokens, vector<char*> &leftPipe, vector<char*> &rightPipe)
{
    const char pipe[] = "|";
    char* token;
    char* left;
    char* right;

    token = strtok(input,pipe);

    while(token != 0)
    {
        tokens.push_back(token);
        token = strtok(NULL,pipe);
    }
    if(tokens.size() > 1)
    {
        parseSpace(tokens[0],leftPipe);
        parseSpace(tokens[1],rightPipe);
        return 1;
    }
    else
    {
        tokens.clear();
        parseSpace(input, tokens);
        return 0;
    }
}

/*
Function: parseSpace
Purpose: To parse the spaces in the input to seperate into tokens
Method: The function will go through user input and parsed based on the delimiters " " and "\n"
Input: char* to represent the user input
       char* vector to represent the tokens 
Output: N/A
*/
void parseSpace(char* input, vector<char*> &tokens)
{
    const char space[] = " \n";
    char* token;

    token = strtok(input,space);

    while(token != 0)
    {
        tokens.push_back(token);
        token = strtok(NULL,space);
    }
}

/*
Function: alternatePipe
Purpose: To parse and direct to the correct alternate pipe call
Method: The function will parse the two vectors and split them up into four seperate commands. After it is seperated, the
        function will direct to the correct call with either left having 2 arguments, right having 1 argument or left
        having 1 argument, right having 2 arguments or left having 2 arguments, right having 2 arguments
Input: Two char* vectors for each side of the pipe
Output: N/A
*/
void alternatePipe(vector<char*> &leftPipe, vector<char*> &rightPipe)
{
    int leftSize = leftPipe.size();
    int rightSize = rightPipe.size();
    char *argLeft1[2];
    char *argLeft2[2];
    char *argRight1[2];
    char *argRight2[2];

    if(leftSize == 2)
    {
        argLeft1[0] = leftPipe[0];
        argLeft1[1] = NULL;
        argLeft2[0] = leftPipe[1];
        argLeft2[1] = NULL;
    }
    else
    { 
        argLeft1[0] = leftPipe[0];
        argLeft1[1] = NULL;
    }
    if(rightSize == 2)
    {
        argRight1[0] = rightPipe[0];
        argRight1[1] = NULL;
        argRight2[0] = rightPipe[1];
        argRight2[1] = NULL;
    }
    else
    {
        argRight1[0] = rightPipe[0];
        argRight1[1] = NULL;
    }

    if(leftSize == 2 && rightSize == 1)
    {
        handleAlternatePipe(1, 0, argLeft1, argLeft2, argRight1, argRight2);
    }

    else if(leftSize == 1 && rightSize == 2)
    {
        handleAlternatePipe(0, 1, argLeft1, argLeft2, argRight1, argRight2);
    }

    else if(leftSize == 2 && rightSize == 2)
    {
        handleAlternatePipe(1, 1, argLeft1, argLeft2, argRight1, argRight2);
    }
    else
    {
        printf("Invalid amount of arguments for alternate pipe\n");
    }
}

/*
Function: handleAlternatePipe
Purpose: To handle the alternate pipe operation of a shell
Method: The functionality is the same as the simple pipe command function with changes made to make it work for the alternate pipe symbol.
        The function will fork multiple childs in which the output of the left side commands are redirected from standard output into a pipe and will be used 
        on the read end of the pipe on the right hand side commands. The function will fork twice if the left hand side of the argument only has 1 command and
        will fork three times if there are 2 left hand side commands presents. If the right hand side of the argument contains two commands, then the function
        will loop twice with each iteration being a single command on the right hand side. 
Input: Four char* arrays to represent the commands on the corresponding side
       int left and right to indicate if either two left or right commands are present
Output: N/A
*/
void handleAlternatePipe(int left, int right, char *argLeft1[], char *argLeft2[], char *argRight1[], char *argRight2[])
{
    for(int i = 0; i < right + 1; i++)
    {
        int fd[2];
        if(pipe(fd) != 0)
        {
            perror("pipe failed");
            exit(EXIT_FAILURE);
        }

        pid_t child1 = fork();

        if(child1 == 0)
        {
            if(dup2(fd[0],0) == -1)
            {
                perror("dup2 failed");
                exit(EXIT_FAILURE);
            }
            close(fd[0]);
            close(fd[1]);
            
            //calls execvp depending on the iteration of the loop and whether user input has second right hand argmument
            if(i == 0)
            {
                execvp(argRight1[0], argRight1);
                perror("Error executing 1st command on the right of $");
            }
            if(i == 1 && right == 1)
            {
                execvp(argRight2[0], argRight2);
                perror("Error executing 2nd command on the right of $");
            }
            exit(EXIT_FAILURE);
        }
        else if (child1 == -1)
        {
            perror("Error forking child");
            exit(EXIT_FAILURE);
        }
        else
        {
            pid_t child2 = fork();

            if(child2 == 0)
            {
                if(dup2(fd[1],1) == -1)
                {
                    perror("dup2 failed");
                    exit(EXIT_FAILURE);
                }
                close(fd[0]);
                close(fd[1]);
                
                //Depends on whether or not the left hand side of pipe has 1 or 2 arguments. This is to make sure the right argument is being called
                if(left == 1)
                {
                    execvp(argLeft2[0], argLeft2);
                    perror("Error executing 2nd command on the left of $");
                }
                if(left == 0)
                {
                    execvp(argLeft1[0], argLeft1);
                    perror("Error executing 1st command on the left of $");
                }
                exit(EXIT_FAILURE);
            }
            else if (child2 == -1)
            {
                perror("Error forking child");
                exit(EXIT_FAILURE);
            }
            else
            {
                //Checks to see if there is 1 or 2 left hand argmuments, if there is only one, then we do not need to fork a third child
                if(left == 1)
                {
                    pid_t child3 = fork();
                    if(child3 == 0)
                    {
                        if(dup2(fd[1],1) == -1)
                        {
                            perror("dup2 failed");
                            exit(EXIT_FAILURE);
                        }
                        close(fd[0]);
                        close(fd[1]);
                        execvp(argLeft1[0], argLeft1);
                        perror("Error executing 1nd command on the left of $");
                        exit(EXIT_FAILURE);
                    }
                    else if (child3 == -1)
                    {
                        perror("Error forking child");
                        exit(EXIT_FAILURE);
                    }
                    else
                    {
                        int status;
                        close(fd[0]);
                        close(fd[1]);
                        waitpid(child3, &status, WCONTINUED);
                    }
                }
                int status;
                if(left == 0)
                {
                    close(fd[0]);
                    close(fd[1]);
                }
                waitpid(child2, &status, WCONTINUED);
            }
            int status;
            waitpid(child1, &status, WCONTINUED);
        }
    }
}

/*
Function: pipedCommands
Purpose: To handle the basic pipe operation of a shell
Method: The function will recieve two char* vectors representing the right and left side of the input. The function will fork a child in which
        it calls a execvp to execute the right side of the pipe in which it will redirect its input into a pipe for the a new child forked in 
        the parent process. The child in the parent process will use the input in the pipe to execute the execvp call
Input: Two char* vector to represent the left and right tokens(commands and arguments)
       int dontWait to represent indicate that an & is present and to run execvp in background 
Output: N/A
*/
void pipedCommands(vector<char*> &leftPipe, vector<char*> &rightPipe, int dontWait)
{
    int i;
    int output = 0;
    int fd[2];
    int out; 
    if(pipe(fd) != 0)
    {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    //converts the char* vector to an array of pointers
    char *argLeft[MAX_SIZE];
    for(i = 0; i < leftPipe.size(); i++)
    {
        argLeft[i] = leftPipe[i];
    }
    argLeft[i] = NULL;

    char *argRight[MAX_SIZE];
    for(i = 0; i < rightPipe.size(); i++)
    {
        argRight[i] = rightPipe[i];
    }
    argRight[i] = NULL;

    int status = 0;
    pid_t child1, child2;
    child1 = fork();

    if(child1 == 0)
    {
        close(fd[0]);
        if(dup2(fd[1],1) == -1)
        {
            perror("dup2 failed");
            exit(EXIT_FAILURE);
        }
        close(fd[1]);
        execvp(argLeft[0], argLeft);
        perror("Error executing command on left left of |");
        exit(EXIT_FAILURE);
    }
    else if (child1 == -1)
    {
        perror("Error forking child");
        exit(EXIT_FAILURE);
    }
    else
    {
        close(fd[1]);
        child2 = fork();
        if(child2 == 0)
        {
            close(fd[1]);
            if(dup2(fd[0],0) == -1)
            {
                perror("dup2 failed");
                exit(EXIT_FAILURE);
            }
            close(fd[0]);
            execvp(argRight[0], argRight);
            perror("Error executing command on right left of |");
            exit(EXIT_FAILURE);
        }
        else if (child2 == -1)
        {
            perror("Error forking child");
            exit(EXIT_FAILURE);
        }
        else
        {
            if(dontWait == 0)
            {
                waitpid(child1,&status,WCONTINUED);
            }
            else
            {
                waitpid(child1,&status,WNOHANG);
            }
            close(fd[0]);
        }
        if(dontWait == 0)
        {
            waitpid(child1,&status,WCONTINUED);
        }
        else
        {
            waitpid(child2,&status,WNOHANG);
        }
    }
}

/*
Function: handleDirection
Purpose: Handles simple commands that include redirection
Method: The method will open the corresponding file and and redirect it to a file in which all the output will go to the
        corresponding file or all the input will be used in the execvp call
Input: int to represent the direction
       Two char* vector to represent the command and filename
       int dontWait to represent if the process is going to be ran in the background
Output: N/A
*/
void handleDirection(int direction, vector<char*> command, vector<char*> filename, int dontWait)
{
    int child;
    int status = 0;
    int success = 0;
    child = fork();
    if(child == 0)
    {
        if(direction == -1)
        {
            int out = open(filename[0], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
            success = dup2(out, 1);
            close(out);
        }
        else
        {
            int in = open(filename[0], O_RDONLY);
            success = dup2(in,0);
            close(in);
        }
        if(success != -1)
        {
            char **arg = &command[0];
            execvp(arg[0], arg);
            perror("Error executing command");
            exit(EXIT_FAILURE);
        }
        else 
        {
            printf("File %s could not be opened\n", filename[0]);
        }
    }
    else if (child == -1)
    {
        perror("Error forking child");
        exit(EXIT_FAILURE);
    }
    else 
    { 
        if(dontWait == 0)
        {
            waitpid(child,&status,WCONTINUED);
        }
        else
        {
            waitpid(child,&status,WNOHANG);
        }
    }
}

/*
Function: singleCommand
Purpose: To handle simple commands containing n number of arguments 
Method: The function will recieve a char* vector with the tokens which were parsed and null terminated. Indices 0 will be the command
        and every other token will represent the arguments. The function will fork a child and run execvp to execute the command given.
Input: char* vector to represent the tokens(commands and arguments)
       int dontWait to represent indicate that an & is present and to run execvp in background 
Output: N/A
*/
void singleCommand(vector<char*> tokens, int dontWait)
{
    int status = 0;
    char *arg[MAX_SIZE];
    int i;
    for(i = 0; i < tokens.size(); i++)
    {
        arg[i] = tokens[i];
    }
    arg[i] = NULL;

    pid_t pid = fork();

    if(pid == 0)
    {
        execvp(arg[0], arg);
        perror("Error executing command");
        exit(EXIT_FAILURE);
    }
    else if (pid == -1)
    {
        perror("Error forking child");
        exit(EXIT_FAILURE);
    }
    else
    {
        if(dontWait == 0)
        {
            waitpid(pid,&status,WCONTINUED);
        }
        else
        {
            waitpid(pid,&status,WNOHANG);
        }
    }
}