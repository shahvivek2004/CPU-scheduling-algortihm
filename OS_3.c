#include <stdio.h>
#include <stdlib.h>
// Define a structure to store process information
struct process {
    int pid;
    int arrival_time;
    int burst_time;
    int waiting_time;
    int turnaround_time;
    int completed;
    int start_time;
    int end_time;
    int remaining_time;
    int completion_time;
    int priority;
};

void sort_by_arrival_time(struct process* procs, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = i+1; j < n; j++) {
            if (procs[j].arrival_time < procs[i].arrival_time) {
                struct process temp = procs[i];
                procs[i] = procs[j];
                procs[j] = temp;
            }
        }
    }
}

void fcfs()
{
    int n;
    FILE *input_file = fopen("input.txt", "r");
    FILE *op=fopen("output.txt","w");
    fscanf(input_file,"%d", &n);
    struct process *processes;
    processes = (struct process*) malloc(n * sizeof(struct process));
    
    for (int i = 0; i < n; i++) {
        fscanf(input_file, "%d %d %d", &processes[i].pid, &processes[i].arrival_time, &processes[i].burst_time);
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].completed = 0;
    }

    fclose(input_file);

    int current_time = 0;
    for (int i = 0; i < n; i++) {
        if (current_time < processes[i].arrival_time) {
            current_time = processes[i].arrival_time;
        }
        processes[i].start_time = current_time;
        current_time += processes[i].burst_time;
        processes[i].end_time = current_time;
        processes[i].turnaround_time = processes[i].end_time - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
    }

    float avg_turnaround_time = 0;
    float avg_waiting_time = 0;
    for (int i = 0; i < n; i++) {
        avg_turnaround_time += processes[i].turnaround_time;
        avg_waiting_time += processes[i].waiting_time;
    }
    avg_turnaround_time /= n;
    avg_waiting_time /= n;

    fprintf(op,"Gantt Chart:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < processes[i].burst_time; j++) {
            fprintf(op,"P%d ", processes[i].pid);
        }
    }

    fprintf(op,"\nAverage turnaround time: %.2f\n", avg_turnaround_time);
    fprintf(op,"Average waiting time: %.2f\n", avg_waiting_time);
    fclose(op);
    free(processes);
}


void sjf()
{
    int n, i, j, time = 0, shortest, count = 0;
    float total_turnaround_time = 0, total_waiting_time = 0;
    struct process *processes;
    FILE *input= fopen("input.txt", "r");
    FILE *op=fopen("output.txt","w");
    fscanf(input,"%d", &n);
    processes = (struct process*) malloc(n * sizeof(struct process));
    
    for (i = 0; i < n; i++) {
        fscanf(input, "%d %d %d", &processes[i].pid, &processes[i].arrival_time, &processes[i].burst_time);
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].completed = 0;
    }

    fprintf(op,"Gant-chartt:\n");
    fclose(input);    
    while (count < n) {
        shortest = -1;
        for (i = 0; i < n; i++) {
            if (processes[i].arrival_time <= time && processes[i].completed == 0) {
                if (shortest == -1 || processes[i].burst_time < processes[shortest].burst_time) {
                    shortest = i;
                }
            }
        }
        
        
        if (shortest == -1) {
            time++; 
        } 
        else
        {
            processes[shortest].waiting_time = time - processes[shortest].arrival_time;
            processes[shortest].turnaround_time = processes[shortest].waiting_time + processes[shortest].burst_time;
            total_waiting_time += processes[shortest].waiting_time;
            total_turnaround_time += processes[shortest].turnaround_time;
            processes[shortest].completed = 1;
            count++;
            time += processes[shortest].burst_time;
            fprintf(op,"P%d ", processes[shortest].pid);
        }
    }
    
    fprintf(op,"\nAverage waiting time: %.2f\n", total_waiting_time / n);
    fprintf(op,"Average turnaround time: %.2f\n", total_turnaround_time / n);
    fclose(op);
    free(processes);
}


