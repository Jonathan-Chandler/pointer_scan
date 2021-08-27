#ifndef __SEARCH_NEARBY_HPP__
#define __SEARCH_NEARBY_HPP__
#include <windows.h>
#include <vector>
#include "process_mem.hpp"

class Search_Nearby
{
  private:
    struct Addr_Dist
    {
        int32_t addr_diff;
        uint32_t avg_addr;
        Addr_Dist(int32_t diff, uint32_t avg):addr_diff(diff),avg_addr(avg){};
        bool operator <(const Addr_Dist& rhs) const {return addr_diff < rhs.addr_diff;};
    };

  private:
    Process_Mem *m_proc_mem;
    std::vector<uint32_t> m_search_values;
    //std::vector<std::vector<uint32_t>> m_search_results_vec;
    std::vector<std::vector<Addr_Dist*>> m_converted_search;
    std::vector<std::vector<Addr_Dist*>> m_results;
    std::vector<Search_Nearby::Addr_Dist*> get_least_distance(std::vector<Search_Nearby::Addr_Dist*> list_1, std::vector<Search_Nearby::Addr_Dist*> list_2);

  public:
    Search_Nearby(Process_Mem *mem, std::vector<uint32_t> searched_values);
    ~Search_Nearby();

    int start_search();
    void find_closest();

    void print_results();
    void print_search_results();
    void print_least_distance();

    std::vector<Addr_Dist*> convert_to_addr_dist(std::vector<uint32_t> addrs);

};

#endif // __SEARCH_NEARBY_HPP__
