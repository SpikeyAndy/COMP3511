/*
    COMP3511 Fall 2023 
    PA2: Simplified Multi-Level Feedback Queue (MLFQ)

    Your name: LUI, Cheuk Kwan Andy
    Your ITSC email:    ckalui@connect.ust.hk 

    Declaration:

    I declare that I am not involved in plagiarism
    I understand that both parties (i.e., students providing the codes and students copying the codes) will receive 0 marks. 

*/

// Note: Necessary header files are included
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define MAX_* constants
#define MAX_NUM_PROCESS 10
#define MAX_QUEUE_SIZE 10
#define MAX_PROCESS_NAME 5
#define MAX_GANTT_CHART 300

// Keywords (to be used when parsing the input)
#define KEYWORD_TQ0 "tq0"
#define KEYWORD_TQ1 "tq1"
#define KEYWORD_PROCESS_TABLE_SIZE "process_table_size"
#define KEYWORD_PROCESS_TABLE "process_table"

// Assume that we only need to support 2 types of space characters: 
// " " (space), "\t" (tab)
#define SPACE_CHARS " \t"

// Process data structure
// Helper functions:
//*  process_init: initialize a process entry
//*  process_table_print: Display the process table
struct Process {
    char name[MAX_PROCESS_NAME];
    int arrival_time ;
    int burst_time;
    int remain_time; // remain_time is needed in the intermediate steps of MLFQ 
};

//todo: create queue with process table
    struct Process q0[MAX_GANTT_CHART];
    struct Process q1[MAX_GANTT_CHART];
    struct Process fcfs[MAX_GANTT_CHART];
    int q0_length = 0;
    int q1_length = 0;
    int fcfs_length = 0;

void process_init(struct Process* p, char name[MAX_PROCESS_NAME], int arrival_time, int burst_time) {
    strcpy(p->name, name);
    p->arrival_time = arrival_time;
    p->burst_time = burst_time;
    p->remain_time = 0;
}
void process_table_print(struct Process* p, int size) {
    int i;
    printf("Process\tArrival\tBurst\n");
    for (i=0; i<size; i++) {
        printf("%s\t%d\t%d\n", p[i].name, p[i].arrival_time, p[i].burst_time);
    }
}

// A simple GanttChart structure
// Helper functions:
//*   gantt_chart_update: append one item to the end of the chart (or update the last item if the new item is the same as the last item)
//*   gantt_chart_print: display the current chart
struct GanttChartItem {
    char name[MAX_PROCESS_NAME];
    int duration;
};
void gantt_chart_update(struct GanttChartItem chart[MAX_GANTT_CHART], int* n, char name[MAX_PROCESS_NAME], int duration) {
    int i;
    i = *n;
    // The new item is the same as the last item
    if ( i > 0 && strcmp(chart[i-1].name, name) == 0) 
    {
        chart[i-1].duration += duration; // update duration
    } 
    else
    {
        strcpy(chart[i].name, name);
        chart[i].duration = duration;
        *n = i+1;
    }
}
void gantt_chart_print(struct GanttChartItem chart[MAX_GANTT_CHART], int n) { 
    int t = 0;
    int i = 0;
    printf("Gantt Chart = ");
    printf("%d ", t);
    for (i=0; i<n; i++) {
        t = t + chart[i].duration;     
        printf("%s %d ", chart[i].name, t);
    }
    printf("\n");
}

//* Global variables
int tq0 = 0, tq1 = 0;
int process_table_size = 0;
struct Process process_table[MAX_NUM_PROCESS];

