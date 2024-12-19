#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
        int pageNo;
        int modified;
} page;
enum    repl { randomFrame, fifo, lru, clockAlg};
int     createMMU( int);
int     checkInMemory( int ) ;
int     allocateFrame( int, int) ;
page    selectVictim( int, enum repl) ;
const   int pageoffset = 12;            /* Page size is fixed to 4 KB */
int     numFrames ;


//Declare globally
int* pageNoArray; //dynamic arrays to store given number of frames
int* modifiedArray; 
//Pages contain a pageNo and a modified bit
//Couldn't figure out how to make an array of pages so
//i'm using two separate arrays instead

int* useArray; //Another dynamic array, this is just for clock algorithm
int clockHand = 0;

int* lruQueue; //Stores a counter for each frame. When a frame is used it gets
//set to 0 and all counters are incremented. Least recently used is the 
//frame with the largest counter

/* Creates the page table structure to record memory allocation */
int     createMMU (int frames)
{
	//Dynamically allocate arrays to store frames
	pageNoArray = (int*)malloc(frames*sizeof(int)); 
	modifiedArray = (int*)malloc(frames*sizeof(int));

	useArray = (int*)malloc(frames*sizeof(int)); //for clock alg
	lruQueue = (int*)malloc(frames*sizeof(int)); //for lru

	//Check memory allocated
	if (pageNoArray == NULL || modifiedArray == NULL || useArray == NULL) {
		return -1;
	}

	//Initialise pageNoArray with all -1s
	//This is because default is 0 and 0 is a possible page number, -1 is not
	for (int i=0; i < numFrames; i++) {
		pageNoArray[i] = -1;
	}

	//Initialise useArray with all zeros
	for (int i=0; i < numFrames; i++) {
		useArray[i] = 0;
	}

    return 0;
}

/* Checks for residency: returns frame no or -1 if not found */
int     checkInMemory( int page_number)
{
        int     result = -1;

        for (int i=0; i < numFrames; i++) {
        	if (pageNoArray[i] == page_number) {
        		result = i;
				//printf("Found page %d at frame %d \n", page_number, i); //for debug
        	}
        }

        return result ;
}

/* allocate page to the next free frame and record where it put it */
int     allocateFrame( int page_number, int allocated)
{
        pageNoArray[allocated] = page_number;
		modifiedArray[allocated] = 0;

		useArray[allocated]=1; //for clock

		//For lru, set new frame counter to zero, increment existing counters
		lruQueue[allocated]=0;
		for (int i=0; i <= allocated; i++) {
			lruQueue[i]++;
		}

		//printf("Allocated page: %d to frame: %d\n", page_number, allocated); //for debug

        return allocated;
}

