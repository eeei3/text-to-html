#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static char TEMPLATE_FILE[40] = "testtemplate";


typedef struct input {
    char *file_contents;
    char *file_name;
    int file_size;
} Input;

void allocate_space(Input* p, int size) {
    p->file_contents = malloc(size * sizeof(char));
    p->file_name = malloc(255 * sizeof(char));
    p->file_size = size;
    return;
}

void free_input(Input* data) {
    free(data->file_contents);
    free(data->file_name);
    free(data);
    return;
}

void read_template(char* p, char* front, char* tail) {
    int pointer = 0;
    int active = 0;
    int change = 0;
    int wait = 0;
    while(p[pointer] != '\0') {
	if (change == 0) {
	    switch(p[pointer]) {
		// Here we are looking for a ~~~!
		case 126: // ~ character
		    active++;
		    goto suspect;
		    break;
		case 33: // ! character
		    if (active == 2)
		    {
			goto suspect;
		    }
		    else
			active = 0;
		    break;
		default:
		    goto suspect;
		    active = 0;
		    break;

	    }
	}
	switch(change) {
	case 1:
	    if ((active == 2) && (wait != 3)) {
		wait++;
	    }
	    else {
		strcat(tail, &p[pointer]);
	    }
	    break;
	case 0:
	    strcat(front, &p[pointer]);
	    break;
	default:
	    break;
	}

    suspect:
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

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    rewind(fp);
    
    allocate_space(data, size + 2);
    if (!new_name) {
	strcpy(data->file_name, file_name);
    }
    else {
	strcpy(data->file_name, new_name);
    }
    fgets(data->file_contents, size, fp);
    fclose(fp);

    fseek(template, 0L, SEEK_END);
    size = ftell(fp);
    rewind(template);
    allocate_space(rtemplate, size);
    allocate_space(ttemplate, size);
    allocate_space(ftemplate, size);
    rtemplate->file_name = NULL;
    ttemplate->file_name = NULL;
    ftemplate->file_name = NULL;
    fgets(rtemplate->file_contents, size, template);
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
