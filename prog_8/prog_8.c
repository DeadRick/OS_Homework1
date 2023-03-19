#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

const int MEMORY = 5000;

const char *name1 = "pipe1.fifo";
const char *name2 = "pipe2.fifo";

void count_vowels_and_consonants(char string[], int *vowels, int *consonants) {
  *vowels = 0;
  *consonants = 0;
  int size = strlen(string);
  for (int i = 0; i < size; ++i) {
    char ch = string[i];
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
      if (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u' ||
          ch == 'A' || ch == 'E' || ch == 'I' || ch == 'O' || ch == 'U') {
        *vowels += 1;
      } else {
        *consonants += 1;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  int fd1, fd2, size;
  char str_buf[MEMORY];
  memset(str_buf, 0, MEMORY);
  mknod(name1, S_IFIFO | 0666, 0);
  mknod(name2, S_IFIFO | 0666, 0);
  fd2 = 0;

  if ((fd1 = open(name1, O_RDONLY)) < 0) {
    printf("Can\'t open FIFO for reading\n");
    exit(-1);
  }

  size = read(fd1, str_buf, MEMORY);

  if (size < 0) {
    printf("Can\'t read string from FIFO\n");
    exit(-1);
  }
  int vowels, consonants;
  count_vowels_and_consonants(str_buf, &vowels, &consonants);
  printf("Number of vowels: %d\n", vowels);
  printf("Number of consonants: %d\n", consonants);

  if (close(fd1) < 0) {
    printf("child: Can\'t close FIFO\n");
    exit(-1);
  }

  if ((fd2 = open(name2, O_WRONLY)) < 0) {
    printf("Can\'t open FIFO for writting\n");
    exit(-1);
  }

  size = write(fd2, str_buf, MEMORY);

  if (size != MEMORY) {
    printf("Can\'t write all string to FIFO\n (size = %d)", size);
    exit(-1);
  }

  if (close(fd2) < 0) {
    printf("parent: Can\'t close FIFO\n");
    exit(-1);
  }

  return 0;
}