#include<stdio.h>
#include<stdlib.h>


typedef struct process_queue {
    int process_sn;
    int process_id;
    int burst_time;
    int waiting_time;
    int t_around_time;

    struct process_queue* nxt_process;
    struct process_queue* prv_process;
}PQ_t;

typedef struct processes {
    int queue_sn;
    PQ_t* queue;
    int queue_priority;

    struct processes* next_queue;
    struct processes* prev_queue;
}Processes;

PQ_t* system_prs;
PQ_t* user_prs;
Processes* all_processes;
void add_process_at_index(PQ_t** head, PQ_t* process, int i, int id);
void add_process_queue_at_index(Processes** head, PQ_t* process_queue, int priority, int i);
PQ_t* get_process_at_index(int i, PQ_t* first);
void print_queue_processes(PQ_t* queue);
void print_all_queues(Processes* head);
//void compute_process_details(PQ_t* queue);
void compute_process_detail_following_priorities(PQ_t* queue, int prev_burst, int prev_turn_around, int prev_waiting);
void compute_all_processes(Processes* processes);
void execute_processes_in_fcfs(PQ_t* queue);
void execute_all_processes_following_fcfs(Processes* processes);
Processes* get_highest_priority_queue(Processes* head);
int get_tot_burst_time(Processes* head);

int get_burst_times(Processes* proc);
int get_turnaround_times(Processes* proc);
int get_waiting_times(Processes* proc);

int get_burst_time(PQ_t* qu);
int get_turnaround_time(PQ_t* qu);
int get_waiting_time(PQ_t* qu);

Processes* get_queue_at(int sn, Processes* head);
void remove_queue_at(Processes** head, int sn);


int main() {
    int i=0;
    int status=1;
    int p_id, p_type, p_burst;
    int sys_process_c=0, user_process_c=0, process_count=0;
    char welcome[]="Multilevel Queue\nTwo queue levels are identified.\nSystem processes which have priority 1 and User processes with priority of 0.\n";

//    for(i=0; i< (sizeof(welcome)/sizeof(char)); i++) {
//        printf("%c", welcome[i]);
//        fflush(stdout);
//        sleep(1);
//    }

    printf("===========================================================================\n");
    printf("%s", welcome);

    printf("\n\t\tAUTHORS\n\t\t1. Ndimu Courage \n\t\t2. Ngouleu Tertullien\n\t\t3. Ramish Benoit\n\t\t4. Serkwi Bruno Ndzi\n");
    printf("\n\n=========================================================================\n");
    printf("\n");

    printf("CREATING PROCESSES\n");

    while(1) { //loop to create processes untill user is done creating processes
    printf("\n=========== Enter ANY INTEGER to INITIALIZE A PROCESS or 0 to QUIT ========\n");
    scanf("%d",&status);

    if(status ==0)
        break;
    printf("\n\n***** New Process Details ****\n\n");
    printf("Input process TYPE (0 /1): ");
    scanf("%d", &p_type);
    printf("Input burst time of process: ");
    scanf("%d", &p_burst);

    PQ_t temp = {.burst_time=p_burst};
    if(p_type > 0)
        add_process_at_index(&system_prs, &temp, ++sys_process_c, ++process_count);
    else {
         add_process_at_index(&user_prs, &temp, ++user_process_c, ++process_count);
    }

    printf("::CREATED process ID: %d", process_count);
    }
    add_process_queue_at_index(&all_processes, system_prs, 1, 1);
    add_process_queue_at_index(&all_processes, user_prs, 0, 2);

    printf("\t\t===== BEFORE Executing Processes=======\n\t Process_Id\t Burst time\t Waiting Time\tTurn Around time\n");
    print_all_queues(all_processes);
	compute_all_processes(all_processes);
	printf("\n\n_____________________ EXECUTING PROCESSES ________________________\n");
	execute_all_processes_following_fcfs(all_processes);
	printf("\n\n");
    printf("\t\t===== AFTER Executing Processes=======\n\t Process_Id\t Burst time\t Waiting Time\tTurn Around time\n");
	print_all_queues(all_processes);

return 0;
}



void add_process_at_index(PQ_t** head, PQ_t* proc, int i, int id) {
    PQ_t* curr ;
    if(*head == NULL) {
        *head = (PQ_t*)malloc(sizeof(PQ_t));

        (*head)->process_sn=i;
        (*head)->process_id = id;
        (*head)->burst_time=proc->burst_time;
        (*head)->prv_process= NULL;
        (*head)->nxt_process=NULL;
    } else {
        curr = *head;
        while(curr->nxt_process != NULL) {
            curr = curr->nxt_process;
        }
        proc->prv_process = curr;
        curr->nxt_process = (PQ_t*)malloc(sizeof(PQ_t));
        curr=curr->nxt_process;

        curr->process_sn=i;
        curr->burst_time = proc->burst_time;
        curr->prv_process= proc->prv_process;
        curr->process_id = id;
        curr->nxt_process=NULL;
    }
}

PQ_t* get_process_at_index(int i, PQ_t* first) {
    PQ_t* curr_p = first;
    while(curr_p != NULL) {
        if(curr_p->process_sn == i)
            return curr_p;
        curr_p = curr_p->nxt_process;
    }
    return NULL;
}

void print_queue_processes(PQ_t* queue) {
    if(queue == NULL)
        return;
    else {
        printf("\n\t\t%d\t\t%d\t\t%d\t\t%d\n", queue->process_id, queue->burst_time, queue->waiting_time, queue->t_around_time);
        print_queue_processes(queue->nxt_process);
    }
}
void print_all_queues(Processes* head) {
    if(head == NULL)
        return;
    else {
        print_queue_processes(head->queue);
        print_all_queues(head->next_queue);
    }
}

