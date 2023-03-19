# Отчет о выполнении задания
## Демьяненко Виктор Николаевич 
## Группа: БПИ217
## Вариант 11.

### Условие задачи:
``` ASCII-строка — строка, содержащая символы таблицы кодировки ASCII. (https://ru.wikipedia.org/wiki/ASCII). Размер строки может быть достаточно большим, чтобы вмещать многостраничные тексты, например, главы из книг, если задача связана с использованием файлов или строк, порождаемых генератором случайных чисел. Тексты при этом могут не нести смыслового содержания. Для обработки в программе предлагается использовать данные, содержащие символы только из первой половины таблицы (коды в диапазоне 0–12710), что связано с использованием кодировки UTF-8 в ОС Linux в качестве основной. Символы, содержащие коды выше 12710, должны отсутствовать во входных данных кроме оговоренных специально случаев. ```

### Условие варианта:
```Разработать программу вычисления отдельно количества гласных и согласных букв в ASCII-строке. ```

## Код выполнен на 4-8 балла.
Все условия выполнения предоставлены в файле требований.

## Общий код, выполняющий основную задачу:

``` C
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>


void count_procedure(char *str, int *data) {
     int len = strlen(str);
    for (int i = 0; i < len; i++) {
        char c = tolower(str[i]);
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') {
            (data[0])++;
        } else if (isalpha(c)) {
            (data[1])++;
        }
    }
}
const int MEMORY = 5000;

int main(int argc, char *argv[]) {
    char str_buf[MEMORY];
    for (int i = 0; i < MEMORY; i++) {
        str_buf[i] = 0;
    }
    if(argc != 3){
        printf("./run.out read.txt write.txt\n");
        return 0;
    }

    int fd1[2],fd2[2], result, size;

    int data[2] = {0, 0};
    // создаем первый канал и второй канал 
    if(pipe(fd1) < 0 || pipe(fd2)) { 
        printf("Can\'t open pipe\n");
        exit(-1);
    }
    
    // Первый и второй процесс
    result = fork(); 

    if(result < 0) {
        printf("Can\'t fork child\n");
        exit(-1);
    } else if (result > 0) { // Вход первого процесса
        if(close(fd2[0]) < 0){
            printf("parent: Can\'t close reading side of pipe\n"); exit(-1);
        }

        int file_to_read = 0;
        
        if((file_to_read = open(argv[1], O_RDONLY , 0666)) < 0) {
            printf("Can\'t open file\n");
            exit(-1);
        }

        // Считываем данные из файла
        size = read(file_to_read, str_buf, MEMORY); 

        // Закрываем файл
        if(close(file_to_read) < 0) {
            printf("Can\'t close file\n");
        }

        // Запись в первый канал
        size = write(fd2[1], str_buf, MEMORY); 

        // Проверка на корректность
        if(size != MEMORY){
            printf("Can\'t write all string to pipe\n (size = %d)",size);
            exit(-1);
        }

        // Закрываем канал
        if(close(fd2[1]) < 0) {
            printf("parent: Can\'t close writing side of pipe\n");
            exit(-1);
        }
    } else{  // Вход во второй процесс
        // Разделяем второй процесс еще на два
        int result1 = fork(); 

        // Если не удалось разделить, то выводим ошибку
        if(result1 < 0) {
            printf("Can\'t fork child\n");
            exit(-1);
        } else if(result1 > 0){ // Вход во второй процесс
            if(close(fd2[1]) < 0){
                printf("child: Can\'t close writing side of pipe\n"); exit(-1);
            }
            // Считываем из первого канала
            size = read(fd2[0], str_buf, MEMORY); 
            if(size < 0){
                printf("Can\'t read string from pipe\n");
                exit(-1);
            }

            // Подсчитываем кол-во гласных и согласных букв
            count_procedure(str_buf, data); 

            // Очищаем строку
            for (int i = 0; i < MEMORY; i++) {
                str_buf[i] = 0;
            }

            // Заполняем строку полученными значениями из функции
            sprintf(str_buf, "%d %d", data[0], data[1]);
            if(close(fd2[0]) < 0){
                printf("child: Can\'t close reading side of pipe\n"); exit(-1);
            }
            
            size = write(fd1[1], str_buf, MEMORY); // записываем во второй канал
            
            if(size != MEMORY){
                printf("Can\'t write all string to pipe\n (size = %d)",size);
                exit(-1);
            }
            if(close(fd1[1]) < 0) {
                printf("parent: Can\'t close writing side of pipe\n");
                exit(-1);
            }
        } else{ // Третий процесс
            if(close(fd1[1]) < 0){
                printf("child: Can\'t close writing side of pipe\n");
                exit(-1);
            }
            
            // Считываем из второго канала
            size = read(fd1[0], str_buf, MEMORY);

            if(size < 0){
                printf("Can\'t read string from pipe\n");
                exit(-1);
            }
            if(close(fd1[0]) < 0){
                printf("child: Can\'t close reading side of pipe\n"); exit(-1);
            }
            int file_to_write = 0;
            
            if((file_to_write = open(argv[2], O_WRONLY | O_CREAT, 0666)) < 0) {
                printf("Can\'t open file\n");
                exit(-1);
            }
            printf("%s \n", str_buf);
            // Записываем в файл.
            size = write(file_to_write, str_buf, strlen(str_buf)); 
            if(size != strlen(str_buf)) {
                printf("Can\'t write all string\n");
                exit(-1);
            }
            if(close(file_to_write) < 0) {
                printf("Can\'t close file\n");
            }
        }
    }
    return 0;
}
```

## Тесты, использованные для првоерки корректности кода:

- read1.txt

``` Hello! ``` -> 2 3

- read2.txt

``` aa bb ``` -> 2 2

- read3.txt

``` aaaaaa12341234123412431234bb ``` -> 6 2

- read4.txt

``` One two three four. Then we go to the. ``` -> 12 16

- read5.txt

``` I like you! ``` -> 5 3

- read6.txt

``` >/./,././././~!$%^&*@*#@*#*@*#@*# ``` -> 0 0

## Результат на 8 баллов:

```c
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
```

Код был немного изменен, однако всё корректно работает и выводит.