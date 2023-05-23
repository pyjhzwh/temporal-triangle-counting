#ifndef UTIL_
#define UTIL_

#include "Graph.h"
#include <map>
#include <unordered_map>

class Util
{
    public:
        template<typename T1, typename T2, typename T3>
        static void unorderedmapMapAddCntAndCreatIfNotExist(
            std::unordered_map<T1,std::map<T2, T3>>& nested_map,
            T1 key0, T2 key1, T3 cnt)
        {

            if (nested_map.find(key0) == nested_map.end())
            {
                std::map<T2, T3> val0;
                nested_map[key0] = val0;
            }
            if (nested_map[key0].find(key1) == nested_map[key0].end())
            {
                nested_map[key0][key1] = cnt;
            }
            else
            {
                nested_map[key0][key1] += cnt;
            }
            return;
        }
        
        template<typename T1, typename T2>
        static void unorderedmapAddCntAndCreatIfNotExist(
            std::unordered_map<T1,T2>& single_map,
            T1 key, T2 val)
        {
            if (single_map.find(key) == single_map.end())
            {
                single_map[key] = val;
            }
            else
            {
                single_map[key] += val;
            }
            return;
        }
};


#endif // UTIL_
