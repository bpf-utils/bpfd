/*
 * BPFd (Berkeley Packet Filter daemon)
 * Common utility/helper functions.
 *
 * Copyright (C) 2017 Joel Fernandes <agnel.joel@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "utils.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Read a file on the local fs to stdout */
int cat_file(char *path) {
	char buf[4096];
	int len, fd;

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		printf("Open failed, ignoring\n");
		return fd;
	}

	while ((len = read(fd, &buf, 4096)) > 0)
		write(1, buf, len);

	close(fd);

	return 0;

}

/* Read a tracefs file to stdout */
int cat_tracefs_file(char *tracefs, char *fn) {
	char tracef[100];

	tracef[0] = 0;
	strcat(tracef, tracefs);
	strcat(tracef, "/");
	strcat(tracef, fn);

	return cat_file(tracef);
}

int cat_dir(char *path, int dirs_only)
{
	DIR *dp;
	struct dirent *ep;

	dp = opendir(path);
	if (!dp)
		return -1;

	while (ep = readdir(dp)) {
		struct stat st;

		if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0)
			continue;

		if (dirs_only) {
			if (fstatat(dirfd(dp), ep->d_name, &st, 0) < 0)
				continue;

			if (!S_ISDIR(st.st_mode))
				continue;
		}

		printf("%s\n", ep->d_name);
	}
	closedir (dp);

	return 0;
}
