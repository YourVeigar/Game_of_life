#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define height 25
#define width 80

void update(int **matrix);
void conclusion(int **matrix, int speed);
int check_around(int **matrix, int row, int col);
void get_speed(int *speed);
int getch(void);
void input(int **matrix);
void clear_memory(int **matrix);
int check_life(int **matrix);

int main() {
    int **matrix = malloc(height * sizeof(int *));
    int speed = 250000;
    int flag_for_game = 1;

    input(matrix);
    FILE *fp;
    fp = freopen("/dev/tty", "r", stdin);
    if (fp == NULL) {
        printf("Fine cant be open");
        exit(1);
    }
    conclusion(matrix, speed);
    while (flag_for_game) {
        if (check_life(matrix)) {
            printf("Ты сдох");
            flag_for_game = 0;
            continue;
        }
        get_speed(&speed);
        if (speed == -1) {
            flag_for_game = 0;
            continue;
        }
        update(matrix);
        conclusion(matrix, speed);
        usleep(speed);
    }
    clear_memory(matrix);
}

void input(int **matrix) {
    for (int i = 0; i < height; i++) {
        matrix[i] = malloc(width * sizeof(int));
    }
    FILE *file = stdin;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fscanf(file, "%d", &matrix[i][j]);
        }
    }
    fclose(file);
}
int check_around(int **matrix, int row, int col) {
    int count = 0, nj, ni;
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            ni = row + i;
            nj = col + j;
            if (i == 0 && j == 0) continue;
            if (ni < 0) ni += height;
            if (ni >= height) ni -= height;
            if (nj < 0) nj += width;
            if (nj >= width) nj -= width;
            count += matrix[ni][nj];
        }
    }
    return count;
}

void update(int **matrix) {
    int clear_matrix[height][width];

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int neighbors = check_around(matrix, i, j);

            if (matrix[i][j]) {
                clear_matrix[i][j] = (neighbors == 2 || neighbors == 3);
            } else {
                clear_matrix[i][j] = (neighbors == 3);
            }
        }
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            matrix[i][j] = clear_matrix[i][j];
        }
    }
}

int check_life(int **matrix) {
    int flag = 1;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (matrix[i][j] == 1) flag = 0;
        }
    }
    return flag;
}

void conclusion(int **matrix, int speed) {
    printf("Press q to exit.");
    printf("Current delay: %d\n", speed);
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (matrix[row][col] == 1) {
                printf("*");
            } else {
                printf("-");
            }
        }
        printf("\n");
    }
}

void get_speed(int *speed) {
    char key = getch();

    switch (key) {
        case '-':
            (*speed < 500000) ? *speed += 50000 : *speed;
            break;
        case '+':
            (*speed > 0) ? *speed -= 50000 : *speed;
            break;
        case 'q':
            *speed = -1;
            break;
    }
}

int getch() {
    struct termios old, new;
    int ch;
    fd_set fds;
    struct timeval tv;

    tcgetattr(0, &old);
    new = old;
    new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &new);

    FD_ZERO(&fds);
    FD_SET(0, &fds);

    tv.tv_sec = 0;
    tv.tv_usec = 10000;

    select(1, &fds, NULL, NULL, &tv);

    if (FD_ISSET(0, &fds)) {
        ch = getchar();
    } else {
        ch = -1;
    }

    tcsetattr(0, TCSANOW, &old);

    return ch;
}

void clear_memory(int **matrix) {
    for (int i = 0; i < height; i++) {
        free(matrix[i]);
    }
    free(matrix);
}
