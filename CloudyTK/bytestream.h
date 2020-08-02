#ifndef BYTESTREAM_H
#define	BYTESTREAM_H

#ifdef	__cplusplus
extern "C" {
#endif

int bytestream_read_int(char **stream);
long bytestream_read_long(char **stream);
char *bytestream_read_string(char **stream);
void bytestream_write_int(char **stream, int i);
void bytestream_write_string(char **stream, char *str);

#ifdef	__cplusplus
}
#endif

#endif

