/*
By: Ali Ghanem and Omar Altahan

The Sleeping Barber
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

void barber();   // barber thread function
void customer(); //customer thread function
                               
sem_t seatMutex;        // Semaphore for the waiting room
sem_t customers;        // Semaphore for customers currently in the waiting room ready to be served
sem_t smfBarber;        // Semaphore for the barber

int runningTime;        // Total time in seconds that the main thread will work
int numberOfFreeSeats;  // Capacity of the waiting room
int customersCount;     // Number of customers in the program that will come to the shop
int customerWait;       // Maximum time a customer would wait to come again to the shop
int getHCn;             // Number of customers who get haircut

pthread_t barberThread;           // Barber's thread
pthread_t customersThreads[20];   // An array of customer threads with max size of 20 

// Main method of the code                    
int main() {

    printf("Enter the running time of the program: ");
    scanf("%d",&runningTime);
    printf("Enter the number of free seats: ");
    scanf("%d",&numberOfFreeSeats);
    printf("Enter the customers count: ");
    scanf("%d",&customersCount);
    printf("Enter the maximun waiting time for the customer to come again: ");
    scanf("%d",&customerWait);
    
    getHCn = 0;   
   
    printf("\nProgram is beginning\n\n");
   
    // Initializing semaphores:
    // zero indicates that this semaphore is not allowed to be shared between processes, 1 is an initial value
    sem_init(&seatMutex,0,1); // the initial value is 1 because we need to allow only one thread to access
    sem_init(&customers,0,0);
    sem_init(&smfBarber,0,0);
   
    // Creating barber thread:
    pthread_create(&barberThread, NULL, barber, NULL); // barber is the function which the thread will start at
    printf("Barber has been created.\n");
    
    // Creating customers threads:
    for (int i = 0; i < customersCount; i++){
	   pthread_create(&customersThreads[i], NULL, customer, NULL); // customer is the function which the thread will start at
	   printf("Customer %u has been created.\n",customersThreads[i]);
    }
   
    // stop the main thread in order to run the other threads
    sleep(runningTime);
     
    printf("\n\nEnd of the day :)\n");
    printf("%d out of %d customers get haircut.",getHCn,customersCount);
    exit(0);
}

// barber thread function:
void barber() {
    int workingTime;    // Will be generated randomly to indicate the time
                    // that the barber will take to cut a customer's hair

    while(1) {
        //===== ENTRY SECTION =====//
       
        // acquire customers to wait for a customer 
	    sem_wait(&customers);
        // acquire seatMutex to access seats count  
	    sem_wait(&seatMutex);

	    //===== CRITICAL SECTION =====//
	  
        // increase the number of free seats
	    numberOfFreeSeats += 1;
	  
	    // generate random time between 1-5 seconds for the period of haircut.  
	    workingTime = (rand() % 5) + 1;
	    printf("Barber took a new customer, and he will take %d seconds for haircut.\n",workingTime);
	  
	    printf("\tNumber of free seats: %d\n",numberOfFreeSeats);
	    //===== EXIT SECTION =====//
	  
        // signal to customer which mean barber is ready  
	    sem_post(&smfBarber);
        // release lock on seat count 
	    sem_post(&seatMutex);
   	    sleep(workingTime);
    } 
}

// customer thread function:
void customer() {
    int waitingTime;
    int notEnd = 1;
    while(notEnd == 1) {
	    //===== ENTRY SECTION =====//
	    
        // acquire seatMutex to access seats count  
	    sem_wait(&seatMutex);
	    
	    
        // when there are no free seats 
	    if(numberOfFreeSeats <= 0){
	      
		    //===== EXIT SECTION =====//
		  
	        // generate random time for waiting until next try
	        waitingTime = (rand() % customerWait) + 1;
		    printf("Customer %u left without haircut, and will come back after %d seconds to try again.\n", pthread_self(),waitingTime);
		    sem_post(&seatMutex); // release the semaphore
		    
	        sleep(waitingTime);
	    }
	  
	  
        // when there are free seats 
	    else{
		  
		    //===== CRITICAL SECTION =====//
		   
            // decrease the number of free seats 
		    numberOfFreeSeats -= 1;
		    printf("Customer %u is waiting.\n",pthread_self());
		    printf("\tNumber of free seats: %d\n",numberOfFreeSeats);
		   
		    //===== EXIT SECTION =====//
		    
            // customer is ready 
		    sem_post(&customers);
            // release seatMutex lock on seat count 
		    sem_post(&seatMutex);
            // wait for barber 
		    sem_wait(&smfBarber);
            // get haircut 
		    printf("Customer %u get a haircut :)\n",pthread_self());
		    // the customer thread will end 
		    notEnd = 0;
		    // increse the number of customers who get haircut
		    getHCn += 1;
	    }
      
     }
}