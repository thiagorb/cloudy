#include <stdlib.h>
#include <string.h>

int bytestream_read_int(char **stream) {
    int ret = *(int *)*stream;
    *stream += sizeof(int);
    return ret;
}

int bytestream_read_long(char **stream) {
    long ret = *(long *)*stream;
    *stream += sizeof(long);
    return ret;
}

char *bytestream_read_string(char **stream) {
    int length = bytestream_read_int(stream);
    char *ret = (char *)malloc(sizeof(char) * (length + 1));
    memcpy(ret, *stream, length);
    ret[length] = 0;
    *stream += length;
    return ret;
}

void bytestream_write_int(char **stream, int i) {
    *(int *)*stream = i;
    *stream += sizeof(int);
}

void bytestream_write_string(char **stream, char *str) {
    int length;
    length = strlen(str);
    bytestream_write_int(stream, length);
    memcpy(*stream, str, length);
    *stream += length;
}