// Helper function: Check whether the line is a blank line (for input parsing)
int is_blank(char *line) {
    char *ch = line;
    while ( *ch != '\0' ) {
        if ( !isspace(*ch) )
            return 0;
        ch++;
    }
    return 1;
}
// Helper function: Check whether the input line should be skipped
int is_skip(char *line) {
    if ( is_blank(line) )
        return 1;
    char *ch = line ;
    while ( *ch != '\0' ) {
        if ( !isspace(*ch) && *ch == '#')
            return 1;
        ch++;
    }
    return 0;
}
// Helper: parse_tokens function
void parse_tokens(char **argv, char *line, int *numTokens, char *delimiter) {
    int argc = 0;
    char *token = strtok(line, delimiter);
    while (token != NULL)
    {
        argv[argc++] = token;
        token = strtok(NULL, delimiter);
    }
    *numTokens = argc;
}
// Helper: parse the input file
void parse_input() {
    FILE *fp = stdin;
    char *line = NULL;
    ssize_t nread;
    size_t len = 0;
    char *two_tokens[2]; // buffer for 2 tokens
    int numTokens = 0, i=0;
    char equal_plus_spaces_delimiters[5] = "";
    char process_name[MAX_PROCESS_NAME];
    int process_arrival_time = 0;
    int process_burst_time = 0;

    strcpy(equal_plus_spaces_delimiters, "=");
    strcat(equal_plus_spaces_delimiters,SPACE_CHARS);

    while ( (nread = getline(&line, &len, fp)) != -1 ) {
        if ( is_skip(line) == 0)  {
            line = strtok(line,"\n");

            if (strstr(line, KEYWORD_TQ0)) {
                // parse tq0
                parse_tokens(two_tokens, line, &numTokens, equal_plus_spaces_delimiters);
                if (numTokens == 2) {
                    sscanf(two_tokens[1], "%d", &tq0);
                }
            } 
            else if (strstr(line, KEYWORD_TQ1)) {
                // parse tq0
                parse_tokens(two_tokens, line, &numTokens, equal_plus_spaces_delimiters);
                if (numTokens == 2) {
                    sscanf(two_tokens[1], "%d", &tq1);
                }
            }
            else if (strstr(line, KEYWORD_PROCESS_TABLE_SIZE)) {
                // parse process_table_size
                parse_tokens(two_tokens, line, &numTokens, equal_plus_spaces_delimiters);
                if (numTokens == 2) {
                    sscanf(two_tokens[1], "%d", &process_table_size);
                }
            } 
            else if (strstr(line, KEYWORD_PROCESS_TABLE)) {

                // parse process_table
                for (i=0; i<process_table_size; i++) {

                    getline(&line, &len, fp);
                    line = strtok(line,"\n");  

                    sscanf(line, "%s %d %d", process_name, &process_arrival_time, &process_burst_time);
                    process_init(&process_table[i], process_name, process_arrival_time, process_burst_time);

                }
            }
        } 
    }
}
// Helper: Display the parsed values
void print_parsed_values() {
    printf("%s = %d\n", KEYWORD_TQ0, tq0);
    printf("%s = %d\n", KEYWORD_TQ1, tq1);
    printf("%s = \n", KEYWORD_PROCESS_TABLE);
    process_table_print(process_table, process_table_size);
}

void dequeue(struct Process *q, int *queue_size) {
    // Check if the queue is empty
    if (*queue_size == 0) {
        printf("ERROR: Calling use_queue when size = 0\n");
        return;
    }

    // Dequeue the first process from the queue
    // struct Process current_process = q[0];

    // Shift the remaining processes forward in the queue
    for (int i = 0; i < *queue_size - 1; i++) {
        q[i] = q[i + 1];
    }

    // Decrement the queue size
    (*queue_size)--;
}

void add_queue(struct Process *q, struct Process *new_process, int *queue_size) {
    // Check if the queue is full
    if (*queue_size == MAX_QUEUE_SIZE) {
        printf("ERROR: Queue is full. Cannot add more processes.\n");
        return;
    }
    // Add the new process to the end of the queue
    q[*queue_size] = *new_process;

    // Increment the queue size
    (*queue_size)++;

    // Print a message indicating the process has been added
    printf("Process %s added to the queue.\n", q[*queue_size].name);
}

void move_queue(struct Process item, struct Process *dest, int *dest_size, struct Process *src, int *src_size) {
    // Check if the source queue is empty
    if (*src_size == 0) {
        printf("ERROR: Source queue is empty. Cannot move process.\n");
        return;
    }

    // Check if the destination queue is full
    if (*dest_size == MAX_QUEUE_SIZE) {
        printf("ERROR: Destination queue is full. Cannot move process.\n");
        return;
    }

    // Find the index of the item in the source queue
    int item_index = -1;
    for (int i = 0; i < *src_size; i++) {
        if (src[i].name == item.name) {
            item_index = i;
            break;
        }
    }

    // Check if the item was found in the source queue
    if (item_index == -1) {
        printf("ERROR: Process not found in the source queue.\n");
        return;
    }

    // Move the item from the source queue to the destination queue
    add_queue(dest, &dest[item_index], dest_size);
    
    // Shift the remaining processes in the source queue (MUST be removed only: cannot use dequeue because it will cause mutual recursion)
    dequeue(src, src_size);

    // Print a message indicating the process has been moved
    printf("Process %s moved to the destination queue.\n", item.name);
}

