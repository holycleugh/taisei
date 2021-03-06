/*
 * This software is licensed under the terms of the MIT License.
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2019, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2019, Andrei Alexeyev <akari@taisei-project.org>.
 */

#include "taisei.h"

#include "private.h"
#include "syspath.h"

char* vfs_path_normalize(const char *path, char *out) {
	const char *p = path;
	char *o = out;
	char *last_sep = out - 1;
	char *path_end = strchr(path, 0);

	while(p < path_end) {
		if(VFS_IS_PATH_SEPARATOR(*p)) {
			if(o > out && !VFS_IS_PATH_SEPARATOR(o[-1])) {
				last_sep = o;
				*o++ = VFS_PATH_SEPARATOR;
			}

			do {
				++p;
			} while(p < path_end && VFS_IS_PATH_SEPARATOR(*p));
		} else if(*p == '.' && VFS_IS_PATH_SEPARATOR(p[1])) {
			p += 2;
		} else if(!strncmp(p, "..", 2) && VFS_IS_PATH_SEPARATOR(p[2])) {
			if(last_sep >= out) {
				do {
					--last_sep;
				} while(!VFS_IS_PATH_SEPARATOR(*last_sep) && last_sep >= out);

				o = last_sep-- + 1;
			}

			p += 3;
		} else {
			*o++ = *p++;
		}
	}

	*o = 0;

	// log_debug("[%s] --> [%s]", path, out);
	return out;
}

char* vfs_path_normalize_alloc(const char *path) {
	return vfs_path_normalize(path, strdup(path));
}

char* vfs_path_normalize_inplace(char *path) {
	char buf[strlen(path)+1];
	strcpy(buf, path);
	vfs_path_normalize(path, buf);
	strcpy(path, buf);
	return path;
}

void vfs_path_split_left(char *path, char **lpath, char **rpath) {
	char *sep;

	while(VFS_IS_PATH_SEPARATOR(*path))
		++path;

	*lpath = path;

	if((sep = strchr(path, VFS_PATH_SEPARATOR))) {
		*sep = 0;
		*rpath = sep + 1;
	} else {
		*rpath = path + strlen(path);
	}
}

void vfs_path_split_right(char *path, char **lpath, char **rpath) {
	char *sep, *c;
	assert(*path != 0);

	while(VFS_IS_PATH_SEPARATOR(*(c = strrchr(path, 0) - 1)))
		*c = 0;

	if((sep = strrchr(path, VFS_PATH_SEPARATOR))) {
		*sep = 0;
		*lpath = path;
		*rpath = sep + 1;
	} else {
		*lpath = path + strlen(path);
		*rpath = path;
	}
}

void vfs_path_resolve_relative(char *buf, size_t bufsize, const char *basepath, const char *relpath) {
	assert(bufsize >= strlen(basepath) + strlen(relpath) + 1);

	if(!*basepath) {
		strcpy(buf, relpath);
		return;
	}

	strcpy(buf, basepath);
	char *end = strrchr(buf, 0);

	while(end > buf) {
		if(VFS_IS_PATH_SEPARATOR(end[-1])) {
			break;
		}

		--end;
	}

	strcpy(end, relpath);
}

void vfs_path_root_prefix(char *path) {
	if(!VFS_IS_PATH_SEPARATOR(*path)) {
		memmove(path+1, path, strlen(path)+1);
		*path = VFS_PATH_SEPARATOR;
	}
}

char* vfs_syspath_normalize_inplace(char *path) {
	char buf[strlen(path)+1];
	strcpy(buf, path);
	vfs_syspath_normalize(buf, sizeof(buf), path);
	strcpy(path, buf);
	return path;
}
