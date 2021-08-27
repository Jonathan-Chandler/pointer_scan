#include <stdexcept>
#include <iostream>
#include <string>
#include <algorithm>    // std::sort

// #include "debug.h"
#include "utils.hpp"
#include "process_mem.hpp"
#include "process_info.hpp"
#include "search_nearby.hpp"

Search_Nearby::Search_Nearby(Process_Mem *mem, std::vector<uint32_t> searched_values)
  : m_proc_mem(mem),
    m_search_values(searched_values)
{
}

Search_Nearby::~Search_Nearby()
{
}

int Search_Nearby::start_search()
{
  for (auto it = m_search_values.begin(); it < m_search_values.end(); it++)
  {
    std::vector<uint32_t> current_results = m_proc_mem->search_pages(4, (uint8_t*)(&(*it)));

    // convert from vector of uint32_t search results to vector of Addr_Dist structs
    std::vector<Addr_Dist*> temp_vector;
    for (auto result_it = current_results.begin(); result_it < current_results.end(); result_it++)
    {
      Addr_Dist *converted_addr = new Addr_Dist(-1,*result_it);

      temp_vector.push_back(converted_addr);
    }

    m_converted_search.push_back(temp_vector);
  }

  return 0;
}

std::vector<Search_Nearby::Addr_Dist*> Search_Nearby::get_least_distance(std::vector<Search_Nearby::Addr_Dist*> list_1, std::vector<Search_Nearby::Addr_Dist*> list_2)
{
  std::vector<Search_Nearby::Addr_Dist*> *list_A;
  std::vector<Search_Nearby::Addr_Dist*> *list_B;
  std::vector<Search_Nearby::Addr_Dist*> result_list;

  // list_A = longest list
  if (list_1.size() > list_2.size())
  {
    list_A = &list_1;
    list_B = &list_2;
  }
  else
  {
    list_A = &list_2;
    list_B = &list_1;
  }

  //min_dist_t *result_list = malloc(sizeof(min_dist_t * len(list_A))
  //    min_list.avg_addr 

  for (auto x = list_A->begin(); x < list_A->end(); x++)
  {
    int32_t least_addr_diff = -1;
    uint32_t best_avg_addr;

    int32_t cur_addr_diff;

    for (auto y = list_B->begin(); y < list_B->end(); y++)
    {
      if ( (cur_addr_diff = (*x)->avg_addr - (*y)->avg_addr) < 0)
        cur_addr_diff *= -1;

      if (cur_addr_diff < least_addr_diff || least_addr_diff < 0)
      {
        least_addr_diff = cur_addr_diff;
        best_avg_addr = (*x)->avg_addr/2 + (*y)->avg_addr/2;
      }
      else
      {
        break;
      }
    }

    Search_Nearby::Addr_Dist *temp = new Search_Nearby::Addr_Dist(least_addr_diff, best_avg_addr);
    result_list.push_back(temp);
  }

  return result_list;
}
//      if (temp_dist < min_list[x].addr_dist || least_distance[x].addr_dist < 0)
//        min_list[x].addr_dist = temp_dist
//        least_distance[x].avg_addr = list_A[x]/2 + list_B[y]/2;
//      else
//        // skip remaining because already found minimum
//        break;


void Search_Nearby::print_search_results()
{
  std::cout << "Matches:" << std::endl;

  for (size_t x = 0; x < m_converted_search.size(); x++)
  {
    std::cout << "Results for 0x" << CONFIGURE_HEX << m_search_values[x] << CONFIGURE_DEC << std::endl;

    for (auto it = m_converted_search[x].begin(); it < m_converted_search[x].end(); it++)
    {
      std::cout << "0x" << CONFIGURE_HEX << (*it)->avg_addr << CONFIGURE_DEC << std::endl;
    }
    std::cout << std::endl;
  }
}

void Search_Nearby::print_results()
{
}

