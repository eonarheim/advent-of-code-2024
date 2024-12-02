#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

typedef struct dict_entry_s {
    const char *key;
    int value;
} dict_entry_s;

typedef struct dict_s {
    int len;
    int cap;
    dict_entry_s *entry;
} dict_s, *dict_t;

int dict_find_index(dict_t dict, const char *key) {
    for (int i = 0; i < dict->len; i++) {
        if (strcmp(dict->entry[i].key, key) == 0) {
            // printf("found key %s\n", key);
            return i;
        }
    }
    return -1;
}

int dict_find(dict_t dict, const char *key, int default_val) {
    int idx = dict_find_index(dict, key);
    return idx == -1 ? default_val : dict->entry[idx].value;
}

void dict_add(dict_t dict, const char *key, int value) {
   int idx = dict_find_index(dict, key);
   if (idx != -1) {
    //    printf("Updating value %d to %d\n", dict->entry[idx].value, value);
       dict->entry[idx].value = value;
       return;
   }
   if (dict->len == dict->cap) {
       dict->cap *= 2;
       dict->entry = realloc(dict->entry, dict->cap * sizeof(dict_entry_s));
   }
   dict->entry[dict->len].key = strdup(key);
   dict->entry[dict->len].value = value;
   dict->len++;
}

dict_t dict_new(void) {
    dict_s proto = {0, 10, malloc(10 * sizeof(dict_entry_s))};
    dict_t d = malloc(sizeof(dict_s));
    *d = proto;
    return d;
}

void dict_free(dict_t dict) {
    for (int i = 0; i < dict->len; i++) {
        free((void*) dict->entry[i].key);
    }
    free((void*) dict->entry);
    free(dict);
}


int distance(int left, int right) {
    return abs(left - right);
}

int compar (const void* a, const void* b) {
    return ( *(int*)a - *(int*)b );
}

int count_char(const char* string, char c, int max_len) {
    int count = 0;
    int index = 0;
    while (string[index] != EOF && string[index] != '\0' && index < max_len) {
        if (string[index] == c) {
            count++;
        }
        index++;
    }

    return count;
}

int find_fault_index(int* reports, int num_reports, int skip) {
    int* parse_reports = reports;
    if (skip != -1) {
        parse_reports = malloc(sizeof(int) * (num_reports - 1));
        int new_idx = 0;
        printf("Skipping %d: ", skip);
        for (int i = 0; i < num_reports; i++) {
            if (i == skip) continue;
            parse_reports[new_idx] = reports[i];
            new_idx++;
            printf("%d ", reports[i]);
        }
        printf("\n");
        num_reports = num_reports - 1;
    }

    int monotonic_fault = -1;
    // monotonic decreasing
    int monotonic_d = 1;
    for (int i = 0; i < (num_reports - 1); i++) {
        if (parse_reports[i] > parse_reports[i+1]) {
            monotonic_d &= 1;
        } else {
            monotonic_d = 0;
            monotonic_fault = MAX(i, monotonic_fault);
            break;
        }
    }
    // monotonic increasing
    int monotonic_i = 1;
    for (int i = 0; i < (num_reports - 1); i++) {
        if (parse_reports[i] < parse_reports[i+1]) {
            monotonic_i &= 1;
        } else {
            monotonic_i = 0;
            monotonic_fault = MAX(i, monotonic_fault);
            break;
        }
    }

    // in range
    int in_range = 1;
    int range_fault = -1;
    for (int i = 0; i < (num_reports - 1); i++) {
        if ((parse_reports[i+1] <= parse_reports[i] + 3) && (parse_reports[i+1] >= parse_reports[i] - 3)) {
            in_range &= 1;
        } else {
            in_range = 0;
            range_fault = i;
            break;
        }
    }

    if (!(monotonic_i | monotonic_d)) {
        return monotonic_fault;
    }
    if (!in_range) {
        return range_fault;
    }

    return -1;
}

