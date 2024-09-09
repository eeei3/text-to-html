#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static char TEMPLATE_FILE[40] = "testtemplate";
// TODO add a feature that will account for ~'s that are NOT part of the seperator

typedef struct input {
    char *file_contents;
    char *file_name;
    int file_size;
} Input;

void die() {
}

int allocate_space(Input* p, int size) {
    p->file_contents = malloc(size * sizeof(char));
    p->file_name = malloc(255 * sizeof(char));
    if ((!p->file_contents) || (!p->file_name))
	return 1;
    p->file_size = size;
    return 0;
}

void free_input(Input* data) {
    free(data->file_contents);
    free(data->file_name);
    free(data);
    return;
}

void read_template(char* p, char* front, char* tail) {
    int pointer = 0; // Our pointer thru the string
    int active = 0; // 0 means we do not think we are reading the seperator. 1 means we think we are reading the seperator
    int change = 0; // 0 means we have not hit the seperator yet are are writing to front. 1 means we have and we are now writing to tail.
    int wait = 0;
    char temp[1]; 
    while(p[pointer] != '\0') {
	if (wait != 0) {
	    wait--;
	    continue;
	}
	if (change == 0) {
	    switch(p[pointer]) {
		// Here we are looking for a ~~~!
		case 126: // ~ character
		    active++;
		    goto seperator;
		    break;
		case 33: // ! character
		    if (active == 3) { // we have three consecutive ~'s, so we have hit the seperator
			change = 1;
			active = 0;
			goto seperator;
		    }
		    else
			active = 0;
		    break;
		default:
		    wait = active;
		    active = 0;
		    break;

	    }
	}
	switch(change) {
	case 1:
	    /*if ((active == 2) && (wait != 3)) { // I don't remember what this is supposed to do
		wait++;
	    }*/
	    //else {
		temp[0] = p[pointer];
		strcat(tail, temp);
	    //}
	    break;
	case 0:
	    temp[0] = p[pointer];
	    strcat(front, temp);
	    break;
	default:
	    break;
	}


seperator:
	pointer++;
    }
}

int start(char *file_name, char *new_name) {
    int size = 0;
    FILE *fp = fopen(file_name, "r"); //file which contains raw data
    FILE *template = fopen(TEMPLATE_FILE, "r"); //file which contains our template
    Input *data = malloc(sizeof(Input)); //our data
    Input *rtemplate = malloc(sizeof(Input)); //whole template
    Input *ftemplate = malloc(sizeof(Input)); //front template (the first part before the seperator)
    Input *ttemplate = malloc(sizeof(Input));//tail template (the tail after the seperator)
    char *temp = NULL;
    // getting size of data file
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    rewind(fp);
    if (allocate_space(data, size + 2) == 1)
	return 2;
    if (!new_name) {
	strcpy(data->file_name, file_name);
    }
    else {
	strcpy(data->file_name, new_name);
	if (!strstr(data->file_name, ".html"))
	    strncat(".html", data->file_name, 12);
    }
    // copying data file to string
    fgets(data->file_contents, size, fp);
    fclose(fp);
    //done with data file
    
    //Getting size of template file
    fseek(template, 0L, SEEK_END);
    size = ftell(template);
    rewind(template);
    if (allocate_space(rtemplate, size) == 1)
	return 2;
    if (allocate_space(ttemplate, size) == 1)
	return 2;
    if (allocate_space(ftemplate, size) == 1)
	return 2;
    temp = malloc(sizeof(char) * size);
    rtemplate->file_name = NULL;
    ttemplate->file_name = NULL;
    ftemplate->file_name = NULL;
    while(fgets(temp, size, template)) {
	strcat(rtemplate->file_contents, temp);
    }
    fclose(template);

    read_template(rtemplate->file_contents, ftemplate->file_contents, ttemplate->file_contents);

    FILE *new = fopen(data->file_name, "a");
    fprintf(new, "%s\n", ftemplate->file_contents);
    fprintf(new, "<p>");
    fprintf(new, "%s", data->file_contents);
    fprintf(new, "</p>\n");
    fprintf(new, "%s", ttemplate->file_contents);
    fclose(new);

    free_input(data);
    free_input(rtemplate);
    free_input(ftemplate);
    free_input(ttemplate);
    }

int main() {
    start("test", "test.html");
}
