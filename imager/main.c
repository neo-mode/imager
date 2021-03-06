//
//  main.c
//  imager
//
//  Created by Neo Mode on 11.11.2021.
//

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

#define FIND_ASSETS 1
#define FIND_IMAGESET 2

typedef struct name {
	char key[100], val[100];
	struct name *next;
} name_t;

name_t *head;
name_t *first;

void find_dir(const char*, short, short);
void create_image(const char*, short);

int main() {

	find_dir(".", 1, FIND_ASSETS);

	if (!first) {
		fprintf(stderr, "No one image found. Cancelled\n");
		return 1;
	}

	printf("import UIKit\n\nenum ImageName: String {\n");

	for (name_t *node = first; node; node = node->next)
		if (node->key[0] != 0)
			printf("\tcase %s = \"%s\"\n", node->key, node->val);
		else
			printf("\tcase %s\n", node->val);

	printf("}\n\nextension UIImage {\n\tstatic func image(name: ImageName) -> UIImage {\n\t\tif let image = UIImage(named: name.rawValue) { return image }\n\t\tassertionFailure(\"Image not found\")\n\t\treturn UIImage()\n\t}\n}\n");
}

void find_dir(const char *dirname, short size, short mode) {

	DIR *dir = opendir(dirname);
	if (!dir) {
		fprintf(stderr, "Could not open directory: %s\n", dirname);
		exit(1);
	}

	short length;
	const char *ext;
	struct dirent *file;

	while ((file = readdir(dir))) {

		if (file->d_type != DT_DIR || file->d_name[0] == '.') continue;

		length = 0; ext = 0;
		for (const char *name = file->d_name; *name; name++) {
			length++;
			if (*name == '.') ext = name;
		}

		if (ext && ext[0] == '.' && ext[1] == 'i' && ext[2] == 'm' && ext[3] == 'a' && ext[4] == 'g' && ext[5] == 'e' && ext[6] == 's' && ext[7] == 'e' && ext[8] == 't' && ext[9] == 0) {
			create_image(file->d_name, length);
			continue;
		}

		if (!ext || (ext[0] == '.' && ext[1] == 'x' && ext[2] == 'c' && ext[3] == 'a' && ext[4] == 's' && ext[5] == 's' && ext[6] == 'e' && ext[7] == 't' && ext[8] == 's' && ext[9] == 0)) {
			short total = size + length + 1;
			char path[total + 1];
			sprintf(path, "%s/%s", dirname, file->d_name);
			find_dir(path, total, ext ? FIND_IMAGESET : mode);
		}
	}

	closedir(dir);
}

void create_image(const char *filename, short size) {

	size -= 9;

	char chr, toggle = -1;
	short i, j;
	name_t *node = malloc(sizeof(name_t));

	chr = node->val[0] = filename[0];
	node->key[0] = chr | 32;

	for (i = 1, j = 1; i < size; i++) {

		chr = node->val[i] = filename[i];
		if (chr == '-' || chr == '_' || chr == ' ' || chr == '(' || chr == ')') {
			toggle = -33; continue;
		}

		node->key[j++] = chr & toggle;
		toggle = -1;
	}

	node->key[i == j ? 0 : j] = 0;
	node->val[i] = 0;
	node->next = 0;

	if (first)
		head->next = node;
	else
		first = node;

	head = node;
}
