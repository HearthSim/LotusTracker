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
        struct tag_caller;
        struct field_caller;
        struct ts_caller;
    }

    struct builder {
        detail::tag_caller& meas(const std::string& m) {
            lines_.imbue(std::locale("C"));
            lines_.clear();
            return _m(m);
        }
    protected:
        detail::tag_caller& _m(const std::string& m) {
            _escape(m, ", ");
            return reinterpret_cast<detail::tag_caller&>(*this);
        }
        detail::tag_caller& _t(const std::string& k, const std::string& v) {
            lines_ << ',';
            _escape(k, ",= ");
            lines_ << '=';
            _escape(v, ",= ");
            return reinterpret_cast<detail::tag_caller&>(*this);
        }
        detail::field_caller& _f_s(char delim, const std::string& k, const std::string& v) {
            lines_ << delim;
            _escape(k, ",= ");
            lines_ << "=\"";
            _escape(v, "\"");
            lines_ << '\"';
            return reinterpret_cast<detail::field_caller&>(*this);
        }
        detail::field_caller& _f_i(char delim, const std::string& k, long long v) {
            lines_ << delim;
            _escape(k, ",= ");
            lines_ << '=';
            lines_ << v << 'i';
            return reinterpret_cast<detail::field_caller&>(*this);
        }
        detail::field_caller& _f_f(char delim, const std::string& k, double v, int prec) {
            lines_ << delim;
            _escape(k, ",= ");
            lines_.precision(prec);
            lines_ << '=' << v;
            return reinterpret_cast<detail::field_caller&>(*this);
        }
        detail::field_caller& _f_b(char delim, const std::string& k, bool v) {
            lines_ << delim;
            _escape(k, ",= ");
            lines_ << '=' << (v ? 't' : 'f');
            return reinterpret_cast<detail::field_caller&>(*this);
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
        void _escape(const std::string& src, const char* escape_seq) {
            size_t pos = 0, start = 0;
            while((pos = src.find_first_of(escape_seq, start)) != std::string::npos) {
                lines_.write(src.c_str() + start, static_cast<int>(pos - start));
                lines_ << '\\' << src[pos];
                start = ++pos;
            }
            lines_.write(src.c_str() + start, static_cast<int>(src.length() - start));
        }

        std::stringstream lines_;
    };

    namespace detail {
        struct tag_caller : public builder {
            detail::tag_caller& tag(const std::string& k, const std::string& v)       { return _t(k, v); }
            detail::field_caller& field(const std::string& k, const std::string& v)   { return _f_s(' ', k, v); }
            detail::field_caller& field(const std::string& k, bool v)                 { return _f_b(' ', k, v); }
            detail::field_caller& field(const std::string& k, short v)                { return _f_i(' ', k, v); }
            detail::field_caller& field(const std::string& k, int v)                  { return _f_i(' ', k, v); }
            detail::field_caller& field(const std::string& k, long v)                 { return _f_i(' ', k, v); }
            detail::field_caller& field(const std::string& k, long long v)            { return _f_i(' ', k, v); }
            detail::field_caller& field(const std::string& k, double v, int prec = 2) { return _f_f(' ', k, v, prec); }
        private:
            detail::tag_caller& meas(const std::string& m);
        };
        struct ts_caller : public builder {
            detail::tag_caller& meas(const std::string& m)                            { lines_ << '\n'; return _m(m); }
            int send_udp(const std::string& host, int port)                           { return _send_udp(host, port); }
        };
        struct field_caller : public ts_caller {
            detail::field_caller& field(const std::string& k, const std::string& v)   { return _f_s(',', k, v); }
            detail::field_caller& field(const std::string& k, bool v)                 { return _f_b(',', k, v); }
            detail::field_caller& field(const std::string& k, short v)                { return _f_i(',', k, v); }
            detail::field_caller& field(const std::string& k, int v)                  { return _f_i(',', k, v); }
            detail::field_caller& field(const std::string& k, long v)                 { return _f_i(',', k, v); }
            detail::field_caller& field(const std::string& k, long long v)            { return _f_i(',', k, v); }
            detail::field_caller& field(const std::string& k, double v, int prec = 2) { return _f_f(',', k, v, prec); }
            detail::ts_caller& timestamp(unsigned long long ts)                       { return _ts(static_cast<long long>(ts)); }
        };
    }
}

#endif // INFLUX_DB_HPP