int main(void) {
    puts("Counting!");

    // Read file into source buffer
    char* source = NULL;
    long buffer_size = 0;
    int lines = 0;
    FILE* fp = fopen("input.txt", "r");
    if (fp != NULL) {
        // Jump to end to find out how big the file is in bytes
        if (fseek(fp, 0L, SEEK_END) == 0) {
            // buffer size in bytes
            buffer_size = ftell(fp);
            if (buffer_size == -1) {
                fclose(fp);
                return EXIT_FAILURE;
            }
            // alloc a buffer to the size+1 to null terminate
            source = malloc(sizeof(char) * buffer_size + 1);

            if (fseek(fp, 0L, SEEK_SET) != 0) {
                fclose(fp);
                return EXIT_FAILURE;
            }

            // returns number of bytes
            size_t new_len = fread(source, sizeof(char), buffer_size, fp);

            if (ferror( fp ) != 0) {
                printf("Failed to open\n");
                fclose(fp);
                return EXIT_FAILURE;
            } else {
                source[new_len] = '\n'; /* Just to be safe. */
                source[new_len + 1] = '\0'; /* Just to be safe. */
            }
        }

        if (fseek(fp, 0L, SEEK_SET) != 0) {
            fclose(fp);
            return EXIT_FAILURE;
        }
        char ch;
        while(!feof(fp))
        {
            ch = fgetc(fp);
            if(ch == '\n' || ch == EOF)
            {
                lines++;
            }
        }
        // close file
        fclose(fp);
    } else {
        printf("Failed to open\n");
    }

    printf("File lines: %d\n", lines);

    char* source_offset = source;
    int list_index = 0;
    int* list1 = malloc(sizeof(int) * lines);

    // Dubious parse source using sscanf and jumping the string forward
    printf("Parsing\n");
    do {
        // jump to next newline
        char* newline = strchr(source_offset, '\n');
        // printf("newline %x\n", newline);
        int offset = (newline - source_offset) + 1;

        int line_length = offset;
        // printf("Line length: %d\n", line_length);

        int spaces = count_char(source_offset, ' ', line_length);
        // printf("Num numbers %d\n", spaces + 1);
        
        int* report = malloc(sizeof(int) * (spaces + 1));
        
        // strtok modifies the string, so make a copy
        char* tmp_str = malloc(sizeof(char) * line_length + 1);
        memcpy(tmp_str, source_offset, line_length);
        tmp_str[line_length] = '\0';

        // Parse report row
        int report_idx = 0;
        char* current_num_str = strtok(tmp_str, " ");
        int current_num;
        do {
            sscanf(current_num_str, "%d", &current_num);
            report[report_idx++] = current_num;
            printf("%d ", current_num);
            current_num_str = strtok(NULL, " ");
        }  while (current_num_str != NULL);

        int num_reports = (spaces + 1);

        int faulted_index = find_fault_index(report, num_reports, -1);
        if (faulted_index == -1) {
            list1[list_index] = 1;
        } else {
            // second chance
            int start_index = 0;
            while (start_index < num_reports) {
                faulted_index = find_fault_index(report, num_reports, start_index);
                if (faulted_index == -1) {
                    list1[list_index] = 1;
                    break;
                } else {
                    list1[list_index] = 0;
                }
                start_index++;
            }
        }

        // printf("Safe[%d] = %d\n", list_index, (monotonic_i | monotonic_d) & in_range);
        // printf("s: %d, i: %d, d: %d, r: %d, f: %d", (monotonic_i | monotonic_d) & in_range, monotonic_i, monotonic_d, in_range, num_faults_r + num_faults_i + num_faults_d);
        printf("\n");

        list_index++;
        source_offset += offset;

        free(tmp_str);
        free(report);
    } while (list_index < lines);


    int safe_reports = 0;
    for (int i = 0; i < lines; i++) {
        safe_reports += list1[i];
    }
    printf("Safe reports: %d\n", safe_reports);
    return EXIT_SUCCESS;
}


