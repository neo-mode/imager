//
//  main.c
//  imager
//
//  Created by Neo Mode on 11.11.2021.
//

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIND_ASSETS 1
#define FIND_IMAGESET 2
#define SIZE 100

typedef struct name {
	char key[SIZE];
	char val[SIZE];
	struct name *next;
} name_t;

name_t *head;
name_t *first;

void find_dir(const char *dirname, short mode);
void create_image(const char *filename);

int main() {

	find_dir(".", FIND_ASSETS);

	if (!first) {
		fprintf(stderr, "No one image found. Cancelled\n");
		return 1;
	}

	printf("import UIKit\n\nenum ImageName: String {\n");

	name_t *prev, *node = first;
	while (node) {

		if (strcmp(node->key, node->val) == 0)
			printf("\tcase %s\n", node->key);
		else
			printf("\tcase %s = \"%s\"\n", node->key, node->val);

		prev = node;
		node = node->next;
		free(prev);
	}

	printf("}\n\nextension UIImage {\n\tstatic func image(name: ImageName) -> UIImage {\n\t\tguard let image = UIImage(named: name.rawValue) else { assertionFailure(\"Image not found\"); return UIImage() }\n\t\treturn image\n\t}\n}\n");
}

void find_dir(const char *dirname, short mode) {

	DIR *dir = opendir(dirname);
	if (!dir) {
		fprintf(stderr, "Could not open directory: %s\n", dirname);
		exit(1);
	}

	struct dirent *file;
	while ((file = readdir(dir))) {

		if (file->d_type != DT_DIR || file->d_name[0] == '.') continue;

		const char *ext = strrchr(file->d_name, '.');
		if (ext && strcmp(ext, ".imageset") == 0) {
			create_image(file->d_name);
			continue;
		}

		if (!ext || strcmp(ext, ".xcassets") == 0) {
			char path[strlen(dirname) + strlen(file->d_name) + 2];
			sprintf(path, "%s/%s", dirname, file->d_name);
			find_dir(path, ext ? FIND_IMAGESET : mode);
		}
	}

	closedir(dir);
}

void create_image(const char *filename) {

	size_t len = strlen(filename) - 9;
	size_t total = len + 1;

	char ext[total];
	memcpy(ext, filename, len);
	ext[len] = 0;

	char key[SIZE];
	key[0] = 0;

	char value[SIZE];
	memcpy(value, filename, len);
	value[len] = 0;

	char *part = strtok(ext, " _-");
	part[0] |= 32;

	for (;;) {
		strcat(key, part);
		part = strtok(NULL, " _-");
		if (!part) break;
		part[0] &= ~32;
	}

	name_t *node = malloc(sizeof(name_t));
	strcpy(node->key, key);
	strcpy(node->val, value);
	node->next = NULL;

	if (first)
		head->next = node;
	else
		first = node;

	head = node;
}