void compute_process_details(PQ_t* queue) {
    while(queue != NULL) {
        if(queue->prv_process == NULL) {
            queue->waiting_time = 0;
        }
        else {
            PQ_t* prev = queue->prv_process;
            queue->waiting_time = prev->waiting_time + prev->burst_time;
            queue->t_around_time = prev->t_around_time + queue->burst_time;
        }
        queue = queue->nxt_process;
    }
}

void compute_process_detail_following_priorities(PQ_t* queue, int prev_burst, int prev_turn_around, int prev_waiting) {
    while(queue != NULL) {
        if(queue->prv_process == NULL) {
            queue->waiting_time = prev_waiting + prev_burst;
            queue->t_around_time = prev_turn_around + queue->burst_time;
        }
        else {
            PQ_t* prev = queue->prv_process;
            queue->waiting_time = prev->waiting_time + prev_waiting + prev->burst_time + prev_burst;
            queue->t_around_time = prev->t_around_time + prev_turn_around + queue->burst_time;
        }
        queue = queue->nxt_process;
    }
}

void execute_processes_in_fcfs(PQ_t* queue) {
    int i;
    while(queue != NULL) {
        printf("Process %d in execution...\n", queue->process_id);
        for(i=queue->burst_time; i>0; i--) {
            queue->burst_time=0;
            sleep(1);
        }
        printf("Process %d out of execution.\n", queue->process_sn);
sleep(1);
        queue=queue->nxt_process;
    }
}

void add_process_queue_at_index(Processes** head, PQ_t* process_queue, int priority, int i) {
    Processes* curr ;
    if(*head == NULL) {
        *head = (Processes*)malloc(sizeof(Processes));

        (*head)->queue = process_queue;
        (*head)->queue_sn = i;
        (*head)->queue_priority = priority;
        (*head)->next_queue = NULL;
        (*head) ->prev_queue = NULL;

    } else {
        curr = *head;
        while(curr->next_queue != NULL) {
            curr = curr->next_queue;
        }
        curr->next_queue = (Processes*)malloc(sizeof(Processes));
        Processes* tmp = curr;

        curr=curr->next_queue;

        curr->queue_sn=i;
        curr->queue_priority = priority;
        curr->queue = process_queue;
        curr->next_queue=NULL;
        curr->prev_queue = tmp;
    }
}

void execute_all_processes_following_fcfs(Processes* processes) {
	int total_burst_time = get_tot_burst_time(processes);
	while(total_burst_time > 0) {
		Processes* max_priority = get_highest_priority_queue(processes);

		execute_processes_in_fcfs(max_priority->queue);
		remove_queue_at(&processes, max_priority->queue_sn);

		total_burst_time = get_tot_burst_time(processes);
	}
}
void compute_all_processes(Processes* processes) {
	int prev_waiting_time = 0, prev_turn_around_time=0, prev_burst_time=0;

	while( processes != NULL) {
        Processes* prev = processes->prev_queue;
        if(prev!=NULL) {//implying this is not the first process queue in process queue list
            prev_burst_time = get_burst_times(prev);
            prev_turn_around_time = get_turnaround_times(prev);
            prev_waiting_time = get_waiting_times(prev);
        } else {

        }

        compute_process_detail_following_priorities(processes->queue, prev_burst_time, prev_turn_around_time, prev_waiting_time);
        processes = processes->next_queue;
	}
}

int get_turnaround_times(Processes* proc) {
    if(proc == NULL)
        return 0;
    else {
        return (get_turnaround_time(proc->queue) + get_turnaround_times(proc->next_queue));
    }
}

int get_burst_times(Processes* proc) {
    if(proc == NULL)
        return 0;
    else {
        return get_burst_time(proc->queue) + (get_burst_times(proc->next_queue));
    }
}

int get_waiting_times(Processes* proc) {
    if(proc == NULL)
        return 0;
    else {
        return get_waiting_time(proc->queue) + get_waiting_times(proc->next_queue);
    }
}

int get_tot_burst_time(Processes* head) {
	if(head == NULL)
        return 0;
    else {
		PQ_t* q = head->queue;
		return get_burst_time(q) + get_tot_burst_time(head->next_queue);
    }
}

int get_burst_time(PQ_t* qu) {
    if(qu == NULL)
        return 0;
    else
        return qu->burst_time + get_burst_time(qu->nxt_process);
}

int get_turnaround_time(PQ_t* qu) {
    if(qu == NULL)
        return 0;
    else
        return qu->t_around_time + get_turnaround_time(qu->nxt_process);
}
int get_waiting_time(PQ_t* qu) {
    if(qu == NULL)
        return 0;
    else
        return qu->waiting_time + get_waiting_time(qu->nxt_process);
}

Processes* get_highest_priority_queue(Processes* head) {
	Processes* temp = head;
	Processes* res = head;
	if(temp == NULL)
        return NULL;
    else {
		while(temp!= NULL) {
			if(temp->queue_priority >= res->queue_priority && get_burst_time(temp->queue) > 0) {
				res = temp;
			}
			temp = temp->next_queue;
		}
    }
	return res;
}

void remove_queue_at(Processes** head, int sn) {
    Processes* curr = *head;

    while(curr != NULL) {
        if(curr->queue_sn == sn) {
            if(curr == *head) {
                *head = curr->next_queue;
            } else {
                Processes* prev = get_queue_at(sn-1, *head);
                prev->next_queue = curr->next_queue;
            }
        }
        curr = curr->next_queue;
    }
}

Processes* get_queue_at(int sn, Processes* head) {
    if(head == NULL)
        return NULL;
    else {
        while(head != NULL) {
            if(head->queue_sn == sn)
                return head;
            head = head->next_queue;
        }
    }
}