void rr()
{
    FILE *input_file = fopen("input.txt", "r");

    // Read number of processes from input file
    int num_processes;
    fscanf(input_file, "%d", &num_processes);

    // Allocate memory for array of processes
    struct process *processes = malloc(num_processes * sizeof(struct process));

    // Read process information from input file
    for (int i = 0; i < num_processes; i++) {
        fscanf(input_file, "%d %d %d", &processes[i].pid, &processes[i].arrival_time, &processes[i].burst_time);
        processes[i].remaining_time = processes[i].burst_time;
    }

    // Close input file
    fclose(input_file);

    // Open output file
    FILE *output_file = fopen("output.txt", "w");

    // Set time quantum
    int time_quantum;
    printf("Enter the value of time-quantum: ");
    scanf("%d",&time_quantum);

    // Initialize variables for average turnaround time and waiting time
    float avg_turnaround_time = 0;
    float avg_waiting_time = 0;

    // Initialize variable for current time
    int current_time = 0;

    // Initialize array for Gantt chart
    int *gantt_chart = calloc(100, sizeof(int));
    int gantt_index = 0;

    // Initialize array for number of bursts for each process
    int *num_bursts = calloc(num_processes, sizeof(int));

    // Initialize array for tracking which processes have completed execution
    int *completed = calloc(num_processes, sizeof(int));

    // Run round robin scheduling algorithm
    while (1) {
        // Check if all processes have completed execution
        int all_completed = 1;
        for (int i = 0; i < num_processes; i++) {
            if (completed[i] == 0) {
                all_completed = 0;
                break;
            }
        }
        if (all_completed) {
            break;
        }

        // Loop through processes and execute for time quantum
        for (int i = 0; i < num_processes; i++) {
            // Check if process has arrived and has remaining burst time
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
                // Add process to Gantt chart
                gantt_chart[gantt_index++] = processes[i].pid;

                // Execute process for time quantum or until completion
                if (processes[i].remaining_time <= time_quantum) {
                    current_time += processes[i].remaining_time;
                    processes[i].completion_time = current_time;
                    processes[i].remaining_time = 0;
                    processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
                    processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
                } else {
                    current_time += time_quantum;
                    processes[i].remaining_time -= time_quantum;
                }

                // Increment
                            // number of bursts for process
            num_bursts[i]++;
        }

        // Check if process has completed execution
        if (processes[i].remaining_time == 0 && completed[i] == 0) {
            completed[i] = 1;
            avg_turnaround_time += processes[i].turnaround_time;
            avg_waiting_time += processes[i].waiting_time;
        }
    }

    // Check if any process has completed execution before the end of the time quantum
    int early_completion = 0;
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].remaining_time == 0 && completed[i] == 0) {
            early_completion = 1;
            break;
        }
    }
    if (early_completion) {
        current_time += time_quantum - (current_time % time_quantum);
    }
}

// Calculate average turnaround time and waiting time
avg_turnaround_time /= num_processes;
avg_waiting_time /= num_processes;

// Write results to output file
fprintf(output_file, "Average turn-around time: %.2f\n", avg_turnaround_time);
fprintf(output_file, "Average waiting time: %.2f\n", avg_waiting_time);

// Print Gantt chart to output file
fprintf(output_file, "Gantt Chart:\n");
for (int i = 0; i < gantt_index; i++) {
    fprintf(output_file, "P%d ", gantt_chart[i]);
}
fprintf(output_file, "\n");

// Print number of bursts for each process to output file
fprintf(output_file, "Number of bursts for each process:\n");
for (int i = 0; i < num_processes; i++) {
    fprintf(output_file, "P%d: %d\n", processes[i].pid, num_bursts[i]);
}

// Close output file
fclose(output_file);

// Free memory allocated for arrays
free(processes);
free(gantt_chart);
free(num_bursts);
free(completed);
}


