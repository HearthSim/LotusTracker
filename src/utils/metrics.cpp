#include "../macros.h"
#include "influxdb.hpp"
#include "metrics.h"

void influx_metric(influxdb_cpp::detail::tag_or_field_caller &builder) {
#ifdef QT_DEBUG
    return;
#else
    builder
        .tag("version", LOTUS_TRACKER->applicationVersion().toStdString())
        .field("user", LOTUS_TRACKER->appSettings->getInstallationUuid().toStdString())
        .send_udp("metrics.hearthsim.net", 8094);
#endif
}
