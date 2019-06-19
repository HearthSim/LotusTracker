#ifndef METRICS_H
#define METRICS_H

#include "influxdb.hpp"

void influx_metric(influxdb_cpp::detail::tag_or_field_caller &);

#endif // METRICS_H