void item_init(struct GanttChartItem *item, char name[MAX_PROCESS_NAME], int duration) {
    strcpy(item->name, name);
    item->duration = duration;
}

int min(int a, int b) {
    return (a < b) ? a : b;
}

// TODO: Implementation of MLFQ algorithm
void mlfq() {

    // Initialize the gantt chart
    struct GanttChartItem gantt_chart[MAX_GANTT_CHART]; //* array chart (in order of priority)
    int chart_size = 0;

    // TODO: implement your MLFQ algorithm here
    int num_process = 0;
    int time = 0;


    for (int i = 0; i < MAX_NUM_PROCESS; i++) { //* num_process = count number of processes
        if (process_table[i].burst_time == 0) {
            num_process = i;
            break;
        }
    }
    for (int i = 0; i < num_process; i++) { //* time = sum of burst time
        time = time + process_table[i].burst_time; 
    }

    //todo: run through time and check which queue to run process
    for (int real_time = 0; real_time < time; real_time++) {

        //todo: condition only if there is new process arrived
        for (int process_id = 0; process_id < num_process; process_id++) {
            if (process_table[process_id].arrival_time == real_time) {  // for interrupts of q0
                process_table[process_id].remain_time = tq0;
                add_queue(q0, &process_table[process_id], &q0_length);
            }
        }

        struct GanttChartItem item;

        //todo: if q0 has processes lined up
        if (q0_length > 0) {
            item_init(&item, q0[0].name, 0); //! min() function usage here!
            q0[0].remain_time--; // deduct remaining time in the queue
            q0[0].burst_time--; // deduct remaining time to burst CPU time

            

            if (q0[0].burst_time == 0) {
                gantt_chart[chart_size] = item;
                chart_size++; // tracking chart index
                dequeue(q0, &q0_length); 
            }
            else if (q0[0].remain_time == 0) {
                q0[0].remain_time = tq1;
                gantt_chart[chart_size] = item;
                chart_size++; // tracking chart index
                move_queue(q0[0], q1, &q1_length, q0, &q0_length);
            }
            else {
                gantt_chart_update(gantt_chart, &chart_size, q0[0].name, item.duration + 1);
            }
            
            continue;
        }
        
        //todo: if q1 has processes lined up only when q0 is empty
        else if (q1_length > 0 && q0_length == 0) {
            item_init(&item, q1[0].name, 0);
            q1[0].remain_time--; // deduct remaining time in the queue
            q1[0].burst_time--;

            
                  
            if (q1[0].burst_time == 0) {
                gantt_chart[chart_size] = item;
                chart_size++; // tracking chart index
                dequeue(q1, &q1_length); 
            }
            else if (q1[0].remain_time == 0) {
                q1[0].remain_time = q1[0].burst_time;
                gantt_chart[chart_size] = item;
                chart_size++; // tracking chart index
                move_queue(q1[0], fcfs, &fcfs_length, q1, &q1_length);
            }
            else {
                gantt_chart_update(gantt_chart, &chart_size, q1[0].name, item.duration++);
            }
            continue;
        }
        else if (fcfs_length > 0 && q0_length == 0 && q1_length == 0) {
            item_init(&item, fcfs[0].name, 0);
            gantt_chart[chart_size] = item;
            
            chart_size++;

            fcfs[0].burst_time--;
            item.duration++;

            if (fcfs[0].burst_time == 0) {
                dequeue(fcfs, &fcfs_length); 
            }
             // tracking chart index
            continue;
        }
    }

    // Tips: A simple array is good enough to implement a queue

    // At the end, display the final Gantt chart
    gantt_chart_print(gantt_chart, chart_size);
}


int main() {
    parse_input();
    print_parsed_values();
    mlfq();
    return 0;
}