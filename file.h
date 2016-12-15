typedef struct file {
    char *name;
    FILE *handle;
    unsigned int lineno;
    unsigned int colno;
} File;