//
// Created by Jonathan Passerat-Palmbach on 24/08/2020.
//

#ifndef LIBSNARK_PROFILING_H
#define LIBSNARK_PROFILING_H


#include <libff/common/profiling.hpp>
#include <iostream>
#include <sstream>
#include <tuple>

namespace libsnark {

    namespace profiling {

      typedef std::tuple<double, size_t, double> time_entry;
      time_entry fetch_cumulative_time_entry(const std::string &key);
      std::string format_as_csv(const time_entry& te, char delimiter=',');
      std::string cumulative_times_as_csv(char in_delimiter=',', const std::string& header = "");

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

      template<typename Fn, Fn f, typename... Args>
      typename std::result_of<Fn(Args...)>::type
      profiler_wrapper(Args... args) {

        libff::enter_block("Call to r1cs_XX_ppzksnark_prover");
        auto proof = f(std::forward<Args>(args)...);
        libff::leave_block("Call to r1cs_XX_ppzksnark_prover");

  //  libff::print_cumulative_times();
        printf("\n"); libff::print_indent(); libff::print_mem("memory after prover");

        std::cout << "*** As CSV ***" << std::endl <<
                  cumulative_times_as_csv(';', "key;total_ms;cnt;avg_ms") << std::endl <<
                  "*** End CSV ***" << std::endl;

        return proof;
      }

    } // namespace profiling

} // namespace libsnark


#define PROFILEIT(FUNC) libsnark::profiling::profiler_wrapper<decltype(&FUNC), &FUNC>

#endif //LIBSNARK_PROFILING_H
