//
// Created by Jonathan Passerat-Palmbach on 30/08/2020.
//

#include "generic_profiling.hpp"

namespace libsnark {
    namespace profiling {

        // FIXME might have to take cumulative_times as input param if moving outside this file
        time_entry fetch_cumulative_time_entry(const std::string &key)
        {
          const double total_ms = (libff::cumulative_times.at(key) * 1e-6);
          const size_t cnt = libff::invocation_counts.at(key);
          const double avg_ms = total_ms / cnt;

          return std::make_tuple(total_ms, cnt, avg_ms);
        }

        std::string format_as_csv(const time_entry& te, char delimiter)
        {
          std::stringstream ss;

          ss << std::get<0>(te) << delimiter <<
             std::get<1>(te) << delimiter <<
             std::get<2>(te) << delimiter;

          return ss.str();
        }

        std::string cumulative_times_as_csv(char in_delimiter, const std::string& header) {

          std::stringstream all_times;

          if (!header.empty()) all_times << header << std::endl;

          for (const auto& kv : libff::cumulative_times)
          {
            const auto& key = kv.first;
            const auto& te = fetch_cumulative_time_entry(key);
            all_times << key << in_delimiter << format_as_csv(te, in_delimiter) << std::endl;
          }

          return all_times.str();
        }



        void turn_on_profiling() {

          libff::clear_profiling_counters();
          libff::start_profiling();

          // make sure profiling is enabled => ZoKrates forces it off otherwise!!!
          libff::inhibit_profiling_info = false;
          libff::inhibit_profiling_counters = false;
        }

    } // namespace profiling
} // namespace libsnark
