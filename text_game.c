
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fail(char *message)
{	
	// print message and exit
	printf("%s\n", message);
	exit(0);
}

void seek_line(char *ret_line, char *type, char *state, char* filename)
{
	FILE *fp;
	fp = fopen(filename, "r");

	// exit if the file can't be used for whatever reason
	if (fp == NULL) {
		fail("No such file or could not open file.");
	}

	char in_state = 0;
	char line[81];

	while (fgets(line, 81, fp)) {
		// find the state we want
		if (line[0] == '#' && !strncmp(state, line+1, strlen(state))) {
			in_state = 1;
		}

		// found the line
		if (in_state && !strncmp(line, type, strlen(type))) {
			strcpy(ret_line, line);
			break;
		}
	}

	fclose(fp);
}

int parse_narration(char *state, char* gamefile)
{
	char line[81];
	int choices = 0;

	// find the narration line
	seek_line(line, ".", state, gamefile);

	char *choice_index = strchr(line, '?');

	// check if there are choices available
	if (choice_index == NULL) {
		choices = 0;
	}
	else {
		choices = *(choice_index + 1) - 48;
		*(choice_index + 1) = ' ';
	}

	printf("%s", line + 1);

	return choices;
}

void parse_choices(int num_choices, char *next_state, char *state, char *gamefile)
{
	int i = 1;
	char seek[] = "?1";

	// print all listed choices
	while (i <= num_choices) {

		// seek each choice
		char line[81];
		seek_line(line, seek, state, gamefile);
		seek[1] = seek[1] + 1;
		i++;

		// find the substring to print
		char print_choice[81];
		strncpy(print_choice, line + 1, strcspn(line, "=") - 2);
		print_choice[strcspn(line, "=") - 2] = '\0';

		printf("%s\n", print_choice);
	}
}

void parse_state(int choice, char *state, char *gamefile)
{
	char line[81];

	if (choice) {
		// seek the correct choice line
		char seek[] = "?0";
		seek[1] = seek[1] + choice;

		seek_line(line, seek, state, gamefile);
	}
	else {	
		seek_line(line, "=", state, gamefile);
	}

	// parse the new state
	char *new_state = strchr(line, '=');

	// copy the new state into the char array
	strcpy(state, new_state + 2);
	state[strlen(new_state + 2) - 1] = '\0';
}

void play(char *gamefile)
{
	char state[] = "START";

	// run until we get to STOP
	while (strcmp(state, "STOP")) {

		// first, find and print the narration
		int choices = parse_narration(state, gamefile);

		// if there are choices, find them and list them
		if (choices) {
			char next_state[81];
			parse_choices(choices, next_state, state, gamefile);

			// get the choice from player
			int choice;
			printf("\nWhat do you do? ");
			scanf("%d", &choice);

			// use the choice to move to next state
			// hilariously, invalid inputs still lead to death
			parse_state(choice, state, gamefile);
		}
		else {
			// there was no choice, just move to next state
			parse_state(0, state, gamefile);
		}
	}
}

int main(int argc, char **argv)
{	
	// exit if no filename is given
	if (argc < 2) {
		fail("Usage: text_game <file>");
	}

	// start the main game loop
	play(argv[1]);
}
