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
#define allowed_spec_chr(chr) (chr == '-' || chr == '.' || chr == '_')

typedef struct name {
	char key[64], val[64];
	struct name *next;
} name_t;

name_t *head = 0, *first = 0;

void find_dir(const char*, short, short);
void create_image(const char*, short);
char check_chr(const char*, short);

int main(int argc, const char **argv) {

	short is_public = 0;
	const char *arg = 0;

	for (int i = 0; i < argc; i++) {
		arg = argv[i];
		if (arg[0] == '-' && arg[1] == 'p' && arg[2] == 0) {
			is_public = 1;
			continue;
		}
	}

	find_dir(".", 1, FIND_ASSETS);

	if (!first) {
		fprintf(stderr, "No one image found. Cancelled\n");
		return 1;
	}

	printf("import UIKit\n\n%senum ImageName: String {\n", is_public ? "public " : "");

	for (name_t *node = first; node; node = node->next)
		if (node->key[0])
			printf("\tcase %s = \"%s\"\n", node->key, node->val);
		else
			printf("\tcase %s\n", node->val);

	printf("}\n\n%sextension UIImage {\n\tstatic func image(name: ImageName) -> UIImage {\n\t\tif let image = UIImage(named: name.rawValue) { return image }\n\t\tassertionFailure(\"Image \\(name.rawValue) not found\")\n\t\treturn UIImage()\n\t}\n}\n", is_public ? "public " : "");
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

	char chr = check_chr(filename, 0);
	char toggle = -1;
	short i, j;

	name_t *node = malloc(sizeof(name_t));
	node->key[0] = chr | 32;
	node->val[0] = chr;

	for (i = 1, j = 1, size -= 9; i < size; i++) {

		node->val[i] = chr = check_chr(filename, i);
		if (allowed_spec_chr(chr)) { toggle = -33; continue; }

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

char check_chr(const char *filename, short pos) {

	char chr = filename[pos];
	if (allowed_spec_chr(chr) || (chr >= '0' && chr <= '9') || (chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z')) return chr;

	fprintf(stderr, "%s has denied character\n", filename);
	exit(1);
}
