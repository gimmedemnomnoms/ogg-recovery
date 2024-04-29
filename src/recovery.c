#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include "__MBR__.h"
#include "__superblock__.h"
#include "__shortblock__.h"
#include "__longblock__.h"
#define BLOCK_SIZE 4096

uint32_t getPartAddr(int fd) {
    //read in mbr and store in struct    
    struct Disk_MBR mbr;
    ssize_t bytesRead = read(fd, &mbr, sizeof(mbr));
    //error if read device fails
    if (bytesRead == -1) {
        fprintf(stderr, "Failed to read device.\n");
        close(fd);
        return -1;
    }
    //return address of lba begin
    uint32_t addr = mbr.partitions[0].lbaBegin * 512;
    return addr;
}
// stores superblock into struct, needed to calculate block number
uint32_t sbinfo(int fd, uint32_t offset){
    struct superblock sb;
    ssize_t bytesRead = read(fd, &sb, sizeof(sb));
    offset = offset + sizeof(sb);
    if (bytesRead == -1) {
        fprintf(stderr, "Failed to read device.\n");
    }
    return offset;
}

void oggblocks (int fd, int fd2, uint32_t offset) {
    struct shortblock devblock;
    struct longblock contents;
    ssize_t bytesRead;
    uint32_t count = 0;
    offset = 0;
    int works = 0;
    int visited = 0;
    int tempPlace = 0;
    FILE *ftemp_1 = fopen("recovered.ogg" ,"a");
    fprintf(ftemp_1, "%d", 0);
    fclose(ftemp_1);
    FILE *blockslog = fopen("blocks.txt" ,"a");
    int fptr = open("recovered.ogg", O_WRONLY);
    lseek(fd, 0, SEEK_SET);
    lseek(fd2, 0, SEEK_SET);
    // read device one block at a time
    while ((bytesRead = read(fd, &devblock, sizeof(devblock))) > 0) {
        offset = offset + bytesRead;
            // if block contains monotonically increasing data, it is an indirect block
            if (devblock.devdata[0] == devblock.devdata[1]-1 && devblock.devdata[1] == devblock.devdata[2]-1 && devblock.devdata[2] == devblock.devdata[3]-1) {
                works = 0;
                if(count == 0){
                    // if first indirect, use address stored in the first index to find addresses of first 12 direct blocks
                    for(int j=0; j<12 && works==0;j++){
                        tempPlace = devblock.devdata[0]-12+j;
                        tempPlace = tempPlace * 4096;
                        // seek to block address and read data into struct for block
                        lseek(fd2, tempPlace,SEEK_SET);
                        read(fd2, &contents, sizeof(contents));
                        // print addresses in blocks.txt
                        fprintf(blockslog, "%d\t", contents.contblock[0]);
                        // write data to recovered.ogg
                        if(contents.contblock[0] == 79 && j==0 && works == 0 || j !=0 && works == 0){
                            visited = devblock.devdata[0]-12;
                            write(fptr, contents.contblock, sizeof(contents));
                            count = 1;
                        }
                        else{
                            works = 1;  
                            j=12;                      
                        }
                    } // recover first indirect block
                    for(int i = 0; i < 1024 && works==0; i++){
                        tempPlace = devblock.devdata[i];
                        if(tempPlace == 0){
                            break;
                        }
                        // obtain block address, multiply by block size to seek to start of block
                        tempPlace = tempPlace * 4096;
                        lseek(fd2, tempPlace,SEEK_SET);
                        // read contents of block into struct
                        read(fd2, &contents, sizeof(contents));
                        // write struct to recovered.ogg
                        write(fptr, contents.contblock, sizeof(contents));
                    }
                    //print addresses stored in first indirect block
                    if(works == 0){
                        fprintf(blockslog, "1\t%d", offset/4096);
                        for(int i=0; i<1024;i++){
                            if(i%32==0){fprintf(blockslog, "[");}
                            fprintf(blockslog, "%x", devblock.devdata[i]);
                            if(i%32!=31){fprintf(blockslog, ", ");}
                            else{fprintf(blockslog, "]\n");}
                        }
                        fprintf(blockslog, "\n\n");
                    }
                }
                // second indirect
                else if(devblock.devdata[0]-12 != visited && count == 1){
                    count = count + 1;
                    // process each datablock corresponding to address stored in indirect
                    for(int i = 0; i < 1024; i++){
                        tempPlace = devblock.devdata[i];
                        if(tempPlace == 0){
                            break;
                        }
                        //seek to start of block
                        tempPlace = tempPlace * 4096;
                        lseek(fd2, tempPlace,SEEK_SET);
                        // read contents of block into struct
                        read(fd2, &contents, sizeof(contents));
                        // write struct to recovered file
                        write(fptr, contents.contblock, sizeof(contents));
                    }
                        // print addresses to blocks.txt
                        fprintf(blockslog, "2\t%d", offset/4096);
                        for(int ike=0; ike<1024;ike++){
                            if(ike%32==0){fprintf(blockslog, "[");}
                            fprintf(blockslog, "%x", devblock.devdata[ike]);
                            if(ike%32!=31){fprintf(blockslog, ", ");}
                            else{fprintf(blockslog, "]\n");}
                        }
                        fprintf(blockslog, "\n\n");
                }
            }
    }
    fclose(blockslog);
    close(fptr);
}
int main(int argc, char *argv[]) {
    //open device, prints error if cannot open
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <device_name>\n", argv[0]);
        return 1;
    }
    int fd = open(argv[1], O_RDONLY);
    int fd2 = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error opening device");
        return 1;
    }
    //returns the address for the beginning of the partition
    uint32_t partitionAddr = getPartAddr(fd);
    //maintain offset to calculate block number
    uint32_t offset = partitionAddr + 1024;
    //seek to beginning of partition
    lseek(fd, (offset), SEEK_SET);
    //read superblock
    offset = sbinfo(fd, offset);
    //function for recovery
    oggblocks(fd, fd2, offset);
    close(fd);
    return 0;
}
