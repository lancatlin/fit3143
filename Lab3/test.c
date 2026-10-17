#include "charging_node.h"
#include <assert.h>
#include <stdio.h>

int test_create_charging_node() {
    ChargingNode node = create_charging_node(1, 3);

    assert(node.id == 1);
    assert(node.port_count == 3);
    for (int i = 0; i < node.port_count; i++) {
        assert(node.ports[i].id == i);
    }
    return 0;
}

int main() {
    int (*test_cases[])() = {test_create_charging_node};
    int len = 1;

    for (int i = 0; i < len; i++) {
        int result = test_cases[i]();
        if (result != 0) {
            printf("Test case %d failed with code %d\n", i, result);
        } else {
            printf("Test case %d succeed\n", i);
        }
    }
}
