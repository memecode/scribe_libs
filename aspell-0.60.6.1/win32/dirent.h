#ifndef _DIR_ENT_H_
#define _DIR_ENT_H_

struct DIR;

struct dirent
{
    int            d_ino;       /* inode number */
    size_t         d_off;       /* offset to the next dirent */
    unsigned short d_reclen;    /* length of this record */
    unsigned char  d_type;      /* type of file; not supported
                                   by all file system types */
    char           d_name[256]; /* filename */
};

int            closedir(DIR *);
DIR           *opendir(const char *);
struct dirent *readdir(DIR *);

#endif