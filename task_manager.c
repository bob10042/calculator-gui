#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_TASK_DESC 100
#define MAX_LINE 150
#define MAX_STATUS 20

// Improved error handling macro
#define HANDLE_MEMORY_ERROR(ptr) \
    do { \
        if ((ptr) == NULL) { \
            fprintf(stderr, "Memory allocation failed at %s:%d\n", __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while(0)

// Structure for a task
typedef struct Task {
    int id;
    char description[MAX_TASK_DESC];
    int priority;
    char status[MAX_STATUS];
    char date[11];
    struct Task* next;
} Task;

// Global variables
Task* taskList = NULL;
int nextId = 1;

// Function prototypes (moved to top for full declaration)
char* getCurrentDate();
Task* createTask(int id, const char* desc, int priority, const char* status, const char* date);
void trimWhitespace(char* str);
int validateInput(const char* input, int maxLen);
void saveToFile();
void loadFromFile();
void clearTaskList();
void viewTasks();
void addTask();
void updateTaskStatus();
void deleteTask();

// Trim whitespace from string
void trimWhitespace(char* str) {
    char* start = str;
    char* end = str + strlen(str) - 1;

    // Trim leading whitespace
    while (isspace(*start)) start++;

    // Trim trailing whitespace
    while (end > start && isspace(*end)) end--;

    // Null terminate
    *(end + 1) = '\0';

    // Move trimmed string to beginning if needed
    if (start != str) {
        memmove(str, start, end - start + 2);
    }
}

// Validate input length
int validateInput(const char* input, int maxLen) {
    if (input == NULL) return 0;
    
    size_t len = strlen(input);
    return (len > 0 && len < (size_t)maxLen);
}

// Get current date in YYYY-MM-DD format
char* getCurrentDate() {
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    static char date[11];
    
    if (tm == NULL) {
        fprintf(stderr, "Failed to get current time\n");
        strcpy(date, "0000-00-00");
        return date;
    }
    
    strftime(date, sizeof(date), "%Y-%m-%d", tm);
    return date;
}

// Create a new task with enhanced error checking
Task* createTask(int id, const char* desc, int priority, const char* status, const char* date) {
    // Validate inputs
    if (!validateInput(desc, MAX_TASK_DESC) || 
        priority < 1 || priority > 5 || 
        !validateInput(status, MAX_STATUS)) {
        fprintf(stderr, "Invalid task parameters\n");
        return NULL;
    }

    Task* newTask = (Task*)malloc(sizeof(Task));
    HANDLE_MEMORY_ERROR(newTask);

    // Safe string copying with trimming
    char trimmedDesc[MAX_TASK_DESC];
    strncpy(trimmedDesc, desc, MAX_TASK_DESC - 1);
    trimmedDesc[MAX_TASK_DESC - 1] = '\0';
    trimWhitespace(trimmedDesc);

    newTask->id = id;
    strncpy(newTask->description, trimmedDesc, MAX_TASK_DESC - 1);
    newTask->description[MAX_TASK_DESC - 1] = '\0';
    
    newTask->priority = (priority >= 1 && priority <= 5) ? priority : 5;
    
    strncpy(newTask->status, status, MAX_STATUS - 1);
    newTask->status[MAX_STATUS - 1] = '\0';
    
    strncpy(newTask->date, date, 10);
    newTask->date[10] = '\0';
    
    newTask->next = NULL;
    return newTask;
}

// View all tasks
void viewTasks() {
    if (taskList == NULL) {
        printf("\nNo tasks found!\n");
        return;
    }
    
    printf("\n%-5s %-40s %-10s %-10s %-12s\n", "ID", "Description", "Priority", "Status", "Date");
    printf("----------------------------------------------------------------\n");
    
    Task* current = taskList;
    while (current != NULL) {
        printf("%-5d %-40s %-10d %-10s %-12s\n",
               current->id,
               current->description,
               current->priority,
               current->status,
               current->date);
        current = current->next;
    }
}

// Add a new task with improved input handling
void addTask() {
    char desc[MAX_TASK_DESC];
    int priority;
    
    printf("\nEnter task description (max %d chars): ", MAX_TASK_DESC - 1);
    getchar(); // Clear input buffer
    
    if (fgets(desc, sizeof(desc), stdin) == NULL) {
        printf("Input error. Task not added.\n");
        return;
    }
    
    // Remove trailing newline
    desc[strcspn(desc, "\n")] = 0;
    trimWhitespace(desc);
    
    if (strlen(desc) == 0) {
        printf("Description cannot be empty.\n");
        return;
    }
    
    printf("Enter priority (1-5, 1 being highest): ");
    if (scanf("%d", &priority) != 1) {
        printf("Invalid input. Setting priority to lowest (5).\n");
        priority = 5;
    }
    
    // Validate and clamp priority
    priority = (priority < 1) ? 5 : (priority > 5) ? 5 : priority;
    
    Task* newTask = createTask(nextId++, desc, priority, "Pending", getCurrentDate());
    
    if (newTask == NULL) {
        printf("Failed to create task.\n");
        return;
    }
    
    // Add to list in sorted order (by priority)
    if (taskList == NULL || taskList->priority > priority) {
        newTask->next = taskList;
        taskList = newTask;
    } else {
        Task* current = taskList;
        while (current->next != NULL && current->next->priority <= priority) {
            current = current->next;
        }
        newTask->next = current->next;
        current->next = newTask;
    }
    
    printf("Task added successfully!\n");
    saveToFile(); // Automatically save after adding
}

// Update task status
void updateTaskStatus() {
    int id;
    printf("\nEnter task ID to update: ");
    scanf("%d", &id);
    
    Task* current = taskList;
    while (current != NULL) {
        if (current->id == id) {
            printf("Current status: %s\n", current->status);
            printf("Enter new status (Pending/In Progress/Completed): ");
            scanf("%s", current->status);
            printf("Status updated successfully!\n");
            saveToFile(); // Automatically save after updating
            return;
        }
        current = current->next;
    }
    printf("Task not found!\n");
}

// Delete a task
void deleteTask() {
    int id;
    printf("\nEnter task ID to delete: ");
    scanf("%d", &id);
    
    if (taskList == NULL) {
        printf("No tasks to delete!\n");
        return;
    }
    
    if (taskList->id == id) {
        Task* temp = taskList;
        taskList = taskList->next;
        free(temp);
        printf("Task deleted successfully!\n");
        saveToFile(); // Automatically save after deleting
        return;
    }
    
    Task* current = taskList;
    while (current->next != NULL) {
        if (current->next->id == id) {
            Task* temp = current->next;
            current->next = current->next->next;
            free(temp);
            printf("Task deleted successfully!\n");
            saveToFile(); // Automatically save after deleting
            return;
        }
        current = current->next;
    }
    printf("Task not found!\n");
}

// Save tasks to file
void saveToFile() {
    FILE* file = fopen("tasks.txt", "w");
    if (file == NULL) {
        printf("Error opening file for writing!\n");
        return;
    }
    
    Task* current = taskList;
    while (current != NULL) {
        fprintf(file, "%d|%s|%d|%s|%s\n",
                current->id,
                current->description,
                current->priority,
                current->status,
                current->date);
        current = current->next;
    }
    
    fclose(file);
}

// Load tasks from file
void loadFromFile() {
    FILE* file = fopen("tasks.txt", "r");
    if (file == NULL) {
        printf("No existing tasks file found.\n");
        return;
    }
    
    clearTaskList(); // Clear existing list before loading
    
    char line[MAX_LINE];
    int maxId = 0;
    
    while (fgets(line, MAX_LINE, file)) {
        char desc[MAX_TASK_DESC];
        int id, priority;
        char status[20], date[11];
        
        sscanf(line, "%d|%[^|]|%d|%[^|]|%s\n",
               &id, desc, &priority, status, date);
        
        Task* newTask = createTask(id, desc, priority, status, date);
        
        // Keep track of highest ID for nextId
        if (id > maxId) maxId = id;
        
        // Add to list in sorted order
        if (taskList == NULL || taskList->priority > priority) {
            newTask->next = taskList;
            taskList = newTask;
        } else {
            Task* current = taskList;
            while (current->next != NULL && current->next->priority <= priority) {
                current = current->next;
            }
            newTask->next = current->next;
            current->next = newTask;
        }
    }
    
    nextId = maxId + 1;
    fclose(file);
}

// Clear the task list
void clearTaskList() {
    while (taskList != NULL) {
        Task* temp = taskList;
        taskList = taskList->next;
        free(temp);
    }
}

// Main function
int main() {
    int choice;
    
    loadFromFile(); // Load existing tasks at startup
    
    while (1) {
        printf("\nTask Manager Menu:\n");
        printf("1. Add Task\n");
        printf("2. View Tasks\n");
        printf("3. Update Task Status\n");
        printf("4. Delete Task\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        
        if (scanf("%d", &choice) != 1) {
            // Clear input buffer on invalid input
            while (getchar() != '\n');
            printf("\nInvalid input! Please enter a number.\n");
            continue;
        }
        
        switch (choice) {
            case 1:
                addTask();
                break;
            case 2:
                viewTasks();
                break;
            case 3:
                updateTaskStatus();
                break;
            case 4:
                deleteTask();
                break;
            case 5:
                printf("\nExiting...\n");
                clearTaskList();
                return 0;
            default:
                printf("\nInvalid choice! Please try again.\n");
        }
    }
    
    return 0;
}
