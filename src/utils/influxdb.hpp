/*
  influxdb-cpp -- 💜 C++ client for InfluxDB.

  Copyright (c) 2010-2018 <http://ez8.co> <orca.zhang@yahoo.com>
  This library is released under the MIT License.

  Please see LICENSE file or visit https://github.com/orca-zhang/influxdb-cpp for details.

  Modified by HearthSim, LLC for MinGW compatibility and size.
 */
#ifndef INFLUX_DB_HPP
#define INFLUX_DB_HPP

#include <sstream>
#include <cstring>
#include <cstdio>
#include <memory>
#include <QHostInfo>
#include <QString>
#include <QUdpSocket>

namespace influxdb_cpp {
    namespace detail {
        struct meas_caller;
        struct tag_or_field_caller;
        struct ts_caller;
    }

    struct builder {
        detail::tag_or_field_caller& meas(const std::string& m) {
            lines_.imbue(std::locale("C"));
            lines_.clear();
            return _m(m);
        }
    protected:
        detail::tag_or_field_caller& _m(const std::string& m) {
            curr_start_ = lines_.str().length();
            has_fields_ = false;
            has_tags_ = false;

            lines_ << _escape(m, ", ");
            return reinterpret_cast<detail::tag_or_field_caller&>(*this);
        }
        detail::tag_or_field_caller& _t(const std::string& k, const std::string& v) {
            std::string kv = "," + _escape(k, ",= ") + "=" + _escape(v, ",= ");

            if (has_fields_) {
                std::string data = lines_.str();
                data.insert(has_tags_ ? data.find(',', curr_start_) : data.find(' ', curr_start_), kv);

                lines_.str("");
                lines_.clear();
                lines_ << data;
            } else {
                lines_ << kv;
            }

            has_tags_ = true;
            return reinterpret_cast<detail::tag_or_field_caller&>(*this);
        }
        detail::tag_or_field_caller& _f_s(const std::string& k, const std::string& v) {
            lines_ << (has_fields_ ? ',' : ' ');
            lines_ << _escape(k, ",= ");
            lines_ << "=\"";
            lines_ << _escape(v, "\"");
            lines_ << '\"';

            has_fields_ = true;

            return reinterpret_cast<detail::tag_or_field_caller&>(*this);
        }
        detail::tag_or_field_caller& _f_i(const std::string& k, long long v) {
            lines_ << (has_fields_ ? ',' : ' ');
            lines_ << _escape(k, ",= ");
            lines_ << '=';
            lines_ << v << 'i';

            has_fields_ = true;

            return reinterpret_cast<detail::tag_or_field_caller&>(*this);
        }
        detail::tag_or_field_caller& _f_f(const std::string& k, double v, int prec) {
            lines_ << (has_fields_ ? ',' : ' ');
            lines_ << _escape(k, ",= ");
            lines_.precision(prec);
            lines_ << '=' << v;

            has_fields_ = true;

            return reinterpret_cast<detail::tag_or_field_caller&>(*this);
        }
        detail::tag_or_field_caller& _f_b(const std::string& k, bool v) {
            lines_ << (has_fields_ ? ',' : ' ');
            lines_ << _escape(k, ",= ");
            lines_ << '=' << (v ? 't' : 'f');

            has_fields_ = true;

            return reinterpret_cast<detail::tag_or_field_caller&>(*this);
        }
        detail::ts_caller& _ts(long long ts) {
            lines_ << ' ' << ts;

            return reinterpret_cast<detail::ts_caller&>(*this);
        }
        int _send_udp(const std::string& host, int port) {
            QHostInfo info = QHostInfo::fromName(QString(host.c_str()));

            if (info.error() == QHostInfo::NoError) {
                QHostAddress address = info.addresses().first();
                QUdpSocket *sock = new QUdpSocket();

                lines_ << '\n';

                if (sock->writeDatagram(&lines_.str()[0], lines_.str().length(), address, static_cast<quint16>(port)) < lines_.str().length()) {
                    return -3;
                }
            } else {
                return -1;
            }

            return 0;
        }
        std::string _escape(const std::string& src, const char* escape_seq) {
            size_t pos = 0, start = 0;
            std::stringstream ret;
            while((pos = src.find_first_of(escape_seq, start)) != std::string::npos) {
                ret.write(src.c_str() + start, static_cast<int>(pos - start));
                ret << '\\' << src[pos];
                start = ++pos;
            }
            ret.write(src.c_str() + start, static_cast<int>(src.length() - start));
            return ret.str();
        }

        std::stringstream lines_;

        unsigned int curr_start_;
        bool has_fields_;
        bool has_tags_;
    };

    namespace detail {
        struct ts_caller : public builder {
            detail::tag_or_field_caller& meas(const std::string& m)                   { lines_ << '\n'; return _m(m); }
            int send_udp(const std::string& host, int port)                           { return _send_udp(host, port); }
        };
        struct tag_or_field_caller : public ts_caller {
            detail::tag_or_field_caller& tag(const std::string& k, const std::string& v)     { return _t(k, v); }
            detail::tag_or_field_caller& field(const std::string& k, const std::string& v)   { return _f_s(k, v); }
            detail::tag_or_field_caller& field(const std::string& k, bool v)                 { return _f_b(k, v); }
            detail::tag_or_field_caller& field(const std::string& k, short v)                { return _f_i(k, v); }
            detail::tag_or_field_caller& field(const std::string& k, int v)                  { return _f_i(k, v); }
            detail::tag_or_field_caller& field(const std::string& k, long v)                 { return _f_i(k, v); }
            detail::tag_or_field_caller& field(const std::string& k, long long v)            { return _f_i(k, v); }
            detail::tag_or_field_caller& field(const std::string& k, double v, int prec = 2) { return _f_f(k, v, prec); }
            detail::ts_caller& timestamp(unsigned long long ts)                              { return _ts(static_cast<long long>(ts)); }
        private:
            detail::tag_or_field_caller& meas(const std::string& m);
        };
    }
}

#endif // INFLUX_DB_HPP
