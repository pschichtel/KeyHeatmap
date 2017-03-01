#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <values.h>
#include <time.h>
#include "util.h"

#ifdef _WIN32
#define PS "\\"
#else
#define PS "/"
#endif

#define MATRIX_WIDTH 24
#define MATRIX_HEIGHT 6

bool shutdown = false;


void handleSignal(int signal) {
    shutdown = true;
    printf("Received signal %s\n", strsignal(signal));
}

struct matrix_info {
    int* matrix;
    int size;
    int min;
    int max;
};

void* check_loop(void* args) {
    srand(time(NULL));
    struct matrix_info* info = args;

    struct hsv_color hsv = {0.0f, 1.0f, 1.0f};

    for (float i = 0; i < 360; ++i) {
        hsv.hue = i;
        struct rgb_color* rgb = hsv_to_rgb(&hsv);

        printf("RGB at angle %f: r=%f g=%f b=%f\n", i, rgb->red, rgb->green, rgb->blue);

        free(rgb);
    }


    while (!shutdown) {
        long offset = random_at_most(info->size);
        int value = (int)random_at_most(MAXINT);
        printf("Mutated %li, new value: %d\n", offset, value);
        info->matrix[offset] = value;
        usleep(1000000);
    }
    return NULL;
}

int main() {
    int matrixSize = MATRIX_WIDTH * MATRIX_HEIGHT;
    size_t bufferSize = sizeof(int) * matrixSize;

    char *homeDir = getenv("HOME");
    if (homeDir == NULL) {
        homeDir = ".";
    }
    char *filePath = concat(concat(homeDir, PS), "test.dat");
    int fd = open(filePath, O_RDWR | O_CREAT | O_SYNC);
    if (fd < 0) {
        printf("Failed to open file: %s! Error: %s (%d)\n", filePath, strerror(errno), errno);
        return errno;
    }
    printf("Opened file %s!\n", filePath);

    off_t size = lseek(fd, 0, SEEK_END);
    if (size == 0) {
        char buf[bufferSize];
        memset(&buf, 0, bufferSize);
        write(fd, &buf, bufferSize);
        lseek(fd, 0, SEEK_SET);
        fchmod(fd, 0644);
    }


    int *keyMatrix = (int *) mmap(NULL, bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (keyMatrix == MAP_FAILED) {
        printf("Failed to map file into memory: %s! Error: %s (%d)\n", filePath, strerror(errno), errno);
        return errno;
    }
    printf("address: %p\n", keyMatrix);

    for (int i = 0; i < matrixSize; ++i) {
        printf("num at %d: %d\n", i, keyMatrix[i]);
        keyMatrix[i] = i;
    }

    struct matrix_info info = {keyMatrix, matrixSize, MAXINT, MININT};
    for (int i = 0; i < info.size; ++i) {
        int val = info.matrix[i];
        if (val < info.min) {
            info.min = val;
        }
        if (val > info.max) {
            info.max = val;
        }
    }
    pthread_t thread;
    pthread_create(&thread, NULL, check_loop, &info);

    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);
    pause();

    pthread_join(thread, NULL);

    msync(keyMatrix, bufferSize, MS_SYNC);
    munmap(keyMatrix, bufferSize);
    close(fd);
    free(filePath);

    return 0;
}