#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int count_chars(const char* string, char c, int max_len) {
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

int* parse_ints(const char* string, int length) {
    int* pages = malloc(sizeof(int) * 30);

    // strtok modifies the string, so make a copy
    char* tmp_str = malloc(sizeof(char) * length + 1);
    memcpy(tmp_str, string, length);
    tmp_str[length] = '\0';

    // Parse pages row
    int page_idx = 0;
    char* current_num_str = strtok(tmp_str, ",");
    while (current_num_str != NULL) {
        int current_num;
        sscanf(current_num_str, "%d", &current_num);
        pages[page_idx] = current_num;
        // printf("%d,", current_num);
        current_num_str = strtok(NULL, ",");
        page_idx++;
    }
    // printf("\n");
    free(tmp_str);
    return pages;
}

int is_valid_page_list(int* pages, int num_pages, int* rules, int num_rules) {
    int is_valid = 1;

    for (int rule_index = 0; rule_index < num_rules; rule_index++) {
        // printf("rule%d\n", rule_index);
        int page_before = rules[rule_index * 2];
        int page_after = rules[rule_index * 2 + 1];
         for (int page_index = 0; page_index < num_pages; page_index++) {
            for (int previous_index = 0; previous_index < page_index; previous_index++) {
                // Page in the wrong order for the current rule
                if (pages[previous_index] == page_after && 
                    pages[page_index] == page_before) {
                    is_valid = 0;
                    // printf("page %d breaks (%d|%d)\n", pages[page_index], page_before, page_after);
                    break;
                }
            }
        }
    }


    return is_valid;
}

int mutate_to_correct_page_list(int* pages, int num_pages, int* rules, int num_rules) {
    int is_valid = 1;
    for (int rule_index = 0; rule_index < num_rules; rule_index++) {
        int page_before = rules[rule_index * 2];
        int page_after = rules[rule_index * 2 + 1];
         for (int page_index = 0; page_index < num_pages; page_index++) {
            for (int previous_index = 0; previous_index < page_index; previous_index++) {
                // Page in the wrong order for the current rule
                if (pages[previous_index] == page_after && 
                    pages[page_index] == page_before) {
                    is_valid = 0;
                    int temp = pages[page_index];
                    pages[page_index] = pages[previous_index];
                    pages[previous_index] = temp;

                    printf("Swapped page list:\n");
                    for (int i = 0; i < num_pages; i++) {
                        printf("%d,", pages[i]);
                    }
                    printf("\n");
                    return is_valid;
                }
            }
        }
    }
    return is_valid;
}

size_t index_of(char* string, char c) {
    char* next_char = strchr(string, c);
    int offset = (next_char - string);
    return offset;
}

int main(void) {
     // Read file into source buffer
    char* source = NULL;
    long buffer_size = 0;
    int lines = 0;
    int width = 0;
    int current_width = 0;
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
        // Rewind
        if (fseek(fp, 0L, SEEK_SET) != 0) {
            fclose(fp);
            return EXIT_FAILURE;
        }

        char ch;
        while(!feof(fp))
        {
            ch = fgetc(fp);
            if(ch == '\n')
            {
                lines++;
                width = current_width;
                current_width = 0;
            } else {
                current_width++;
            }
        }

        fclose(fp);
    } else {
        printf("Failed to open\n");
    }

    printf("File lines: %d\n", lines);

    // TODO parse rules
    char * rules = source;
    int* parsed_rules = malloc(sizeof(int) * 2 * lines);
    int num_rules = 0;
    while(rules[0] != '\n' && rules[1] != '\n') {
        int newline_index = index_of(rules, '\n') + 1;

        int before;
        int after;
        sscanf(rules, "%d|%d", &before, &after);
        printf("rule: %d -> %d\n", before, after);
        parsed_rules[num_rules*2] = before;
        parsed_rules[num_rules*2+1] = after;
        num_rules++;
        rules += newline_index;
    }
    printf("Num rules: %d\n", num_rules);

    // Print parsed rules
    // for (int rule_index = 0; rule_index < num_rules; rule_index++) {
    //     int page_before = parsed_rules[rule_index * 2];
    //     int page_after = parsed_rules[rule_index * 2 + 1];
    //     printf("(%d before %d)\n", page_before, page_after);
    // }

    // Process page lines and track success
    char* pages_list = rules + 1;
    int midpoint_sum = 0;
    int midpoint_sum_of_rearranged = 0;
    while(pages_list[0] != '\n' && pages_list[1] != '\n') {
        int newline_index = index_of(pages_list, '\n') + 1;

        int num_pages = count_chars(pages_list, ',', newline_index) + 1;
        printf("Num pages: %d\n", num_pages);

        int* pages = parse_ints(pages_list, newline_index);
        // for (int i = 0; i < num_pages; i++) {
        //     printf("%d,", pages[i]);
        // }
        // printf("\n");
        
        int valid = is_valid_page_list(pages, num_pages, parsed_rules, num_rules);
        if (valid) {
            // printf("Valid: ");
            // for (int i = 0; i < num_pages; i++) {
            //     printf("%d,", pages[i]);
            // }
            // printf("\n");
            int midpoint = num_pages / 2;
            midpoint_sum += pages[midpoint];
        } else {
            mutate_to_correct_page_list(pages, num_pages, parsed_rules, num_rules);
            int max_iter = num_pages * 10;
            while (!is_valid_page_list(pages, num_pages, parsed_rules, num_rules) && max_iter >= 0) {
                mutate_to_correct_page_list(pages, num_pages, parsed_rules, num_rules);
                max_iter--;
            }
            int midpoint = num_pages / 2;
            midpoint_sum_of_rearranged += pages[midpoint];
        }

        pages_list += newline_index;
        free(pages);
    }

    printf("midpoint sum: %d\n", midpoint_sum);
    printf("midpoint sum re-ordered: %d\n", midpoint_sum_of_rearranged);

    // Sum middle number in line
    return EXIT_SUCCESS;
}