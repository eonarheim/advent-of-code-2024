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

int parse(char* source, size_t source_length) {
    // read 1 character at a time
    int index = 0;
    int total = 0;
    int enabled = 1;
    while (index < source_length) {
        //printf("i: %d t: %d\n", index, source_length);
        char current = source[index];
        switch (current)
        {
            case '\0': {
                /* code */
                // break;
            }
            case 'd': {
                // peek for do() or don't()

                if (index + 3 < source_length &&
                    strncmp(source + index, "do()", 4) == 0) {
                    printf("enabled\n");
                    enabled = 1;
                }
                if (index + 6 < source_length &&
                    strncmp(source + index, "don\'t()", 7) == 0) {

                    printf("disabled\n");
                    enabled = 0;
                }
            }
            case 'm': {
                // peek for mul(
                if (index+3 < source_length && 
                    strncmp(source + index, "mul(", 4) == 0) {
                    int a;
                    int b;
                    int n = 0;
                    int found = sscanf(source + index, "mul(%d,%d)%n", &a, &b, &n);
                    if (found == 2 && n > 0 && enabled) {
                        printf("mul(%d,%d)\n", a, b);
                        total += (a * b);
                    }
                }
            }
        }
        index++;
    }
    return total;
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
        fclose(fp);
    } else {
        printf("Failed to open\n");
    }

    printf("File lines: %d\n", lines);

    int total = parse(source, buffer_size);

    printf("Total: %d", total);
    return EXIT_SUCCESS;
}


