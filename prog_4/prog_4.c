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