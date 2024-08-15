#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TASK_MAX 100

typedef struct{
    char *task;
    int completed; // 1 represent true and 0 -> false
} Task;

Task *tasks = NULL;
int length = 0;
const char *filename = "tasks.dat"; // Name of the file to store tasks

void listTasks();
int addTask(const char *task);
int markCompleted(int index);
int deleteTask(int index);
int editTask(int index, const char *task);
void saveTasksToFile();
void loadTasksFromFile();
void freeAll();
void cleanupAndExit(int signal);

int main(){

    signal(SIGINT, cleanupAndExit);

    // Load saved tasks from the file
    loadTasksFromFile();
    
    int running = 1;
    int option, indexInput;
    char taskInput[TASK_MAX];

    puts("//// OPTIONS ////");
    puts("-----------------");

    puts("1 - List all tasks");
    puts("2 - Add task");
    puts("3 - Mark completed task");
    puts("4 - Edit task");
    puts("5 - Delete task");
    puts("6 - Exit");

    while (running)
    {
        printf("Enter the section number 1-6: ");
        scanf("%d", &option);

        switch (option)
        {
        case 1:
            listTasks();
            break;
        
        case 2:
            printf("Enter the task: ");
            getchar();
            fgets(taskInput, sizeof(taskInput), stdin);
            taskInput[strcspn(taskInput, "\n")] = '\0';

            addTask(taskInput);
            break;
        
        case 3:
            printf("Enter the index of task: ");
            scanf("%d", &indexInput);

            markCompleted(indexInput);
            break;
        
        case 4:
            printf("Enter the index of task: ");
            scanf("%d", &indexInput);
            printf("Enter the task: ");
            getchar();
            fgets(taskInput, sizeof(taskInput), stdin);
            taskInput[strcspn(taskInput, "\n")] = '\0';

            editTask(indexInput, taskInput);
            break;
        
        case 5:
            printf("Enter the index of task: ");
            scanf("%d", &indexInput);

            deleteTask(indexInput);
            break;
        
        case 6:
            running = 0;
            break;
        
        default:
            puts("Invalid choice.");
            break;
        }
    }

    saveTasksToFile();
    freeAll();

    return 0;
}



void listTasks(){
    if (length == 0)
    {
        puts("There is not tasks to show");
        return;
    }
    
    for (size_t i = 0; i < length; i++)
    {
       printf("%zu => %s / state => %s\n", i + 1, tasks[i].task, tasks[i].completed ? "Completed" : "Uncompleted");
    }
}

int addTask(const char *task){

    tasks = (Task*)realloc(tasks, (length + 1) * sizeof(Task));

    if (tasks != NULL){
        tasks[length].task = (char *)malloc(strlen(task) + 1);
        strcpy(tasks[length].task, task);

        tasks[length].completed = 0;

        length++;

        puts("Task Added Successfully.");
        return 0;
    }

    puts("Failed to add the task");
    return -1;    
}

int markCompleted(int index){
    if (index <= length && index > 0)
    {
        index--;

        tasks[index].completed = 1;
        puts("Task marked as completed");
        return 0;
    }

    puts("Number of task out of range, please try again");
    return -1;
}
int deleteTask(int index){
    if (index <= length && index > 0)
    {
        index--;

        free(tasks[index].task);

        for (size_t i = index; i < length - 1; i++)
        {
            tasks[i] = tasks[i + 1];
        }

        length--;
        tasks = (Task*)realloc(tasks, (length * sizeof(Task)));

        if (tasks != NULL){
            puts("Task deleted successfully");
            return 0;
        }

        puts("Failed to delete the task");
        return -1;
    }

    puts("Number of task out of range, please try again");
    return -1;
}

int editTask(int index, const char *task){
    if (index <= length && index > 0)
    {
        index--;

        char *newTask = realloc(tasks[index].task, strlen(task) + 1);

        if (newTask != NULL)
        {
            tasks[index].task = newTask;
            strcpy(tasks[index].task, task);

            puts("Task updated successfully");
            return 0;
        }
        puts("Failed to update the task");
        return -1;
    }

    puts("Number of task out of range, please try again");
    return -1;
};

void saveTasksToFile(){
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        puts("Failed save tasks!!");
        return;
    }

    fprintf(file, "%d\n", length);

    for (int i = 0; i < length; i++) {
        fprintf(file, "%d\n", tasks[i].completed);
        fprintf(file, "%s\n", tasks[i].task);
    }

    fclose(file);
    puts("Tasks saved successfully.");
}

void loadTasksFromFile(){
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        puts("Failed get tasks!!");
        return;
    }

    fscanf(file, "%d", &length);

    tasks = (Task*)malloc(length * sizeof(Task));
    if (tasks == NULL) {
        puts("Failed to allocate memory for tasks.");
        fclose(file);
        return;
    }

    for (size_t i = 0; i < length; i++)
    {
        int completed;
        char task[TASK_MAX];

        fscanf(file, "%d", &completed);
        fgetc(file);

        if (fgets(task, sizeof(task), file) == NULL) {
            break;
        }
        task[strcspn(task, "\n")] = '\0';

        tasks[i].task = malloc(strlen(task) + 1);
        if (tasks[i].task == NULL) {
            puts("Failed to allocate memory.");
            break;
        }

        strcpy(tasks[i].task, task);
        tasks[i].completed = completed;
    }
    
    fclose(file);
    puts("Tasks loaded successfully.");
}

void freeAll(){
    for (int i = 0; i < length; i++) {
        free(tasks[i].task);
    }

    free(tasks);
}


void cleanupAndExit(int signal) {
    // Perform cleanup operations
    saveTasksToFile();
    freeAll();

    puts("\nProgram interrupted. Tasks saved.");
    exit(0);
}