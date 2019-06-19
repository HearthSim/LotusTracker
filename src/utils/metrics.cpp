#include "../macros.h"
#include "influxdb.hpp"
#include "metrics.h"

void influx_metric(influxdb_cpp::detail::field_caller &builder) {
    builder
        .field("user", LOTUS_TRACKER->appSettings->getInstallationUuid().toStdString())
        .send_udp("metrics.hearthsim.net", 8094);
}
