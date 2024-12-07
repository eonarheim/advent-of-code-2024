#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

typedef struct grid_s {
    int rows;
    int columns;
    int guard_x;
    int guard_y;
    char heading;
    char* grid_data;
} grid_s, *grid_t;



char grid_get(grid_t grid, int x, int y) {
    if (x < 0 || y < 0 || x >= grid->columns || y >= grid->rows) {
        return -1;
    }
    char c = grid->grid_data[x + y *grid->columns];
    return c;
}

void grid_set(grid_t grid, int x, int y, char c) {
    if (x < 0 || y < 0 || x >= grid->columns || y >= grid->rows) {
        return;
    }
    grid->grid_data[x + y *grid->columns] = c;
}

void grid_print(grid_t grid) {
    printf("\n");
    for (int y = 0; y < grid->rows; y++) {
        for (int x = 0; x < grid->columns; x++) {
            printf("%c ", grid_get(grid, x, y));
        }
        printf("\n");
    }
}

int move_and_turn(grid_t grid) {
    // advance the guard and turn
    int dir_x = 0;
    int dir_y = 0;
    if (grid->heading == '^') {
        dir_y = -1;
    }
    if (grid->heading == 'v') {
        dir_y = 1;
    }
    if (grid->heading == '<') {
        dir_x = -1;
    }
    if (grid->heading == '>') {
        dir_x = 1;
    }

    // move and mark traversal
    int max_iter = MAX(grid->rows, grid->columns);
    while (grid_get(grid, grid->guard_x + dir_x, grid->guard_y + dir_y) != '#' &&
           grid_get(grid, grid->guard_x + dir_x, grid->guard_y + dir_y) != 'O') {
        grid_set(grid, grid->guard_x, grid->guard_y, 'X');
        grid->guard_x += dir_x;
        grid->guard_y += dir_y;
        if (max_iter-- < 0) {
            return 1;
        }
    }

    // turn 90 clockwise
    if (grid->heading == '^') {
        grid->heading = '>';
    } else if (grid->heading == 'v') {
        grid->heading = '<';
    } else if (grid->heading == '<') {
        grid->heading = '^';
    } else if (grid->heading == '>') {
        grid->heading = 'v';
    }

    grid_set(grid, grid->guard_x, grid->guard_y, grid->heading);

    return 0;
}

int grid_is_guard_on_board(grid_t grid) {
    return grid_get(grid, grid->guard_x, grid->guard_y) != -1;
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

    printf("Grid %dx%d\n", width, lines);

    char * grid_data = malloc(sizeof(char) * width * lines);
    int grid_index = 0;
    int initial_guard_x = 0;
    int initial_guard_y = 0;
    char initial_heading = '^'; // possible headings ^ > < v
    for (int i = 0; i < (buffer_size + 1); i++) {
        if (source[i] != '\n' && source[i] != '\0') {
            char item = source[i];
            grid_data[grid_index] = item;

            int x = grid_index % lines;
            int y = grid_index / lines;
            if (item == '^' || 
                item == '>' ||
                item == '<' || 
                item == 'v') {
                initial_guard_x = x;
                initial_guard_y = y;
                initial_heading = '^';
            }

            grid_index++;
        }
    }

    grid_s grid = {
        .columns = width,
        .rows = lines,
        .grid_data = grid_data,
        .guard_x = initial_guard_x,
        .guard_y = initial_guard_y,
        .heading = initial_heading
    };

    char * grid_data_search = malloc(sizeof(char) * width * lines);
    memcpy(grid_data_search, grid_data, sizeof(char) * width * lines);

    grid_s grid_search = {
        .columns = width,
        .rows = lines,
        .grid_data = grid_data_search,
        .guard_x = initial_guard_x,
        .guard_y = initial_guard_y,
        .heading = initial_heading
    };

    printf("Initial grid\n");
    grid_print(&grid);

    while (move_and_turn(&grid) != 1) {
        
        // move_and_turn(&grid);
        // grid_print(&grid);
    }

    printf("final grid\n");
    grid_print(&grid);

    int count_traversed = 0;
    for (int i = 0; i < (grid.columns * grid.rows); i++) {
        if (grid.grid_data[i] == 'X') {
            count_traversed++; 
        }
    }
    printf("Traversed: %d\n", count_traversed);

    // naive exhaustive search for loops
    

    int loops = 0;
    for (int y = 0; y < grid_search.rows; y++) {
        for (int x = 0; x < grid_search.columns; x++) {
            if (grid_get(&grid_search, x, y) == '.' ||
                grid_get(&grid_search, x, y) == 'X') {
                grid_set(&grid_search, x, y, 'O');

                // grid_print(&grid_search);
                // perimeter of the grid is our max
                int max_iter = 2 * (MAX(grid.rows, grid.columns) + MAX(grid.rows, grid.columns));
                while(move_and_turn(&grid_search) != 1 && max_iter-- > 0) {}
                // printf("checked (%d, %d)\n", x, y);
                if (grid_is_guard_on_board(&grid_search)) {
                    loops++;
                    printf("loop @ (%d, %d)\n", x, y);
                    // grid_print(&grid_search);
                }
                // reset
                grid_set(&grid_search, grid_search.guard_x, grid_search.guard_y, '.');
                grid_search.guard_x = initial_guard_x;
                grid_search.guard_y = initial_guard_y;
                grid_search.heading = initial_heading;
                grid_set(&grid_search, grid_search.guard_x, grid_search.guard_y, initial_heading);
                // grid_print(&grid_search);
                grid_set(&grid_search, x, y, '.');
            }
        }
    }

    printf("Loops found: %d\n", loops);

}