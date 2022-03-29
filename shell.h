#include <iostream>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <sys/wait.h>
#include <fcntl.h>

using namespace std;

void shell();

void parseSpace(char* input, vector<char*> &tokens);
void singleCommand(vector<char*> tokens, int dontwait);

void pipedCommands(vector<char*> &leftPipe, vector<char*> &rightPipe, int dontWait);
int parseRedirection(char* input, vector<char*> &command, vector<char*> &filename);

void handleDirection(int direction, vector<char*> command, vector<char*> filename, int dontWait);
int parsePipe(char* input, vector<char*> &tokens, vector<char*> &leftPipe, vector<char*> &rightPipe);

int parseAlternatePipe(char* input, vector<char*> &leftPipe, vector<char*> &rightPipe);
void alternatePipe(vector<char*> &leftPipe, vector<char*> &rightPipe);
void handleAlternatePipe(int left, int right, char *argLeft1[], char *argLeft2[], char *argRight1[], char *argRight2[]);

const int MAX_SIZE = 1024;