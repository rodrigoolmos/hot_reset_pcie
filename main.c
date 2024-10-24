#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#define BYTES_SIZE 128000
#define MAX_DATA BYTES_SIZE/4 //4 bytes word

void write_fpga(int fd, off_t base_addr, const void *data, size_t size) {
    if (pwrite(fd, data, size, base_addr) == -1) {
        perror("pwrite");
        exit(EXIT_FAILURE);
    }
}

void read_fpga(int fd, off_t base_addr, void *buffer, size_t size) {
    if (pread(fd, buffer, size, base_addr) == -1) {
        perror("pread");
        exit(EXIT_FAILURE);
    }
}

int main() {

    uint32_t data_send[MAX_DATA] = {0};
    uint32_t data_recive_lite[MAX_DATA] = {0};
    uint32_t data_recive_full[MAX_DATA] = {0};

    int fd_h2c = open("/dev/xdma0_h2c_0", O_RDWR);
    int fd_user = open("/dev/xdma0_user", O_RDWR);
    int fd_c2h = open("/dev/xdma0_c2h_0", O_RDWR);
    if (fd_h2c == -1){
        perror("open /dev/xdma0_h2c_0");
        return EXIT_FAILURE;
    }    
    if (fd_c2h == -1){
        perror("open /dev/xdma0_c2h_0");
        return EXIT_FAILURE;
    }
    if (fd_user == -1){
        perror("open /dev/xdma0_user");
        return EXIT_FAILURE;
    }

    clock_t start_lite = clock();
    for (int i = 0; i < MAX_DATA; i++){ // test write lite
        data_send[i] = i;
        write_fpga(fd_user, 0x0 + i*4, &data_send[i], 4);
    }
    for (int i = 0; i < MAX_DATA; i++){ // test read lite
        read_fpga(fd_user, 0x0 + i*4, &data_recive_lite[i], 4);
    }
    clock_t end_lite = clock();

    clock_t start_full = clock();
    write_fpga(fd_h2c, 0xC0000000, data_send, 4 * MAX_DATA); // test write full
    read_fpga(fd_c2h, 0xC0000000, data_recive_full, 4 * MAX_DATA); // test read full
    clock_t end_full = clock();

    for (int i = 0; i < MAX_DATA; i++){ // test read lite
        if (data_send[i] != data_recive_full[i] || data_send[i] != data_recive_lite[i]){
            printf("Error read wirte FPGA!!\n");
            return -1;
            close(fd_h2c);
            close(fd_c2h);
            close(fd_user);
        }
        
    }

    printf("Read write OK\n");
    printf("Time read write lite %f full %f\n",((double)(end_lite - start_lite)) / CLOCKS_PER_SEC,
                                             ((double)(end_full - start_full)) / CLOCKS_PER_SEC);

    close(fd_h2c);
    close(fd_c2h);
    close(fd_user);


    return 0;
}
