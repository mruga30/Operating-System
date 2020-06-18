/**
 * Mruga shah
 * CS 543 - HW3 - Operating Systems
 * 
 * Virtual memory manager is built for this assignment
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define maskadrs  0xFFFF  //mask used to get address
#define maskoffset  0xFF //mask used to retrieve offset
#define TLB_SIZE 16
#define BLOCK 256

FILE *address_file;
FILE *backing_file;
int nola=0; //number of addresses in logical memory
char str[3]; //read/write variable for Dirty
int l_adrs; //logical addresses
double pf_rate; //page fault percent
double TLB_hit_rate; //TLB hit percent
int pageTableNumbers[256];  //page numbers in page table
int pageTableFrames[256];   //frame numbers in page table
int TLBPageNumber[16];  //page numbers in the TLB
int TLBFrameNumber[16]; //frame numbers in the TLB
int physicalMemory[256][256]; //physical memory size=(number of frames)*(no. of frames)
int pf = 0;   //page faults counter
int TLB_hits = 0;      //TLB hit counter
int firstAvailableFrame = 0;  //counter to track the first available frame
int firstAvailablePageTableNumber = 0; //counter track the first available page table entry
int numberOfTLBEntries = 0;   //number of entries in the TLB

signed char buffer[BLOCK]; // the buffer containing reads from backing store
signed char value; // the value of the byte (signed char) in memory

void update_TLB(int pageNumber, int frameNumber);
void backing_store(int pageNumber);

//this function converts logical address to physical address,reads the value stored at that physical address
void read_padrs(l_adrs){
    
    // obtain the page number and offset from the logical address
    int pageNumber = ((l_adrs & maskadrs)>>8);
    int offset = (l_adrs & maskoffset);
    
    int frameNumber = -1; // initialized to -1 to tell if it's valid in the conditionals below
    
    int i;  // look for the frame in TLB
    for(i = 0; i < TLB_SIZE; i++){
        if(TLBPageNumber[i] == pageNumber)
	{   
            frameNumber = TLBFrameNumber[i];  
                TLB_hits++;                //TLBHit incremented
        }
    }
    
    //Next, look in page table
    if(frameNumber == -1){
        int i;  
        for(i = 0; i < firstAvailablePageTableNumber; i++){
            if(pageTableNumbers[i] == pageNumber)
		{         
                frameNumber = pageTableFrames[i]; 
            }
        }

    //if page is not found in TLB or page table, read from backing store
    if(frameNumber == -1)
    {     
        backing_store(pageNumber);
        pf++;                          // increment the number of page faults
        frameNumber = firstAvailableFrame - 1;  
        }
    }
    
    update_TLB(pageNumber, frameNumber); //insert the page number and frame number into the TLB
    value = physicalMemory[frameNumber][offset];  // frame number and offset used to get value at that address
    printf("Virtual address: %d Physical address: %d Value: %d\n", l_adrs, (frameNumber << 8) | offset, value);
}

// function to insert a page number and frame number into the TLB with a FIFO replacement
void update_TLB(int pageNumber, int frameNumber){
    
    int i;  // if it's already in the TLB, break
    for(i = 0; i < numberOfTLBEntries; i++){
        if(TLBPageNumber[i] == pageNumber){
            break;
        }
    }
    
    // if the number of entries is equal to the index
    if(i == numberOfTLBEntries){
        if(numberOfTLBEntries < TLB_SIZE){  // and the TLB still has room in it
            TLBPageNumber[numberOfTLBEntries] = pageNumber;    // insert the page and frame on the end
            TLBFrameNumber[numberOfTLBEntries] = frameNumber;
        }
        else{                                            // otherwise move everything over
            for(i = 0; i < TLB_SIZE - 1; i++){
                TLBPageNumber[i] = TLBPageNumber[i + 1];
                TLBFrameNumber[i] = TLBFrameNumber[i + 1];
            }
            TLBPageNumber[numberOfTLBEntries-1] = pageNumber;  // and insert the page and frame on the end
            TLBFrameNumber[numberOfTLBEntries-1] = frameNumber;
        }        
    }
    
    // if the index is not equal to the number of entries
    else{
        for(i = i; i < numberOfTLBEntries - 1; i++)
	{// iterate through up to one less than the number of entries
            TLBPageNumber[i] = TLBPageNumber[i + 1]; //move everything over in the arrays
            TLBFrameNumber[i] = TLBFrameNumber[i + 1];
        }
        if(numberOfTLBEntries < TLB_SIZE)
	{  // if there is still room in the array, put the page and frame on the end
            TLBPageNumber[numberOfTLBEntries] = pageNumber;
            TLBFrameNumber[numberOfTLBEntries] = frameNumber;
        }
        else
	{ // otherwise put the page and frame on the number of entries - 1
            TLBPageNumber[numberOfTLBEntries-1] = pageNumber;
            TLBFrameNumber[numberOfTLBEntries-1] = frameNumber;
        }
    }
    if(numberOfTLBEntries < TLB_SIZE){
       // if there is still room in the arrays, increment the number of entries
        numberOfTLBEntries++;
    }    
}

// function to read from the backing store and bring the frame into physical memory and the page table
void backing_store(int pageNumber){
    // first seek to byte BLOCK in the backing store
    // SEEK_SET in fseek() seeks from the beginning of the file
    if (fseek(backing_file, pageNumber * BLOCK, SEEK_SET) != 0) {
        fprintf(stderr, "Error seeking in backing store\n");
    }
    
    // now read CHUNK bytes from the backing store to the buffer
    if (fread(buffer, sizeof(signed char), BLOCK, backing_file) == 0) {
        fprintf(stderr, "Error reading from backing store\n");        
    }
    
    // load the bits into the first available frame in the physical memory 2D array
    int i;
    for(i = 0; i < BLOCK; i++){
        physicalMemory[firstAvailableFrame][i] = buffer[i];
    }
    
    // and then load the frame number into the page table in the first available frame
    pageTableNumbers[firstAvailablePageTableNumber] = pageNumber;
    pageTableFrames[firstAvailablePageTableNumber] = firstAvailableFrame;
    
    // increment the counters that track the next available frames
    firstAvailableFrame++;
    firstAvailablePageTableNumber++;
}

int main(int argc, char* argv[])
{
	if(argc!=2)
	{
	fprintf(stderr,"Please use format: ./a.out filename\n");	
	return -1;
	}
	else
	{ 
	address_file = fopen(argv[1],"r"); //open input file and backing store
 	backing_file = fopen("BACKING_STORE.bin","rb");
	}

	while(!feof(address_file))
	{
	fscanf(address_file,"%d %s",&l_adrs,str);
	read_padrs(l_adrs); //read from physical memory using logical address
	nola++;
	}	
	//printf("%d %s %d\n",l_adrs,str,nola);
	   
    	//stats calculation
    	printf("Number of translated addresses = %d\n", nola);
    	pf_rate = pf / (double)nola;
    	TLB_hit_rate = TLB_hits / (double)nola;
    	
	//stats output
    	printf("Page Faults = %d\n", pf);
    	printf("Page Fault Rate = %.3f % \n",pf_rate*100);
    	printf("TLB Hits = %d\n", TLB_hits);
    	printf("TLB Hit Rate = %.3f % \n", TLB_hit_rate*100);
    
    	// close the input file and backing store
	fclose(address_file);
	fclose(backing_file);
    
    	return 0;
}
