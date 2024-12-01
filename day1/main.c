#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

int main(void) {
    puts("Counting!");

    // Read file into source buffer
    char* source = NULL;
    long buffer_size = 0;
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
            source = malloc(sizeof(char) * (buffer_size + 1));

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
                source[new_len++] = '\0'; /* Just to be safe. */
            }
        }
        // close file
        fclose(fp);
    } else {
        printf("Failed to open\n");
    }

    // Parse numbers into list1 and list2 through source text

    char* source_offset = source;
    int list_index = 0;
    // Text + newline
    int line_length = (strchr(source, '\n') - source) + 1;
    // Hack to keep the last line
    int list_size = ceil((double)buffer_size / (double)line_length);
    // printf("Line length: %d\n", line_length);
    // printf("Buffer Size: %d\n", buffer_size);
    // printf("List size: %d\n", list_size);
    int* list1 = malloc(sizeof(int) * list_size);
    int* list2 = malloc(sizeof(int) * list_size);
    // Dubious parse source using sscanf and jumping the string forward
    printf("Parsing\n");
    do {
        int list1_int;
        int list2_int;
        int res = sscanf(source_offset, "%d   %d", &list1_int, &list2_int);
        if (res < 2) {
            printf("sscanf failed");
            return EXIT_FAILURE;
        }

        // printf("list1[%d] = %d\n", list_index, list1_int);
        // printf("list2[%d] = %d\n", list_index, list2_int);

        list1[list_index] = list1_int;
        list2[list_index] = list2_int;
        list_index++;

        // jump to next newline
        char* newline = strchr(source_offset, '\n');
        // printf("pointer %x\n", newline);
        int offset = (newline - source_offset) + 1;
        // printf("offset: %d\n", offset);
        source_offset += offset;
    } while (list_index < list_size);


    // Sort using standard sort
    printf("Sorting\n");
    qsort(list1, list_size, sizeof(int), &compar);
    qsort(list2, list_size, sizeof(int), &compar);


    dict_t dictionary = dict_new();

    int total_distance = 0;
    for (int i = 0; i < list_size; i++) {
        printf("%d %d\n", list1[i], list2[i]);
        total_distance += distance(list1[i], list2[i]);

        // TODO mem inefficient
        char* number_string = malloc(sizeof(char) * 100);
        sprintf(number_string, "%100d", list2[i]);

        int current_count = dict_find(dictionary, number_string, 0);
        dict_add(dictionary, number_string, current_count + 1);
        // printf("%s count %d\n", number_string, current_count + 1);
    }

    int list_similarity = 0;
    for (int i = 0; i < list_size; i++) {
        char number_string[100];
        sprintf(number_string, "%100d", list1[i]);
        int current_sim = list1[i] * dict_find(dictionary, number_string, 0);
        printf("current sim %d * %d\n", list1[i], dict_find(dictionary, number_string, 0));
        list_similarity += current_sim;
    }
    printf("puzzle 1 - total_distance:%d\n", total_distance);
    printf("puzzle 2 - list_similarity:%d\n", list_similarity);
    return EXIT_SUCCESS;
}


