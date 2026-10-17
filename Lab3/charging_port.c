#include "charging_port.h"

ChargingPort create_charging_port(int id) {
    ChargingPort port = {id, false};
    return port;
}