void srtf()
{
     FILE *input_file = fopen("input.txt", "r");
    FILE *output_file = fopen("output.txt", "w");

    // Read number of processes from input file
    int n;
    fscanf(input_file, "%d", &n);

    // Allocate memory for processes
    struct process *procs = (struct process *)malloc(n * sizeof(struct process));

    // Read process data from input file
    for (int i = 0; i < n; i++) {
        fscanf(input_file, "%d %d %d", &procs[i].pid, &procs[i].arrival_time, &procs[i].burst_time);
        procs[i].remaining_time = procs[i].burst_time;
    }

    // Sort processes by arrival time
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (procs[i].arrival_time > procs[j].arrival_time) {
                struct process temp = procs[i];
                procs[i] = procs[j];
                procs[j] = temp;
            }
        }
    }

    // Initialize variables
    int current_time = 0;
    int completed = 0;
    int min_remaining_time = 0;
    int selected_process = -1;
    int gantt_chart[n * 2];
    int chart_index = 0;

    // Loop until all processes are completed
    while (completed < n) {
        // Select process with shortest remaining time
        min_remaining_time = 999999;
        selected_process = -1;
        for (int i = 0; i < n; i++) {
            if (procs[i].arrival_time <= current_time && procs[i].remaining_time < min_remaining_time && procs[i].remaining_time > 0) {
                min_remaining_time = procs[i].remaining_time;
                selected_process = i;
            }
        }

        // If no process is selected, increment current time
        if (selected_process == -1) {
            current_time++;
            continue;
        }

        // Update process data
        procs[selected_process].remaining_time--;
        current_time++;

        // Add process to Gantt chart
        gantt_chart[chart_index] = procs[selected_process].pid;
        chart_index++;

        // If process is completed, calculate turnaround time and waiting time
        if (procs[selected_process].remaining_time == 0) {
            procs[selected_process].turnaround_time = current_time - procs[selected_process].arrival_time;
            procs[selected_process].waiting_time = procs[selected_process].turnaround_time - procs[selected_process].burst_time;
            completed++;
        }
    }

    // Calculate average turnaround time and waiting time
    float avg_turnaround_time = 0;
    float avg_waiting_time = 0;
    for (int i = 0; i < n; i++) {
        avg_turnaround_time += procs[i].turnaround_time;
        avg_waiting_time += procs[i].waiting_time;
    }

    avg_turnaround_time /= n;
avg_waiting_time /= n;

// Write output to output file
fprintf(output_file, "Average turn-around time: %0.2f\n", avg_turnaround_time);
fprintf(output_file, "Average waiting time: %0.2f\n", avg_waiting_time);

// Print Gantt chart
fprintf(output_file, "Gantt chart:\n");
for (int i = 0; i < chart_index; i++) {
    fprintf(output_file, "P%d ", gantt_chart[i]);
}
fprintf(output_file, "\n");

// Free memory and close files
free(procs);
fclose(input_file);
fclose(output_file);

}

void prio()
{
    FILE *input_file, *output_file;
    input_file = fopen("input.txt", "r");
    output_file = fopen("output.txt", "w");
    struct process *procs;
    int n;
    fscanf(input_file, "%d", &n);

    procs = (struct process*) malloc(n * sizeof(struct process));
    for (int i = 0; i < n; i++) {
        fscanf(input_file, "%d %d %d %d", &procs[i].pid, &procs[i].arrival_time, &procs[i].burst_time, &procs[i].priority);
        procs[i].waiting_time = 0;
        procs[i].turnaround_time = 0;
    }

    sort_by_arrival_time(procs, n);

    int current_time = procs[0].arrival_time;
    int completed = 0;
    int gantt_chart[100];
    int chart_index = 0;
    int total_waiting_time = 0;
    int total_turnaround_time = 0;

    while (completed < n) {
        int selected_proc = -1;
        int highest_priority = -1;
        for (int i = 0; i < n; i++) {
            if (procs[i].arrival_time <= current_time && procs[i].burst_time > 0) {
                if (procs[i].priority > highest_priority) {
                    highest_priority = procs[i].priority;
                    selected_proc = i;
                }
            }
        }

        if (selected_proc == -1) {
            current_time++;
            continue;
        }

        gantt_chart[chart_index++] = procs[selected_proc].pid;
        procs[selected_proc].burst_time--;

        if (procs[selected_proc].burst_time == 0) {
            completed++;
            procs[selected_proc].completion_time = current_time + 1;
            procs[selected_proc].turnaround_time = procs[selected_proc].completion_time - procs[selected_proc].arrival_time;
            procs[selected_proc].waiting_time = procs[selected_proc].turnaround_time - (procs[selected_proc].burst_time + procs[selected_proc].arrival_time);
            total_waiting_time += procs[selected_proc].waiting_time;
            total_turnaround_time += procs[selected_proc].turnaround_time;
        }

        current_time++;
    }

    fprintf(output_file, "Average turn-around time: %.2f\n", (float) total_turnaround_time / n);
    fprintf(output_file, "Average waiting time: %.2f\n", (float) total_waiting_time / n);
    fprintf(output_file, "\nGantt Chart: ");
    for (int i = 0; i < chart_index; i++) {
        fprintf(output_file, "P%d ", gantt_chart[i]);
    }

    fclose(input_file);
    fclose(output_file);

    free(procs);
}