void Search_Nearby::print_least_distance()
{
  std::vector<Search_Nearby::Addr_Dist*> least = get_least_distance(m_converted_search[0], m_converted_search[1]);

  for (auto it = least.begin(); it < least.end(); it++)
  {
    std::cout << "avg 0x" << CONFIGURE_HEX << (*it)->avg_addr << CONFIGURE_DEC << std::endl;
    std::cout << "dist 0x" << CONFIGURE_HEX << (*it)->addr_diff << CONFIGURE_DEC << std::endl;
  }

  std::cout << std::endl << std::endl << "sorted" << std::endl;
  std::sort(least.begin(), least.end(), [](Search_Nearby::Addr_Dist* lhs, Search_Nearby::Addr_Dist* rhs){return *lhs < *rhs;});
  for (auto it = least.begin(); it < least.end(); it++)
  {
    std::cout << "avg 0x" << CONFIGURE_HEX << (*it)->avg_addr << CONFIGURE_DEC << std::endl;
    std::cout << "dist 0x" << CONFIGURE_HEX << (*it)->addr_diff << CONFIGURE_DEC << std::endl;
  }
}

#if 0
void Search_Nearby::find_closest()
{
  for (auto it = m_search_results_vec.begin(); it < m_search_results_vec.end(); it++)
  {
    for (auto it_2 = (*it).begin(); it_2 < (*it).end(); it_2++)
    {
      std::vector<uint32_t> m_search_results_vec;
    }
  }
}
#endif

// A/2 + B/2 = (A+B)/2

#if 0
struct Addr_Dist
{
    uint32_t m_addr_dist;
    uint32_t m_avg_addr;

    Addr_Dist(uint32_t addr_dist, uint32_t avg_addr) : m_addr_dist(addr_dist), m_avg_addr(avg_addr) {}

    bool operator < (const Addr_Dist& rhs) const
    {
        return (addr_dist < rhs.addr_dist);
    }
};

sort( values.begin( ), values.end( ), [ ]( const MyStruct& lhs, const MyStruct& rhs )
{
   return lhs.key < rhs.key;
});

find least distance between A[x] and B[y]
  input_list_1, input_list_2
  if (len(input_list_1) > len(input_list_2))
    list_A = input_list_1
    list_B = input_list_2
  else
    list_A = input_list_2
    list_B = input_list_1

  min_dist_t *min_list = malloc(sizeof(min_dist_t * len(list_A))
      min_list.avg_addr 

  for (x = 0 ... len(A))
    min_list[x].addr_dist = -1;      // 
    for (y = 0 ... len(B))
      if ( (temp_dist = A[x].avg_addr - B[y].avg_addr) < 0)
        temp_dist *= -1;

      if (temp_dist < min_list[x].addr_dist || least_distance[x].addr_dist < 0)
        min_list[x].addr_dist = temp_dist
        least_distance[x].avg_addr = list_A[x]/2 + list_B[y]/2;
      else
        // skip remaining because already found minimum
        break;


find least distance between A[x] and B[y]
  input_list_1, input_list_2
  if (len(input_list_1) > len(input_list_2))
    list_A = input_list_1
    list_B = input_list_2
  else
    list_A = input_list_2
    list_B = input_list_1

  min_dist_t *min_list = malloc(sizeof(min_dist_t * len(list_A))
      min_list.avg_addr 

  for (x = 0 ... len(A))
    min_list[x].addr_dist = -1;      // 
    for (y = 0 ... len(B))
      if ( (temp_dist = A[x] - B[y]) < 0)
        temp_dist *= -1;

      if (temp_dist < addr_dist || least_distance[x].addr_dist < 0)
        least_distance[x].addr_dist = temp_dist
        least_distance[x].avg_addr = list_A[x]/2 + list_B[y]/2;
      else
        // skip remaining because already found minimum
        break;
  


results list A , results list B

  A[0] - B[0]
  
  for (x = 0 .. len(A))
    int cur_min = A[x] - B[0]
    if cur_min < 0
      cur_min *= -1;

    cur_min_x = x;
    cur_min_y = 0;

    for (y = 0 .. len(B)
        temp = A[x] - B[y]
        if temp < 0
          temp *= -1;

        if (temp < cur_min)
          cur_min = temp;
          cur_min_x = x;
          cur_min_y = y;

        if temp > 
        if (A[x] - B[y]) increases


    
  value of results_list[


  

#endif 
