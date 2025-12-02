#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdarg.h>
#include <assert.h>

#include <alloc/arena.h>
#include "csv/csv.h"


#define CSV_FILE "assets/Iris.csv"
#define VIRTUAL_HEAP_SIZE 1024*40


size_t file_size(FILE * f) {
    fseek(f, 0L, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0L, SEEK_SET);
    return sz;
}


int main(void) {
    ArenaAlloc arena = arena_alloc(VIRTUAL_HEAP_SIZE);
    
    FILE * f = fopen(CSV_FILE, "r");
    assert(f != NULL);

    size_t size = file_size(f);

    char * csv_text = new(ALLOC(&arena), sizeof(char) * (size + 1));
    fread(csv_text, sizeof(char), size, f);

    Csv doc = csv(ALLOC(&arena), ',', csv_text);
    Csv data = csv_slice(&doc, (size_t[]) {1, 0, doc.row-1, doc.column});
    //Csv data = csv_transpose(&doc);
    printf("%ldx%ld %ld | %ld\n", data.row, data.column, data.size, doc.size);  

    for(size_t row = 0; row < data.row; row++) {
        for(size_t column = 0; column < data.column; column++) {
            Csv_Cell *cell = csv_at(&data, row, column);
            assert(cell != NULL);

            printf(column == 0 ? "%.*s" : ", %.*s", (int) cell->length, cell->c_str);
        }

        printf("\n");
    }
    

    printf("allocated: %ldB = %ldKb\n", arena.size, arena.size / 1024);

    csv_finalize(&doc);
    finalize(ALLOC(&arena));
    printf("Program exit..\n");
    return EXIT_SUCCESS;
}