void lrtf()
{
    FILE *input_file = fopen("input.txt", "r");
    FILE *output_file = fopen("output.txt", "w");
    int num_processes;
    fscanf(input_file, "%d", &num_processes);
    struct process processes[num_processes];
    for (int i = 0; i < num_processes; i++) {
        fscanf(input_file, "%d %d %d",&processes[i].pid, &processes[i].arrival_time, &processes[i].burst_time);
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
    }
    fclose(input_file);
    
    // Implement LRTF scheduling algorithm
    int current_time = 0;
    int completed = 0;
    int prev_process_id = 0;
    int gantt_chart[100];
    int gantt_index = 0;
    while (completed < num_processes) {
        int max_remaining_time = -1;
        int max_remaining_time_index = -1;
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0 &&
                processes[i].remaining_time > max_remaining_time && i != prev_process_id) {
                max_remaining_time = processes[i].remaining_time;
                max_remaining_time_index = i;
            }
        }
        if (max_remaining_time_index != -1) {
            processes[max_remaining_time_index].remaining_time--;
            gantt_chart[gantt_index++] = max_remaining_time_index+1;
            prev_process_id = max_remaining_time_index;
            if (processes[max_remaining_time_index].remaining_time == 0) {
                completed++;
                int completion_time = current_time+1;
                processes[max_remaining_time_index].turnaround_time = completion_time - processes[max_remaining_time_index].arrival_time;
                processes[max_remaining_time_index].waiting_time = processes[max_remaining_time_index].turnaround_time - processes[max_remaining_time_index].burst_time;
            }
        }
        else {
            gantt_chart[gantt_index++] = 0;
        }
        current_time++;
    }
    
    // Write output to file
    
    float avg_turnaround_time = 0;
    float avg_waiting_time = 0;
    for (int i = 0; i < num_processes; i++) {
        avg_turnaround_time += processes[i].turnaround_time;
        avg_waiting_time += processes[i].waiting_time;
    }
    fprintf(output_file, "Average Turnaround Time: %.2f\n", avg_turnaround_time/num_processes);
    fprintf(output_file, "Average Waiting Time: %.2f\n\n", avg_waiting_time/num_processes);
    fprintf(output_file, "Gantt Chart:\n");
     // Print Gantt chart
    for (int i = 0; i < gantt_index; i++) {
        if (gantt_chart[i] == 0) {
            fprintf(output_file, "-- ");
        }
        else {
            fprintf(output_file, "P%d ", gantt_chart[i]);
        }
    }
    
    fclose(output_file);
}


int main() 
{
    int a=1;
    while(1){
    printf("Select a scheduling algorithm:\n\n");
    printf("1. First-Come First-Served\n");
    printf("2. Shortest Job First\n");
    printf("3. Round Robin Scheduling\n");
    printf("4. Priority Scheduling\n");
    printf("5. Shortest Remaining Time First\n");
    printf("6. Longest remaining time first\n");
    printf("7. None of the above\n");
    int choice;
    scanf("%d", &choice);
    switch (choice) {
    case 1: fcfs();
        break;
    case 2: sjf();
        break;
    case 3: rr();
        break;
    case 4: prio();
        break;
    case 5: srtf();
        break;
    case 6: lrtf();
        break;
    case 7:printf("Thank You for using our services...\n");
           a=0;
           break;
    }
    if(a==0)
    {break;}
    }
return 0;
}