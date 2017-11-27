//   scmd a simple and small unix shell
//
//	Copyright (C) 2017 Streanga Sarmis-Stefan.
//
//	This program is free software : you can redistribute it and / or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program, if not, see <http://www.gnu.org/licenses/>.

#include "command/command.h"

void initPipe(){
	pipe(mainPipe);
}


void initCommander(){
	kids = 0;
}

void initForkKid(){
	pid = fork();
	kids++;
}

/* @OldCode
void printPointer(){
	printf("$> ");
}
*/

void waitInput(){

	/* @OldCode	
	char c;
	int canContinue = 1;	
	while(canContinue){
		getch();
		getch();
		char tmp = getch();
		// @FixMe
		switch(tmp){
			case 'A': // up arrow
				goHistUp();
				printHistLine();
				canContinue = 1;
				break;

			case 'B': // down arrow
				goHistDown();
				printHistLine();
				canContinue = 1;
				break;

			default:
				fputc(tmp, stdin);
				canContinue = 0;
				break;
		}
	}
	//} // else write(STDOUT_FILENO, c, sizeof(c));
	*/

	
	char* buf = readline("$> ");
	
	// @OldCode
	// fgets(buf, MAX_LINE_SIZE, stdin);
	
	parseCommand(buf);

	free(buf);
}


void parseCommand(const char* cmd){

	char* tmp = cmd;
	int checker = 0;

	while(tmp = strstr(tmp, " ")){
		checker++;
		tmp++;
	}

	add_history(cmd);
	
	if(!checker) { // just one thing in the command line
		if(strlen(cmd) != 0){

			///
			/// Things get interpreted here VVV just one command so nothing too complicated
			/// no args, no nothing.
			///
			/// @OldCode
			/// goHistDown();
			/// appendCommand(cmd);


			if (strcmp(cmd, "exit") == 0) commandExit();
			else { // not a listed command
			}
	
			// else if (strcmp(cmd, "...") != 0) command function
			// etc
			
		} else {
			printf("Could not read parse an empty command\n");
		}
	} else { // multiple things on the command line

		kids++;
		initForkKid();

		if(pid > 0){

			char* tmp = cmd;
			char** args;
			args = (char**)malloc(sizeof(char*) * 12); // 12 should be the maximum words in command line 
										        // so that I don't use sysconf to see how many are max
			int num = 0;
	
			while((tmp = strtok(tmp, "\n")) != NULL){
				// store things in the char* array
	
				args[num] = (char*)malloc(sizeof(char) * strlen(tmp));
				args[num++] = tmp;
				tmp = strtok(NULL, "\n");
			}
			
			num++;
			
			close(mainPipe[0]);
	
			write(mainPipe[1], &num, sizeof(num));
			int i;
		
			for(i = 0; i < num; i++){
				int size = strlen(args[i]) * sizeof(char);
				write(mainPipe[1], &size, sizeof(size));
				write(mainPipe[1], args[i], size);
			}
	
			close(mainPipe[1]);
	
			free(args);


		} else if(pid == 0){
			close(mainPipe[1]);

			int num;
			read(mainPipe[0], &num, sizeof(num));

			char** args = (char**)malloc((num + 1) * sizeof(char*));
	
			int i;
			for(i = 0; i < num; i++){
				if(i == 0){
					int size;
					read(mainPipe[0], &size, sizeof(size));

					args[i] = (char*)malloc(size + 6);
					char* tmpc = (char*)malloc(size + 1);
					
					read(mainPipe[0], tmpc, size);
					tmpc[size] = 0;

					char* str1 = (char*)malloc(size + 6);

					str1[0] = '/';
					str1[1] = 'b';
					str1[2] = 'i';
					str1[3] = 'n';
					str1[4] = '/';
					
					strcat(str1, tmpc);
					
					args[i] = str1;	
					args[i][size + 5] = 0;
				} else {
					int size;
					read(mainPipe[0], &size, sizeof(size));

					args[i] = (char*)malloc(size);
					read(mainPipe[0], args[i], size);
					args[i][size] = 0;
				}
			}

			args[num] = NULL;

			kids--;

			close(mainPipe[0]);
			
			execl(args[0], args);
			
			printf("Could not execute ! \n%s %s %s\n$> ", args[0], args[1], args[2]);

			free(args);

			exit(127); //@CleanUp
		}

	}
}

void waitForKids(){
	if (getKids() > 0){
		int i = 0;
		for(; i < getKids(); i++) wait();
	}
}

uint32_t getKids(){
	return kids;
}


///
/// function copied from https://www.daniweb.com
/// from user jaybhanderi 
/// equivalent for getch()
/// @OldCode
/*
int getch(){
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}
*/
///

