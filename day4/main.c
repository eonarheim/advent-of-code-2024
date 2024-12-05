#include <stdio.h>
#include <stdlib.h>

typedef struct grid_s {
    int rows;
    int columns;
    char* grid_data;
} grid_s, *grid_t;

char grid_get(grid_t grid, int x, int y) {
    if (x < 0 || y < 0 || x >= grid->columns || y >= grid->rows) {
        // printf("(%d,%d) not found\n");
        return -1;
    }
    char c = grid->grid_data[x + y *grid->columns];
    // printf("(%d,%d) = %c\n", x, y, c);
    return c;
}

/**
 * Returns number of xmas found left and right from (x,y)
 */
int grid_find_xmas_horizontal(grid_t grid, int x, int y) {
    int count = 0;
    if (grid_get(grid, x, y) == 'X' &&
        grid_get(grid, x+1, y) == 'M' &&
        grid_get(grid, x+2, y) == 'A' &&
        grid_get(grid, x+3, y) == 'S'
    ) {
        count++;
    }

    if (grid_get(grid, x, y) == 'X' &&
        grid_get(grid, x-1, y) == 'M' &&
        grid_get(grid, x-2, y) == 'A' &&
        grid_get(grid, x-3, y) == 'S'
    ) {
        count++;
    }
    return count;
}

/**
 * Returns number of xmas found up and down from (x,y)
 */
int grid_find_xmas_vertical(grid_t grid, int x, int y) {
    int count = 0;
    if (grid_get(grid, x, y) == 'X' &&
        grid_get(grid, x, y+1) == 'M' &&
        grid_get(grid, x, y+2) == 'A' &&
        grid_get(grid, x, y+3) == 'S'
    ) {
        count++;
    }

    if (grid_get(grid, x, y) == 'X' &&
        grid_get(grid, x, y-1) == 'M' &&
        grid_get(grid, x, y-2) == 'A' &&
        grid_get(grid, x, y-3) == 'S'
    ) {
        count++;
    }
    return count;
}

/**
 * Returns number of xmas found up and down from (x,y)
 */
int grid_find_xmas_diagonals(grid_t grid, int x, int y) {
    int count = 0;
    if (grid_get(grid, x, y) == 'X' &&
        grid_get(grid, x+1, y+1) == 'M' &&
        grid_get(grid, x+2, y+2) == 'A' &&
        grid_get(grid, x+3, y+3) == 'S'
    ) {
        count++;
    }

    if (grid_get(grid, x, y) == 'X' &&
        grid_get(grid, x-1, y-1) == 'M' &&
        grid_get(grid, x-2, y-2) == 'A' &&
        grid_get(grid, x-3, y-3) == 'S'
    ) {
        count++;
    }

    if (grid_get(grid, x, y) == 'X' &&
        grid_get(grid, x+1, y-1) == 'M' &&
        grid_get(grid, x+2, y-2) == 'A' &&
        grid_get(grid, x+3, y-3) == 'S'
    ) {
        count++;
    }

    if (grid_get(grid, x, y) == 'X' &&
        grid_get(grid, x-1, y+1) == 'M' &&
        grid_get(grid, x-2, y+2) == 'A' &&
        grid_get(grid, x-3, y+3) == 'S'
    ) {
        count++;
    }
    return count;
}

/**
 * Returns number of xmas found from (x,y) in all directions
 */
int grid_find_xmas(grid_t grid, int x, int y) {
    return grid_find_xmas_horizontal(grid, x, y) +
           grid_find_xmas_vertical(grid, x, y) +
           grid_find_xmas_diagonals(grid, x, y);
}

int grid_find_cross_mas(grid_t grid, int x, int y) {
    /**
     * M.M
     * .A.
     * S.S 
     */
    if (grid_get(grid, x, y) == 'A' &&
        grid_get(grid, x-1, y-1) == 'M' &&
        grid_get(grid, x+1, y+1) == 'S' &&
        grid_get(grid, x+1, y-1) == 'M' &&
        grid_get(grid, x-1, y+1) == 'S'
    ) {
        return 1;
    }
    /**
     * S.S
     * .A.
     * M.M 
     */
    if (grid_get(grid, x, y) == 'A' &&
        grid_get(grid, x-1, y-1) == 'S' &&
        grid_get(grid, x+1, y+1) == 'M' &&
        grid_get(grid, x+1, y-1) == 'S' &&
        grid_get(grid, x-1, y+1) == 'M'
    ) {
        return 1;
    }

    /**
     * M.S
     * .A.
     * M.S 
     */
    if (grid_get(grid, x, y) == 'A' &&
        grid_get(grid, x-1, y-1) == 'M' &&
        grid_get(grid, x+1, y+1) == 'S' &&
        grid_get(grid, x+1, y-1) == 'S' &&
        grid_get(grid, x-1, y+1) == 'M'
    ) {
        return 1;
    }

    /**
     * S.M
     * .A.
     * S.M 
     */
    if (grid_get(grid, x, y) == 'A' &&
        grid_get(grid, x-1, y-1) == 'S' &&
        grid_get(grid, x+1, y+1) == 'M' &&
        grid_get(grid, x+1, y-1) == 'M' &&
        grid_get(grid, x-1, y+1) == 'S'
    ) {
        return 1;
    }

    return 0;
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
    for (int i = 0; i < (buffer_size + 1); i++) {
        if (source[i] != '\n' && source[i] != '\0') {
            grid_data[grid_index] = source[i];
            grid_index++;
        }
    }

    grid_s grid = {
        .columns = width,
        .rows = lines,
        .grid_data = grid_data
    };
    int count = 0;
    int count2 = 0;
    // count += grid_find_xmas(&grid, 3, 9);
    for (int y = 0; y < grid.rows; y++) {
        for (int x = 0; x < grid.columns; x++) {
            int curr = grid_find_xmas(&grid, x, y);
            count2 += grid_find_cross_mas(&grid, x, y);
            // printf("XMAS @ (%d, %d) = %d\n", x, y, curr);
            count += curr;
        }
    }

    printf("XMAS-es: %d\n", count);
    printf("X-MAS-es: %d\n", count2);
}