/* Selects a victim for eviction/discard according to the replacement algorithm,  returns chosen frame_no  */
page    selectVictim(int page_number, enum repl  mode )
{
        page    victim;

		switch(mode) {
			case randomFrame:
				//Choose random frame
				srand(time(0));
				int randomNum = rand() % numFrames;
				//Get victim details
				victim.pageNo = pageNoArray[randomNum];
				victim.modified = modifiedArray[randomNum];
				//Replace victim
				pageNoArray[randomNum] = page_number;
				modifiedArray[randomNum] = 0;
			break;
			case fifo:
				//This is optional if we have time
			break;
			case lru:
				//Choose least recently used
				int lruChosen;
				int max = -1;
				for (int i=0; i < numFrames; i++) { //find location of largest counter
					if (lruQueue[i] > max) {
						lruChosen = i;
						max = lruQueue[i];
					}
				}
				//Get victim details
				victim.pageNo = pageNoArray[lruChosen];
				victim.modified = modifiedArray[lruChosen];
				//Replace victim
				pageNoArray[lruChosen] = page_number;
				modifiedArray[lruChosen] = 0;

				lruQueue[lruChosen] = 0; //new is most recently used
				for (int i=0; i < numFrames; i++) { //increment all counters
					lruQueue[i]++;
				}

			break;
			case clockAlg:
				int victimFound = 0;
				while(victimFound == 0) {
					
					//check use bit
					if (useArray[clockHand] == 1) {
						useArray[clockHand] = 0;	
					} else {
						//Get victim details
						victim.pageNo = pageNoArray[clockHand];
						victim.modified = modifiedArray[clockHand];
						//Replace victim
						pageNoArray[clockHand] = page_number;
						modifiedArray[clockHand] = 0;
						useArray[clockHand] = 1;
						victimFound = 1; //break loop
					}

					//advance hand
					if (clockHand >= numFrames-1) { //If at end of array, reset
						clockHand = 0;
					} else {
						clockHand++;
					}
				}

			break;
			default:
	        	victim.pageNo = 0;
    	    	victim.modified = 0;
		}

        return (victim) ;
}

		
int main(int argc, char *argv[])
{
  
	char	*tracename;
	int	page_number,frame_no, done ;
	int	do_line, i;
	int	no_events, disk_writes, disk_reads;
	int     debugmode;
 	enum	repl  replace;
	int	allocated=0; 
	int	victim_page;
        unsigned address;
    	char 	rw;
	page	Pvictim;
	FILE	*trace;


        if (argc < 5) {
             printf( "Usage: ./memsim inputfile numberframes replacementmode debugmode \n");
             exit ( -1);
	}
	else {
        tracename = argv[1];	
	trace = fopen( tracename, "r");
	if (trace == NULL ) {
             printf( "Cannot open trace file %s \n", tracename);
             exit ( -1);
	}
	numFrames = atoi(argv[2]);
        if (numFrames < 1) {
            printf( "Frame number must be at least 1\n");
            exit ( -1);
        }
        if (strcmp(argv[3], "lru\0") == 0)
            replace = lru;
	    else if (strcmp(argv[3], "rand\0") == 0)
	     replace = randomFrame;
	          else if (strcmp(argv[3], "clock\0") == 0)
                       replace = clockAlg;		 
	               else if (strcmp(argv[3], "fifo\0") == 0)
                             replace = fifo;		 
        else 
	  {
             printf( "Replacement algorithm must be rand/fifo/lru/clock  \n");
             exit ( -1);
	  }

        if (strcmp(argv[4], "quiet\0") == 0)
            debugmode = 0;
	else if (strcmp(argv[4], "debug\0") == 0)
            debugmode = 1;
        else 
	  {
             printf( "Replacement algorithm must be quiet/debug  \n");
             exit ( -1);
	  }
	}
	
	done = createMMU (numFrames);
	if ( done == -1 ) {
		 printf( "Cannot create MMU" ) ;
		 exit(-1);
        }
	no_events = 0 ;
	disk_writes = 0 ;
	disk_reads = 0 ;

        do_line = fscanf(trace,"%x %c",&address,&rw);
	while ( do_line == 2)
	{
		page_number =  address >> pageoffset;
		frame_no = checkInMemory( page_number) ;    /* ask for physical address */

		if ( frame_no == -1 )
		{
		  disk_reads++ ;			/* Page fault, need to load it into memory */
		  if (debugmode) 
		      printf( "Page fault %8d \n", page_number) ;
		  if (allocated < numFrames)  			/* allocate it to an empty frame */
		   {
                     frame_no = allocateFrame(page_number, allocated);
		     allocated++;
                   }
                   else{
		      Pvictim = selectVictim(page_number, replace) ;   /* returns page number of the victim  */
		      frame_no = checkInMemory( page_number) ;    /* find out the frame the new page is in */
		   if (Pvictim.modified)           /* need to know victim page and modified  */
	 	      {
                      disk_writes++;			    
                      if (debugmode) printf( "Disk write %8d \n", Pvictim.pageNo) ;
		      }
		   else
                      if (debugmode) printf( "Discard    %8d \n", Pvictim.pageNo) ;
		   }
		} else {
			//If page is already in memory, mark as used for clock algorithm
			useArray[frame_no] = 1;

			//reset counter and increment the others for lru
			lruQueue[frame_no] = 0; //new is most recently used
			for (int i=0; i < numFrames; i++) { //increment all counters
				lruQueue[i]++;
			}
		}
		if ( rw == 'R'){
		    if (debugmode) printf( "reading    %8d \n", page_number) ;
		}
		else if ( rw == 'W'){
		    // mark page in page table as written - modified
			modifiedArray[frame_no] = 1;

		    if (debugmode) printf( "writing   %8d \n", page_number) ;
		}
		 else {
		      printf( "Badly formatted file. Error on line %d\n", no_events+1); 
		      exit (-1);
		}

		no_events++;
        	do_line = fscanf(trace,"%x %c",&address,&rw);
	}

	printf( "total memory frames:  %d\n", numFrames);
	printf( "events in trace:      %d\n", no_events);
	printf( "total disk reads:     %d\n", disk_reads);
	printf( "total disk writes:    %d\n", disk_writes);
	printf( "page fault rate:      %.4f\n", (float) disk_reads/no_events);
}
				
