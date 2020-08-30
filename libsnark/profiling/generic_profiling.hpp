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
      void turn_on_profiling();

      template<typename Fn, Fn f, typename... Args>
      typename std::result_of<Fn(Args...)>::type
      profiler_wrapper(Args... args) {

        // this might wipe out previous metrics so make sure to collect in between...
        turn_on_profiling();

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
