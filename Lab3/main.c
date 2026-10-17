#include "charging_node.h"

int main() {
    ChargingNode node = create_charging_node(0, 9);
    run_node(&node, 5);
    return 0;
}
