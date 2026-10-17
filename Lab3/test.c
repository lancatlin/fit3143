#include "charging_node.h"
#include "charging_port.h"
#include "random_generator.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

int test_create_charging_node() {
    ChargingNode node = create_charging_node(1, 3);

    assert(node.id == 1);
    assert(node.port_count == 3);
    for (int i = 0; i < node.port_count; i++) {
        assert(node.ports[i].id == i);
        assert(!node.ports[i].occupied);
    }
    return 0;
}

int test_generate_random_number() {
    RandomGenerator generator = create_random_generator(1, 0, 10, 50);
    for (int i = 0; i < 20; i++) {
        int value = next_random_value(&generator);
        assert(value >= generator.min && value <= generator.max);
        // printf("Random %d: %d\n", i, value);
    }
    return 0;
}

int test_start_port() {
    ChargingPort port = create_charging_port(0);
    RandomGenerator generator = create_random_generator(1, 0, 1, 3);
    start_port(&port, &generator);
    assert(port.running);

    sleep(10);

    stop_port(&port);
    assert(!port.running);
    return 0;
}

int main() {
    int (*test_cases[])() = {test_create_charging_node,
                             test_generate_random_number, test_start_port};
    int len = 3;

    for (int i = 0; i < len; i++) {
        int result = test_cases[i]();
        if (result != 0) {
            printf("Test case %d failed with code %d\n", i, result);
        } else {
            printf("Test case %d succeed\n", i);
        }
    }
}
