#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>

#define MAX_THINK_TIME 5
#define MIN_THINK_TIME 2
#define MAX_TABLE_CAPAC 1000
#define EATING_DURATION 3


sem_t s;
enum phil_state {
    THINKING, WAITING, EATING
};
struct p {
    char* name;
};
typedef struct phil {
    sem_t cutlery;
    int phil_index;
    int food_qty;
    int phil_status;

    struct phil* nxt_phil;
    struct phil* prv_phil;
}Phil;

Phil* first;    //first dinning philosopher to take his seat at the table
void add_phil_at_index(Phil** head, Phil* phil, int i);
Phil* get_phil_at_index(int i);
int get_food_left(Phil* head);

void think(Phil* phil);
void eat(Phil* p);
void* phil_rout(void * v); //routine tasks to be performed by a philosopher

int number_of_phils;    //the total number of defined philosophers (philosophers who have been given food)
pthread_t phils[MAX_TABLE_CAPAC];   //array of philosopher threads

int main() {
    int i=0;
    sem_init(&s, 0, 1);
    printf("=================== Dinning Philospher Problem Solved ===============\n");
    printf("\n Solution Proposed by: \n");
    printf("\tNdimu Courage\n \tNgouleu Tertulien\n \tRamish Benoit\n \tSerkwi Bruno Ndzi\n");
    printf("\n====================================================================\n");
    printf("\n\n");
    printf("Input the Number of philosophers available to dine (integer):\n");
    scanf("%d", &number_of_phils);
    printf("\n_ _ _ _  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _\n");


    //if an invalid number of philosophers is provided
    if(number_of_phils <= 0) {
        printf("%d is an invalid number of philosophers\n Program exiting ...\n", number_of_phils);
        sleep(1);
        return 1;
    }

//    //if a valid number of philosophers is provided
    printf("\t***You will have %d philosopher(s) dining***\n\n", number_of_phils);
    for (i=0; i<number_of_phils; i++) {
        int new_phil_food_allocated;
        printf("How many cocoyams should be given philosopher %d (an integer only)?\n", i+1);
        scanf("%d", &new_phil_food_allocated);
        Phil new_phil = {.phil_status=THINKING, .nxt_phil=NULL, .food_qty=new_phil_food_allocated};
        add_phil_at_index(&first, &new_phil, i+1);
    }

    //SET UP TABLE FOR PHILOSOPHERS TO DINE
    //permit philosophers start their routine
    for(i = 0; i<number_of_phils; i++) {
        Phil* v = get_phil_at_index(i+1);//obtain the data of the philosopher sitting at the index specified by i
        pthread_create(&phils[i], NULL, phil_rout, (void *)v);
    }
    //wait for all philosophers to finish dining before the program exits
    for(i=0; i<number_of_phils; i++) {
        pthread_join(phils[i], NULL);
    }

    printf("\n\n========================================================================\n");
    printf("******************** All Philosophers are done eating **************\n");
    int f=0;
    printf("\nexiting ");
    while(f<3) {
        printf(".");
        fflush(stdout);
        sleep(2);
        f++;
    }
return 0;
}


void * phil_rout(void* v) {
    Phil* phil = (Phil*)v;
    int tot_food = get_food_left(first);
    while(tot_food > 0) {
        think(phil);
        eat(phil);
        tot_food = get_food_left(first); //get the total amount of food remaining for all philosophers
    }
}
void eat(Phil* p) {
    sem_wait(&s);
    if(p->food_qty > 0) {
        //verify availability of left and write cutlery
        //get index of left and righ philosophers, given they are sitted in a cicle
        int left_index = (p->phil_index - 1)%number_of_phils;
        int right_index = (p->phil_index + 1)%number_of_phils;
        //if any of the indices is 0, set its value to the total number of philosophers dinning
        right_index = right_index==0 ? number_of_phils : right_index;
        left_index = left_index == 0 ? number_of_phils : left_index;

        Phil* left_phil = get_phil_at_index(left_index);
        Phil* right_phil = get_phil_at_index(right_index);

        if(left_phil->phil_status != EATING && right_phil->phil_status != EATING) {
            //allow the philosopher to eat
            p->phil_status = EATING;
            printf("Philosopher %d is EATING. Food qty left: %d\n", p->phil_index, p->food_qty);
            sleep(EATING_DURATION);
            p->food_qty--;
            printf("Philosopher %d has had a bite.\n", p->phil_index);
            p->phil_status = WAITING;
        }
    }
    sem_post(&s);
}

void think(Phil* p) {
    int x = 0;
    int dur = rand_r(&x)%((MAX_THINK_TIME - MIN_THINK_TIME + 1)+MIN_THINK_TIME);
    printf("Philosopher %d is THINKING\n", p->phil_index);
    p->phil_status = THINKING;
    //philosopher thinks for a variable number of seconds
    sleep(dur);
    if(p->food_qty > 0)
        printf("Philosopher %d is HUNGRY\n", p->phil_index);
    p->phil_status = WAITING;
}


void add_phil_at_index(Phil** head, Phil* phil, int i) {
    Phil* curr ;
    //if the table of dinning philosophers is empty
    if(*head == NULL) {
        *head = (Phil*)malloc(sizeof(Phil));

        (*head)->food_qty = phil->food_qty;
        (*head)->phil_index=i;
        (*head)->prv_phil= NULL;
        (*head)->nxt_phil=NULL;
    } else {//if there are already philosophers at the table
        curr = *head;
        while(curr->nxt_phil != NULL) {
            curr = curr->nxt_phil;
        }
        curr->nxt_phil = (Phil*)malloc(sizeof(Phil));
        curr=curr->nxt_phil;

        curr->food_qty = phil->food_qty;
        curr->phil_index=i;
        curr->prv_phil= NULL;
        curr->nxt_phil=NULL;
    }
}

int get_food_left(Phil* head) { //recursively get all food left on the table
    if(head == NULL)
        return 0;
    else
        return head->food_qty + get_food_left(head->nxt_phil);
}
Phil* get_phil_at_index(int i) { //get the details of a philosopher sitting at a specified index at table
    Phil* curr_p = first;
    while(curr_p != NULL) {
        if(curr_p->phil_index == i)
            return curr_p;
        curr_p = curr_p->nxt_phil;
    }
    return NULL;
}
