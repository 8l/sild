#include <stdio.h>

struct Cell {
    int value;
    struct Cell * next;
};

void print_list(struct Cell car) {
    printf("%d ", car.value);
    if (car.next) {
        print_list(*car.next);
    }
}

int main() {
    struct Cell another_cell;
    another_cell.value = 2;
    another_cell.next = 0x0;

    struct Cell a_cell;
    a_cell.value = 1;
    a_cell.next = &another_cell;

    print_list(a_cell);
}
