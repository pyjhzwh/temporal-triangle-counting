#include "ETTC/t_triangle_counting.h"

#include <algorithm>
#include <unordered_set>
#include "timer.h"
#include "util.h"

using namespace std;

//////////////////////////////////////////////////
//////////////////////////////////////////////////
// indexing all 64 types of edge counts we need based on the directed static edge we are counting for (eg. uv), directed static edge
// whose compatible corresponding temporal edges are being counted (eg. vw), and the time interval (eg. [t-delta, t])
//////////////////////////////////////////////////
//////////////////////////////////////////////////


//     edge_count_index_map["uv on vw: -inf, t"] = 0 * highest_mult; edge_count_index_map["uv on vw: t-delta, t"] = 1 * highest_mult; edge_count_index_map["uv on vw: t, t+delta"] = 2 * highest_mult; edge_count_index_map["uv on vw: t, inf"] = 3 * highest_mult;
//     edge_count_index_map["uv on wv: -inf, t"] = 4 * highest_mult; edge_count_index_map["uv on wv: t-delta, t"] = 5 * highest_mult; edge_count_index_map["uv on wv: t, t+delta"] = 6 * highest_mult; edge_count_index_map["uv on wv: t, inf"] = 7 * highest_mult;
//     edge_count_index_map["vu on vw: -inf, t"] = 8 * highest_mult; edge_count_index_map["vu on vw: t-delta, t"] = 9 * highest_mult; edge_count_index_map["vu on vw: t, t+delta"] = 10 * highest_mult; edge_count_index_map["vu on vw: t, inf"] = 11 * highest_mult;
//     edge_count_index_map["vu on wv: -inf, t"] = 12 * highest_mult; edge_count_index_map["vu on wv: t-delta, t"] = 13 * highest_mult; edge_count_index_map["vu on wv: t, t+delta"] = 14 * highest_mult; edge_count_index_map["vu on wv: t, inf"] = 15 * highest_mult;
//     edge_count_index_map["uw on vw: -inf, t"] = 16 * highest_mult; edge_count_index_map["uw on vw: t-delta, t"] = 17 * highest_mult; edge_count_index_map["uw on vw: t, t+delta"] = 18 * highest_mult; edge_count_index_map["uw on vw: t, inf"] = 19 * highest_mult;
//     edge_count_index_map["uw on wv: -inf, t"] = 20 * highest_mult; edge_count_index_map["uw on wv: t-delta, t"] = 21 * highest_mult; edge_count_index_map["uw on wv: t, t+delta"] = 22 * highest_mult; edge_count_index_map["uw on wv: t, inf"] = 23 * highest_mult;
//     edge_count_index_map["wu on vw: -inf, t"] = 24 * highest_mult; edge_count_index_map["wu on vw: t-delta, t"] = 25 * highest_mult; edge_count_index_map["wu on vw: t, t+delta"] = 26 * highest_mult; edge_count_index_map["wu on vw: t, inf"] = 27 * highest_mult;
//     edge_count_index_map["wu on wv: -inf, t"] = 28 * highest_mult; edge_count_index_map["wu on wv: t-delta, t"] = 29 * highest_mult; edge_count_index_map["wu on wv: t, t+delta"] = 30 * highest_mult; edge_count_index_map["wu on wv: t, inf"] = 31 * highest_mult;
    

//     edge_count_index_map["uv on vw: t-delta1, t"] = 32;
//     edge_count_index_map["uv on wv: t-delta1, t"] = 33;
//     edge_count_index_map["vu on vw: t-delta1, t"] = 34;
//     edge_count_index_map["vu on wv: t-delta1, t"] = 35;
//     edge_count_index_map["uw on vw: t-delta1, t"] = 36;
//     edge_count_index_map["uw on wv: t-delta1, t"] = 37;
//     edge_count_index_map["wu on vw: t-delta1, t"] = 38;
//     edge_count_index_map["wu on wv: t-delta1, t"] = 39;

//     edge_count_index_map["uv on vw: t, t+delta1"] = 40;
//     edge_count_index_map["uv on wv: t, t+delta1"] = 41;
//     edge_count_index_map["vu on vw: t, t+delta1"] = 42;
//     edge_count_index_map["vu on wv: t, t+delta1"] = 43;
//     edge_count_index_map["uw on vw: t, t+delta1"] = 44;
//     edge_count_index_map["uw on wv: t, t+delta1"] = 45;
//     edge_count_index_map["wu on vw: t, t+delta1"] = 46;
//     edge_count_index_map["wu on wv: t, t+delta1"] = 47;


//     edge_count_index_map["uv on vw: t-delta2, t"] = 48;
//     edge_count_index_map["uv on wv: t-delta2, t"] = 49;
//     edge_count_index_map["vu on vw: t-delta2, t"] = 50;
//     edge_count_index_map["vu on wv: t-delta2, t"] = 51;
//     edge_count_index_map["uw on vw: t-delta2, t"] = 52;
//     edge_count_index_map["uw on wv: t-delta2, t"] = 53;
//     edge_count_index_map["wu on vw: t-delta2, t"] = 54;
//     edge_count_index_map["wu on wv: t-delta2, t"] = 55;

//     edge_count_index_map["uv on vw: t, t+delta2"] = 56;
//     edge_count_index_map["uv on wv: t, t+delta2"] = 57;
//     edge_count_index_map["vu on vw: t, t+delta2"] = 58;
//     edge_count_index_map["vu on wv: t, t+delta2"] = 59;
//     edge_count_index_map["uw on vw: t, t+delta2"] = 60;
//     edge_count_index_map["uw on wv: t, t+delta2"] = 61;
//     edge_count_index_map["wu on vw: t, t+delta2"] = 62;
//     edge_count_index_map["wu on wv: t, t+delta2"] = 63;


void MotifCounter::populateEdgeCount(CSRTemporalGraph &t_graph,
                          VertexEdgeId search_for_start_pos,
                          VertexEdgeId search_for_end_pos,
                          VertexEdgeId search_from_start_pos,
                          VertexEdgeId search_from_end_pos,
                          VertexEdgeId minus_inf,
                          VertexEdgeId minus_delta,
                          VertexEdgeId plus_delta,
                          VertexEdgeId plus_inf,
                          VertexEdgeId minus_delta1,
                          VertexEdgeId plus_delta1,
                          VertexEdgeId minus_delta2,
                          VertexEdgeId plus_delta2)
{


    for (VertexEdgeId t_pos = search_for_start_pos; t_pos < search_for_end_pos; t_pos++)
    {   
        edge_count[minus_inf + t_pos - search_for_start_pos] = 
        edge_count_cum[minus_inf + t_pos - search_for_start_pos] = 
            t_graph.edgeTimeMaxLimitCount(search_from_start_pos, search_from_end_pos, t_graph.times_[t_pos]);

        edge_count[minus_delta + t_pos - search_for_start_pos] = 
        edge_count_cum[minus_delta + t_pos - search_for_start_pos] = 
            t_graph.edgeTimeIntervalCount(search_from_start_pos, search_from_end_pos, t_graph.times_[t_pos] - delta_, t_graph.times_[t_pos]);

        edge_count[plus_delta + t_pos - search_for_start_pos] =
        edge_count_cum[plus_delta + t_pos - search_for_start_pos] =
            t_graph.edgeTimeIntervalCount(search_from_start_pos, search_from_end_pos, t_graph.times_[t_pos], t_graph.times_[t_pos] + delta_);

        edge_count[plus_inf + t_pos - search_for_start_pos] = 
        edge_count_cum[plus_inf + t_pos - search_for_start_pos] = 
            t_graph.edgeTimeMinLimitCount(search_from_start_pos, search_from_end_pos, t_graph.times_[t_pos]);

        edge_count[minus_delta1 + t_pos - search_for_start_pos] = 
        edge_count_cum[minus_delta1 + t_pos - search_for_start_pos] = 
            t_graph.edgeTimeIntervalCount(search_from_start_pos, search_from_end_pos, t_graph.times_[t_pos] - delta1_, t_graph.times_[t_pos]);

        edge_count[plus_delta1 + t_pos - search_for_start_pos] = 
        edge_count_cum[plus_delta1 + t_pos - search_for_start_pos] = 
            t_graph.edgeTimeIntervalCount(search_from_start_pos, search_from_end_pos, t_graph.times_[t_pos], t_graph.times_[t_pos] + delta1_);

        edge_count[minus_delta2 + t_pos - search_for_start_pos] = 
        edge_count_cum[minus_delta2 + t_pos - search_for_start_pos] = 
            t_graph.edgeTimeIntervalCount(search_from_start_pos, search_from_end_pos, t_graph.times_[t_pos] - delta2_, t_graph.times_[t_pos]);

        edge_count[plus_delta2 + t_pos - search_for_start_pos] = 
        edge_count_cum[plus_delta2 + t_pos - search_for_start_pos] = 
            t_graph.edgeTimeIntervalCount(search_from_start_pos, search_from_end_pos, t_graph.times_[t_pos], t_graph.times_[t_pos] + delta2_);
    }

    // cumulative
    for (VertexEdgeId t_pos = search_for_start_pos + 1; t_pos < search_for_end_pos; t_pos++)
    {
        edge_count_cum[minus_inf + t_pos - search_for_start_pos] +=
             edge_count_cum[minus_inf + t_pos - search_for_start_pos - 1];

        edge_count_cum[minus_delta + t_pos - search_for_start_pos] += 
            edge_count_cum[minus_delta + t_pos - search_for_start_pos - 1];

        edge_count_cum[plus_delta + t_pos - search_for_start_pos] +=
            edge_count_cum[plus_delta + t_pos - search_for_start_pos - 1];

        edge_count_cum[plus_inf + t_pos - search_for_start_pos] += 
            edge_count_cum[plus_inf + t_pos - search_for_start_pos - 1];


        edge_count_cum[minus_delta1 + t_pos - search_for_start_pos] +=
            edge_count_cum[minus_delta1 + t_pos - search_for_start_pos - 1];

        edge_count_cum[plus_delta1 + t_pos - search_for_start_pos] +=
            edge_count_cum[plus_delta1 + t_pos - search_for_start_pos - 1];

        edge_count_cum[minus_delta2 + t_pos - search_for_start_pos] +=
            edge_count_cum[minus_delta2 + t_pos - search_for_start_pos - 1];

        edge_count_cum[plus_delta2 + t_pos - search_for_start_pos] +=
            edge_count_cum[plus_delta2 + t_pos - search_for_start_pos - 1];

    }

}


// \pi_1 and \pi_2 as explained in the paper
Count MotifCounter::countCaseA(CSRTemporalGraph &t_graph,
                            VertexEdgeId t1_start_pos, VertexEdgeId t1_end_pos,
                            VertexEdgeId t2_start_pos, VertexEdgeId t2_end_pos,
                            VertexEdgeId t3_start_pos, VertexEdgeId t3_end_pos,
                            VertexEdgeId t1_on_t3_plus_delta,
                            VertexEdgeId t1_on_t3_plus_inf,
                            VertexEdgeId t2_on_t3_plus_inf,
                            VertexEdgeId t2_on_t3_plus_delta2)
{
    
    Count sum = 0, sum_1 = 0, sum_2 = 0;
    // if any of e1, e2 and e3 multiplicity is zero, the count must be zero, could speedup ~18%
    if ((t1_end_pos - t1_start_pos == 0) || (t2_end_pos - t2_start_pos == 0 ) || (t3_end_pos - t3_start_pos == 0 ))
    {
        return 0;
    }

    for (VertexEdgeId t2_pos = t2_start_pos; t2_pos < t2_end_pos; t2_pos++)
    {
        // sum_2
        // t2_on_t1 = l_l - l_{delta_{2,3}} // # of edges in S1 whose timestamp is between t(S2[i]) - delta_{1,3} + delta_{2,3} and t(S2[i])
        VertexEdgeId t2_on_t1 = t_graph.edgeTimeIntervalCount(t1_start_pos, t1_end_pos, t_graph.times_[t2_pos] + delta2_ - delta_ + 1, t_graph.times_[t2_pos]);
        // edge_count[t2_on_t3_plus_delta2  + t2_pos - t2_start_pos] = EC([t(S2[i]), t(S2[i]+delta_{2,3})], S3)
        sum_2 += t2_on_t1 * edge_count[t2_on_t3_plus_delta2  + t2_pos - t2_start_pos];

        // sum_1
        // (start) l_f = LOWERBOUND(t(S2[i]) - delta_{1,2})
        // (finish) l_{delta_{2,3}} = UPPERBOND(t(S2[i]) - delta_{1,3} + delta_{2,3})
        VertexEdgeId start = t_graph.edgeTimeMinLimitSearch(t1_start_pos, t1_end_pos, t_graph.times_[t2_pos] - delta1_);
        VertexEdgeId finish = t_graph.edgeTimeMaxLimitSearch(t1_start_pos, t1_end_pos, t_graph.times_[t2_pos] + delta2_ - delta_);

        if(start == -1)
        {
            continue;
        }
        if(finish == -1)
        {
            continue;
        }
        start -= t1_start_pos;
        finish -= t1_start_pos;
 

        // cur_sum += CEC_{+delta_{1,3}}(S1[l_f: l_{delta_{2,3}}])
        Count cur_sum = edge_count_cum[t1_on_t3_plus_delta + finish - 1] -
                        edge_count_cum[t1_on_t3_plus_delta + start] +
                        edge_count[t1_on_t3_plus_delta + start];
        // cur_sum -= CEC_{inf}S1[l_f: l_{delta_{2,3}}])
        cur_sum -= edge_count_cum[t1_on_t3_plus_inf + finish - 1] -
            edge_count_cum[t1_on_t3_plus_inf + start] +
            edge_count[t1_on_t3_plus_inf + start];
        // cur_sum += (l_l - l_f + 1) * EC([t(S2[i]), inf], S3) // where is the +1 though??
        cur_sum += (finish - start) * edge_count[t2_on_t3_plus_inf + t2_pos - t2_start_pos];

        sum_1 += cur_sum;

    }


    sum = sum_1 + sum_2;
    return sum;

}

// \pi_3 and \pi_4 as explained in the paper
Count MotifCounter::countCaseB(CSRTemporalGraph &t_graph,
                            VertexEdgeId t1_start_pos, VertexEdgeId t1_end_pos,
                            VertexEdgeId t2_start_pos, VertexEdgeId t2_end_pos,
                            VertexEdgeId t3_start_pos, VertexEdgeId t3_end_pos,
                            VertexEdgeId t1_on_t2_plus_delta1,
                            VertexEdgeId t1_on_t2_minus_inf,
                            VertexEdgeId t3_on_t2_minus_delta2,
                            VertexEdgeId t3_on_t2_minus_inf)
{

    Count sum = 0, sum_1 = 0, sum_2 = 0, sum_3 = 0;
    // if any of e1, e2 and e3 multiplicity is zero, the count must be zero, could speedup ~18%
    if ((t1_end_pos - t1_start_pos == 0) || (t2_end_pos - t2_start_pos == 0 ) || (t3_end_pos - t3_start_pos == 0 ))
    {
        return 0;
    }

    for (VertexEdgeId t1_pos = t1_start_pos; t1_pos < t1_end_pos; t1_pos++)
    {
        //sum_1

        VertexEdgeId start = t_graph.edgeTimeMinLimitSearch(t3_start_pos, t3_end_pos, t_graph.times_[t1_pos]);
        VertexEdgeId finish = t_graph.edgeTimeMaxLimitSearch(t3_start_pos, t3_end_pos, t_graph.times_[t1_pos] + min(delta1_, delta2_));

        if(start == -1)
        {
            continue;
        }
        if(finish == -1)
        {
            continue;
        }
        start -= t3_start_pos;
        finish -= t3_start_pos;

        
        Count cur_sum = edge_count_cum[t3_on_t2_minus_inf + finish - 1] -
                        edge_count_cum[t3_on_t2_minus_inf + start] +
                        edge_count[t3_on_t2_minus_inf + start];

        cur_sum -= (finish - start) * edge_count[t1_on_t2_minus_inf + t1_pos - t1_start_pos];

        cur_sum += (finish - start) * t_graph.edgeTimeIntervalCount(t2_start_pos, t2_end_pos, t_graph.times_[t1_pos], t_graph.times_[t1_pos]);
        
        sum_1 += cur_sum;


        //sum_2

        start = finish;
        finish = t_graph.edgeTimeMinLimitSearch(t3_start_pos, t3_end_pos, t_graph.times_[t1_pos] + max(delta1_, delta2_));
        if(start == -1)
        {
            continue;
        }
        if(finish == -1)
        {
            continue;
        }

        finish -= t3_start_pos;

        if(delta1_ <= delta2_)
        {
            sum_2 += (finish - start) * edge_count[t1_on_t2_plus_delta1 + t1_pos - t1_start_pos];
        }
        else
        {
            sum_2 += edge_count_cum[t3_on_t2_minus_delta2 + finish - 1] -
                        edge_count_cum[t3_on_t2_minus_delta2 + start] +
                        edge_count[t3_on_t2_minus_delta2 + start];
        }


        start = finish;
        finish = t_graph.edgeTimeMaxLimitSearch(t3_start_pos, t3_end_pos, t_graph.times_[t1_pos] + delta_);
 
        if(start == -1)
        {
            continue;
        }
        if(finish == -1)
        {
            continue;
        }

        finish -= t3_start_pos;

        cur_sum = edge_count_cum[t3_on_t2_minus_delta2 + finish - 1] -
                        edge_count_cum[t3_on_t2_minus_delta2 + start] +
                        edge_count[t3_on_t2_minus_delta2 + start];

        cur_sum -= edge_count_cum[t3_on_t2_minus_inf + finish - 1] -
                        edge_count_cum[t3_on_t2_minus_inf + start] +
                        edge_count[t3_on_t2_minus_inf + start];

        cur_sum += (finish - start) * t_graph.edgeTimeMaxLimitCount(t2_start_pos, t2_end_pos, t_graph.times_[t1_pos] + delta1_);
        sum_3 += cur_sum;
    }

    sum= sum_1 + sum_2 + sum_3;
    return sum;

}

// \pi_5 and \pi_6 as explained in the paper
Count MotifCounter::countCaseC(CSRTemporalGraph &t_graph,
                            VertexEdgeId t1_start_pos, VertexEdgeId t1_end_pos,
                            VertexEdgeId t2_start_pos, VertexEdgeId t2_end_pos,
                            VertexEdgeId t3_start_pos, VertexEdgeId t3_end_pos,
                            VertexEdgeId t3_on_t1_minus_delta,
                            VertexEdgeId t3_on_t1_minus_inf,
                            VertexEdgeId t2_on_t1_minus_inf,
                            VertexEdgeId t2_on_t1_minus_delta1)
{
    
    Count sum = 0, sum_1 = 0, sum_2 = 0;

    // if any of e1, e2 and e3 multiplicity is zero, the count must be zero, could speedup ~18%
    if ((t1_end_pos - t1_start_pos == 0) || (t2_end_pos - t2_start_pos == 0 ) || (t3_end_pos - t3_start_pos == 0 ))
    {
        return 0;
    }

    for (VertexEdgeId t2_pos = t2_start_pos; t2_pos < t2_end_pos; t2_pos++)
    {

        // sum_2
        VertexEdgeId t2_on_t3 = t_graph.edgeTimeIntervalCount(t3_start_pos, t3_end_pos, t_graph.times_[t2_pos], t_graph.times_[t2_pos] + delta_ - delta1_-1);
        sum_2 += t2_on_t3 * edge_count[t2_on_t1_minus_delta1  + t2_pos - t2_start_pos];


        //sum_1
        VertexEdgeId start = t_graph.edgeTimeMinLimitSearch(t3_start_pos, t3_end_pos, t_graph.times_[t2_pos] - delta1_ + delta_);
        VertexEdgeId finish = t_graph.edgeTimeMaxLimitSearch(t3_start_pos, t3_end_pos, t_graph.times_[t2_pos] + delta2_);
 
        if(start == -1)
        {
            continue;
        }
        if(finish == -1)
        {
            continue;
        }
        start -= t3_start_pos;
        finish -= t3_start_pos;

        Count cur_sum = edge_count_cum[t3_on_t1_minus_delta + finish - 1] -
                        edge_count_cum[t3_on_t1_minus_delta + start] +
                        edge_count[t3_on_t1_minus_delta + start];
        
        cur_sum -= edge_count_cum[t3_on_t1_minus_inf + finish - 1] -
            edge_count_cum[t3_on_t1_minus_inf + start] +
            edge_count[t3_on_t1_minus_inf + start];
        
        cur_sum += (finish - start) * edge_count[t2_on_t1_minus_inf + t2_pos - t2_start_pos];
        
        sum_1 += cur_sum;

    }

    sum = sum_1 + sum_2;
    return sum;

}


void MotifCounter::countTemporalTriangle(CSRGraph &s_dag, CSRTemporalGraph &t_graph, TemporalTime delta, TemporalTime delta1, TemporalTime delta2)
{
    CSRGraph mult_graph = t_graph.extractMultGraph(); // mult_graph is the temporal graph without temporal mutiplicity
    VertexEdgeId highest_mult = 0;
    for (VertexEdgeId i = 0; i < mult_graph.num_edges_; i++)
        highest_mult = max(highest_mult, mult_graph.temporal_start_pos_[i+1] - mult_graph.temporal_start_pos_[i]);
    highest_mult_ = highest_mult;


    // reload edge times from temporal_Nbrs_ to times_
    t_graph.reloadTimes();


    edge_count = new VertexEdgeId[64 * highest_mult];
    edge_count_cum = new VertexEdgeId[64 * highest_mult];
    delta_ = delta;
    delta1_ = delta1;
    delta2_ = delta2;

    // we have 64 different types of edge counts as shown at the top of page
    VertexEdgeId index_array[64];
    for(int i=0; i < 64; i++)
        index_array[i] = i * highest_mult; // for each type of edge count we reserve highest_mult many spaces.

    // Count cnt = 0;
    Count max_static_mult = 0;
    Count min_static_mult = 1000;

    TemporalTime max_delta = max({delta_, delta1_, delta2_});

    for (VertexEdgeId i=0; i < s_dag.num_vertices_; i++)
    {
        VertexEdgeId u = i;
        for (VertexEdgeId j = s_dag.offsets_[i]; j < s_dag.offsets_[i+1]; j++)
        {
            VertexEdgeId v = s_dag.nbrs_[j]; // neighbor of u
            for (VertexEdgeId k = j+1; k < s_dag.offsets_[i+1]; k++)
            {
                VertexEdgeId w = s_dag.nbrs_[k]; // neighbor of u
                if(w == v)
                    continue;

                VertexEdgeId w_index = s_dag.getEdgeIndx(v,w); // check if w is a neigh of v
                if(w_index == -1)
                    continue;
                
                // we are looking at satic triangle <u,v,w>

                static_triangles_count_++;

                // Timer begin
                Timer t;
                t.Start();
                              
                // v is the mult_graph_indx_u_v's neighbor in mult_graph
                VertexEdgeId mult_graph_indx_u_v = mult_graph.getEdgeIndx(u, v);
                VertexEdgeId mult_graph_indx_u_w = mult_graph.getEdgeIndx(u, w);
                VertexEdgeId mult_graph_indx_v_u = mult_graph.getEdgeIndx(v, u);
                VertexEdgeId mult_graph_indx_v_w = mult_graph.getEdgeIndx(v, w);
                VertexEdgeId mult_graph_indx_w_u = mult_graph.getEdgeIndx(w, u);
                VertexEdgeId mult_graph_indx_w_v = mult_graph.getEdgeIndx(w, v);


                // starting point of temporal edges corresponding to each edge for example (u,v)
                VertexEdgeId start_pos_u_v = mult_graph_indx_u_v == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_u_v];
                VertexEdgeId start_pos_u_w = mult_graph_indx_u_w == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_u_w];
                VertexEdgeId start_pos_v_u = mult_graph_indx_v_u == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_v_u];
                VertexEdgeId start_pos_v_w = mult_graph_indx_v_w == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_v_w];
                VertexEdgeId start_pos_w_u = mult_graph_indx_w_u == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_w_u];
                VertexEdgeId start_pos_w_v = mult_graph_indx_w_v == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_w_v];


                // one past the actual end index
                VertexEdgeId end_pos_u_v = mult_graph_indx_u_v == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_u_v+1];
                VertexEdgeId end_pos_u_w = mult_graph_indx_u_w == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_u_w+1];
                VertexEdgeId end_pos_v_u = mult_graph_indx_v_u == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_v_u+1];
                VertexEdgeId end_pos_v_w = mult_graph_indx_v_w == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_v_w+1];
                VertexEdgeId end_pos_w_u = mult_graph_indx_w_u == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_w_u+1];
                VertexEdgeId end_pos_w_v = mult_graph_indx_w_v == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_w_v+1];

                // the actual temporal multiplicity of each static edge
                VertexEdgeId mult_u_v = start_pos_u_v == -1 ? 0 : end_pos_u_v - start_pos_u_v;
                VertexEdgeId mult_u_w = start_pos_u_w == -1 ? 0 : end_pos_u_w - start_pos_u_w;
                VertexEdgeId mult_v_u = start_pos_v_u == -1 ? 0 : end_pos_v_u - start_pos_v_u;
                VertexEdgeId mult_v_w = start_pos_v_w == -1 ? 0 : end_pos_v_w - start_pos_v_w;
                VertexEdgeId mult_w_u = start_pos_w_u == -1 ? 0 : end_pos_w_u - start_pos_w_u;
                VertexEdgeId mult_w_v = start_pos_w_v == -1 ? 0 : end_pos_w_v - start_pos_w_v;


                Count sum_mult = (mult_u_v + mult_v_u) * (mult_u_w + mult_w_u) * (mult_v_w + mult_w_v);

                TemporalTime start_time_u_v = start_pos_u_v == -1 ? -1 : t_graph.times_[start_pos_u_v];
                TemporalTime start_time_u_w = start_pos_u_w == -1 ? -1 : t_graph.times_[start_pos_u_w];
                TemporalTime start_time_v_u = start_pos_v_u == -1 ? -1 : t_graph.times_[start_pos_v_u];
                TemporalTime start_time_v_w = start_pos_v_w == -1 ? -1 : t_graph.times_[start_pos_v_w];
                TemporalTime start_time_w_u = start_pos_w_u == -1 ? -1 : t_graph.times_[start_pos_w_u];
                TemporalTime start_time_w_v = start_pos_w_v == -1 ? -1 : t_graph.times_[start_pos_w_v];

                TemporalTime end_time_u_v = end_pos_u_v == -1 ? -1 : t_graph.times_[end_pos_u_v-1];
                TemporalTime end_time_u_w = end_pos_u_w == -1 ? -1 : t_graph.times_[end_pos_u_w-1];
                TemporalTime end_time_v_u = end_pos_v_u == -1 ? -1 : t_graph.times_[end_pos_v_u-1];
                TemporalTime end_time_v_w = end_pos_v_w == -1 ? -1 : t_graph.times_[end_pos_v_w-1];
                TemporalTime end_time_w_u = end_pos_w_u == -1 ? -1 : t_graph.times_[end_pos_w_u-1];
                TemporalTime end_time_w_v = end_pos_w_v == -1 ? -1 : t_graph.times_[end_pos_w_v-1];

                t.Stop();
                times_[0] += t.Millisecs();

                if (
                    (
                        ( start_time_u_v == -1 || 
                            (
                                ( (start_time_v_w == -1) || (start_time_u_v > end_time_v_w + max_delta) || (end_time_u_v + max_delta < start_time_v_w) ) &&
                                ( (start_time_w_v == -1) || (start_time_u_v > end_time_w_v + max_delta) || (end_time_u_v + max_delta < start_time_w_v) )
                            ) ||
                            (
                                ( (start_time_u_w == -1) || (start_time_u_v > end_time_u_w + max_delta) || (end_time_u_v + max_delta < start_time_u_w) ) &&
                                ( (start_time_w_u == -1) || (start_time_u_v > end_time_w_u + max_delta) || (end_time_u_v + max_delta < start_time_w_u) )
                            )
                        ) &&
                        ( start_time_v_u == -1 || 
                            (
                                ( (start_time_v_w == -1) || (start_time_v_u > end_time_v_w + max_delta) || (end_time_v_u + max_delta < start_time_v_w) ) &&
                                ( (start_time_w_v == -1) || (start_time_v_u > end_time_w_v + max_delta) || (end_time_v_u + max_delta < start_time_w_v) )
                            ) ||
                            (
                                ( (start_time_u_w == -1) || (start_time_v_u > end_time_u_w + max_delta) || (end_time_v_u + max_delta < start_time_u_w) ) &&
                                ( (start_time_w_u == -1) || (start_time_v_u > end_time_w_u + max_delta) || (end_time_v_u + max_delta < start_time_w_u) )
                            )
                        )
                    ) ||
                    (
                        ( start_time_u_w == -1 || 
                            (
                                ( (start_time_v_w == -1) || (start_time_u_w > end_time_v_w + max_delta) || (end_time_u_w + max_delta < start_time_v_w) ) &&
                                ( (start_time_w_v == -1) || (start_time_u_w > end_time_w_v + max_delta) || (end_time_u_w + max_delta < start_time_w_v) )
                            ) ||
                            (
                                ( (start_time_u_v == -1) || (start_time_u_w > end_time_u_v + max_delta) || (end_time_u_w + max_delta < start_time_u_v) ) &&
                                ( (start_time_v_u == -1) || (start_time_u_w > end_time_v_u + max_delta) || (end_time_u_w + max_delta < start_time_v_u) )
                            )
                        ) &&
                        ( start_time_w_u == -1 || 
                            (
                                ( (start_time_v_w == -1) || (start_time_w_u > end_time_v_w + max_delta) || (end_time_w_u + max_delta < start_time_v_w) ) &&
                                ( (start_time_w_v == -1) || (start_time_w_u > end_time_w_v + max_delta) || (end_time_w_u + max_delta < start_time_w_v) )
                            ) ||
                            (
                                ( (start_time_u_v == -1) || (start_time_w_u > end_time_u_v + max_delta) || (end_time_w_u + max_delta < start_time_u_v) ) &&
                                ( (start_time_v_u == -1) || (start_time_w_u > end_time_v_u + max_delta) || (end_time_w_u + max_delta < start_time_v_u) )
                            )
                        )
                    ) ||
                    (
                        ( start_time_w_v == -1 || 
                            (
                                ( (start_time_u_w == -1) || (start_time_w_v > end_time_u_w + max_delta) || (end_time_w_v + max_delta < start_time_u_w) ) &&
                                ( (start_time_w_u == -1) || (start_time_w_v > end_time_w_u + max_delta) || (end_time_w_v + max_delta < start_time_w_u) )
                            ) ||
                            (
                                ( (start_time_u_v == -1) || (start_time_w_v > end_time_u_v + max_delta) || (end_time_w_v + max_delta < start_time_u_v) ) &&
                                ( (start_time_v_u == -1) || (start_time_w_v > end_time_v_u + max_delta) || (end_time_w_v + max_delta < start_time_v_u) )
                            )
                        ) &&
                        ( start_time_v_w == -1 || 
                            (
                                ( (start_time_u_w == -1) || (start_time_v_w > end_time_u_w + max_delta) || (end_time_v_w + max_delta < start_time_u_w) ) &&
                                ( (start_time_w_u == -1) || (start_time_v_w > end_time_w_u + max_delta) || (end_time_v_w + max_delta < start_time_w_u) )
                            ) ||
                            (
                                ( (start_time_u_v == -1) || (start_time_v_w > end_time_u_v + max_delta) || (end_time_v_w + max_delta < start_time_u_v) ) &&
                                ( (start_time_v_u == -1) || (start_time_v_w > end_time_v_u + max_delta) || (end_time_v_w + max_delta < start_time_v_u) )
                            )
                        )
                    )
                )
                {
                    skip_useless_cnt++;
                    continue;
                }
                

                // we are looking at a triangle u,v,w. Directions in the degeneracy DAG are (u,v), (u,w), and (v,w)
                // p: 6 directions in u->v, v->u, u->w, w->u, v->w, w->v
                // q: 8 patterns, T1-T8
                for(int p = 0; p < 6; p++)
                    for(int q = 0; q < 8; q++)
                        triangle_motif_counts_[p][q] = 0;
 
                //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                // Here we populate the auxilary arrays for edge interval counting
                //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                t.Start();
                // for each temporal edge with time t in (u,v) from (v,w)
                // search from (v,w) edge set, whose timestamp is within the time rage related to t(u,v)
                populateEdgeCount(t_graph, start_pos_u_v, end_pos_u_v, start_pos_v_w, end_pos_v_w,
                                index_array[0], index_array[1], index_array[2], index_array[3], index_array[32], index_array[40], index_array[48], index_array[56]);

                // for each temporal edge with time t in (u,v) from (w,v)
                populateEdgeCount(t_graph, start_pos_u_v, end_pos_u_v, start_pos_w_v, end_pos_w_v,
                                index_array[4], index_array[5], index_array[6], index_array[7], index_array[33], index_array[41], index_array[49], index_array[57]);

                // for each temporal edge with time t in (v,u) from (v,w)
                populateEdgeCount(t_graph, start_pos_v_u, end_pos_v_u, start_pos_v_w, end_pos_v_w,
                                index_array[8], index_array[9], index_array[10], index_array[11], index_array[34], index_array[42], index_array[50], index_array[58]);

                // for each temporal edge with time t in (v,u) from (w,v)
                populateEdgeCount(t_graph, start_pos_v_u, end_pos_v_u, start_pos_w_v, end_pos_w_v,
                                index_array[12], index_array[13], index_array[14], index_array[15], index_array[35], index_array[43], index_array[51], index_array[59]);

                // for each temporal edge with time t in (u,w) from (v,w)
                populateEdgeCount(t_graph, start_pos_u_w, end_pos_u_w, start_pos_v_w, end_pos_v_w,
                                index_array[16], index_array[17], index_array[18], index_array[19], index_array[36], index_array[44], index_array[52], index_array[60]);

                // for each temporal edge with time t in (u,w) from (w,v)
                populateEdgeCount(t_graph, start_pos_u_w, end_pos_u_w, start_pos_w_v, end_pos_w_v,
                                index_array[20], index_array[21], index_array[22], index_array[23], index_array[37], index_array[45], index_array[53], index_array[61]);

                // for each temporal edge with time t in (w,u) from (v,w)
                populateEdgeCount(t_graph, start_pos_w_u, end_pos_w_u, start_pos_v_w, end_pos_v_w,
                                index_array[24], index_array[25], index_array[26], index_array[27], index_array[38], index_array[46], index_array[54], index_array[62]);

                // for each temporal edge with time t in (w,u) from (w,v)
                populateEdgeCount(t_graph, start_pos_w_u, end_pos_w_u, start_pos_w_v, end_pos_w_v,
                                index_array[28], index_array[29], index_array[30], index_array[31], index_array[39], index_array[47], index_array[55], index_array[63]);

                t.Stop();
                times_[1] += t.Millisecs();
                // Dir Di is the equivalent of ordering \rho_i in the paper 
                // Mi is Triangle (motif) of type i: \Tau_i in the paper
                t.Start();
                // %%%%%%%%%%%%%%%%%%%%%%%%%                
                //Case A1 (\pi_1)
                // %%%%%%%%%%%%%%%%%%%%%%%%%

                /////////////////////////////////////////////////////////////////////
                //A1 Dir D1: M7
                triangle_motif_counts_[0][0] = countCaseA(t_graph,
                            start_pos_u_v, end_pos_u_v,
                            start_pos_u_w, end_pos_u_w,
                            start_pos_v_w, end_pos_v_w,
                            index_array[2], index_array[3], index_array[19], index_array[60]);
                // cout << "case A1 D1 : M7 addition: " << triangle_motif_counts_[0][0] << endl;
                motif_counts_[0][0] += triangle_motif_counts_[0][0];

                // /////////////////////////////////////////////////////////////////////
                // //A1 Dir D2: M6
                triangle_motif_counts_[0][1] = countCaseA(t_graph,
                            start_pos_u_v, end_pos_u_v,
                            start_pos_w_u, end_pos_w_u,
                            start_pos_w_v, end_pos_w_v,
                            index_array[6], index_array[7], index_array[31], index_array[63]);
                // cout << "case A1 D2 : M6 addition: " << triangle_motif_counts_[0][1] << endl;
                motif_counts_[0][1] += triangle_motif_counts_[0][1];

                // /////////////////////////////////////////////////////////////////////
                // //A1 Dir D3: M5
                triangle_motif_counts_[0][2] = countCaseA(t_graph,
                            start_pos_u_v, end_pos_u_v,
                            start_pos_u_w, end_pos_u_w,
                            start_pos_w_v, end_pos_w_v,
                            index_array[6], index_array[7], index_array[23], index_array[61]);
                // cout << "case A1 D3 : M5 addition: " << triangle_motif_counts_[0][2] << endl;
                motif_counts_[0][2] += triangle_motif_counts_[0][2];
             
                // /////////////////////////////////////////////////////////////////////
                // //A1 Dir D4: M8
                triangle_motif_counts_[0][3] = countCaseA(t_graph,
                            start_pos_u_v, end_pos_u_v,
                            start_pos_w_u, end_pos_w_u,
                            start_pos_v_w, end_pos_v_w,
                            index_array[2], index_array[3], index_array[27], index_array[62]);
                // cout << "case A1 D4 : M8 addition: " << triangle_motif_counts_[0][3] << endl;
                motif_counts_[0][3] += triangle_motif_counts_[0][3];
             
                // /////////////////////////////////////////////////////////////////////
                // //A1 Dir D5: M3
                triangle_motif_counts_[0][4] = countCaseA(t_graph,
                            start_pos_v_u, end_pos_v_u,
                            start_pos_u_w, end_pos_u_w,
                            start_pos_v_w, end_pos_v_w,
                            index_array[10], index_array[11], index_array[19], index_array[60]);
                // cout << "case A1 D5 : M3 addition: " << triangle_motif_counts_[0][4] << endl;
                motif_counts_[0][4] += triangle_motif_counts_[0][4];
             
                // /////////////////////////////////////////////////////////////////////
                // //A1 Dir D6: M2
                triangle_motif_counts_[0][5] = countCaseA(t_graph,
                            start_pos_v_u, end_pos_v_u,
                            start_pos_w_u, end_pos_w_u,
                            start_pos_w_v, end_pos_w_v,
                            index_array[14], index_array[15], index_array[31], index_array[63]);
                // cout << "case A1 D6 : M2 addition: " << triangle_motif_counts_[0][5] << endl;
                motif_counts_[0][5] += triangle_motif_counts_[0][5];
             
                // /////////////////////////////////////////////////////////////////////
                // //A1 Dir D7: M4
                // D7: v->u, u->w, w->v
                triangle_motif_counts_[0][6] = countCaseA(t_graph,
                            start_pos_v_u, end_pos_v_u,
                            start_pos_u_w, end_pos_u_w,
                            start_pos_w_v, end_pos_w_v,
                            index_array[14], index_array[15], index_array[23], index_array[61]);
                // cout << "case A1 D7 : M4 addition: " << triangle_motif_counts_[0][6] << endl;
                motif_counts_[0][6] += triangle_motif_counts_[0][6];
             
                // /////////////////////////////////////////////////////////////////////
                // //A1 Dir D8: M1
                triangle_motif_counts_[0][7] = countCaseA(t_graph,
                            start_pos_v_u, end_pos_v_u,
                            start_pos_w_u, end_pos_w_u,
                            start_pos_v_w, end_pos_v_w,
                            index_array[10], index_array[11], index_array[27], index_array[62]);
                // cout << "case A1 D8 : M1 addition: " << triangle_motif_counts_[0][7] << endl;
                motif_counts_[0][7] += triangle_motif_counts_[0][7];

                // %%%%%%%%%%%%%%%%%%%%%%%%%
                //Case A2 (\pi_2)
                // %%%%%%%%%%%%%%%%%%%%%%%%%                

                /////////////////////////////////////////////////////////////////////
                //A2 Dir D1: M5
                triangle_motif_counts_[1][0] = countCaseA(t_graph,
                            start_pos_u_w, end_pos_u_w,
                            start_pos_u_v, end_pos_u_v,
                            start_pos_v_w, end_pos_v_w,
                            index_array[18], index_array[19], index_array[3], index_array[56]);
                // cout << "case A2 D1 : M5 addition: " << triangle_motif_counts_[1][0] << endl;
                motif_counts_[1][0] += triangle_motif_counts_[1][0];

                /////////////////////////////////////////////////////////////////////
                //A2 Dir D2: M3
                triangle_motif_counts_[1][1] = countCaseA(t_graph,
                            start_pos_w_u, end_pos_w_u,
                            start_pos_u_v, end_pos_u_v,
                            start_pos_w_v, end_pos_w_v,
                            index_array[30], index_array[31], index_array[7], index_array[57]);
                // cout << "case A2 D2 : M3 addition: " << triangle_motif_counts_[1][1] << endl;
                motif_counts_[1][1] += triangle_motif_counts_[1][1];

                /////////////////////////////////////////////////////////////////////
                //A2 Dir D3: M7
                triangle_motif_counts_[1][2] = countCaseA(t_graph,
                            start_pos_u_w, end_pos_u_w,
                            start_pos_u_v, end_pos_u_v,
                            start_pos_w_v, end_pos_w_v,
                            index_array[22], index_array[23], index_array[7], index_array[57]);
                // cout << "case A2 D3 : M7 addition: " << triangle_motif_counts_[1][2] << endl;
                motif_counts_[1][2] += triangle_motif_counts_[1][2];

                /////////////////////////////////////////////////////////////////////
                //A2 Dir D4: M4
                triangle_motif_counts_[1][3] = countCaseA(t_graph,
                            start_pos_w_u, end_pos_w_u,
                            start_pos_u_v, end_pos_u_v,
                            start_pos_v_w, end_pos_v_w,
                            index_array[26], index_array[27], index_array[3], index_array[56]);
                // cout << "case A2 D4 : M4 addition: " << triangle_motif_counts_[1][3] << endl;
                motif_counts_[1][3] += triangle_motif_counts_[1][3];

                /////////////////////////////////////////////////////////////////////
                //A2 Dir D5: M6
                triangle_motif_counts_[1][4] = countCaseA(t_graph,
                            start_pos_u_w, end_pos_u_w,
                            start_pos_v_u, end_pos_v_u,
                            start_pos_v_w, end_pos_v_w,
                            index_array[18], index_array[19], index_array[11], index_array[58]);
                // cout << "case A2 D5 : M6 addition: " << triangle_motif_counts_[1][4] << endl;
                motif_counts_[1][4] += triangle_motif_counts_[1][4];

                /////////////////////////////////////////////////////////////////////
                //A2 Dir D6: M1
                triangle_motif_counts_[1][5] = countCaseA(t_graph,
                            start_pos_w_u, end_pos_w_u,
                            start_pos_v_u, end_pos_v_u,
                            start_pos_w_v, end_pos_w_v,
                            index_array[30], index_array[31], index_array[15], index_array[59]);
                // cout << "case A2 D6 : M1 addition: " << triangle_motif_counts_[1][5] << endl;
                motif_counts_[1][5] += triangle_motif_counts_[1][5];

                /////////////////////////////////////////////////////////////////////
                //A2 Dir D7: M8
                triangle_motif_counts_[1][6] = countCaseA(t_graph,
                            start_pos_u_w, end_pos_u_w,
                            start_pos_v_u, end_pos_v_u,
                            start_pos_w_v, end_pos_w_v,
                            index_array[22], index_array[23], index_array[15], index_array[59]);
                // cout << "case A2 D7 : M8 addition: " << triangle_motif_counts_[1][6] << endl;
                motif_counts_[1][6] += triangle_motif_counts_[1][6];

                /////////////////////////////////////////////////////////////////////
                //A2 Dir D8: M2
                triangle_motif_counts_[1][7] = countCaseA(t_graph,
                            start_pos_w_u, end_pos_w_u,
                            start_pos_v_u, end_pos_v_u,
                            start_pos_v_w, end_pos_v_w,
                            index_array[26], index_array[27], index_array[11], index_array[58]);
                // cout << "case A2 D8 : M2 addition: " << triangle_motif_counts_[1][7] << endl;
                motif_counts_[1][7] += triangle_motif_counts_[1][7];
                

                // %%%%%%%%%%%%%%%%%%%%%%%%%
                //Case B1 (\pi_3)
                // %%%%%%%%%%%%%%%%%%%%%%%%%                

                /////////////////////////////////////////////////////////////////////
                //B1 Dir D1: M3
                triangle_motif_counts_[2][0] = countCaseB(t_graph,
                            start_pos_u_v, end_pos_u_v,
                            start_pos_v_w, end_pos_v_w,
                            start_pos_u_w, end_pos_u_w,
                            index_array[40], index_array[0], index_array[52], index_array[16]);
                // cout << "case B1 D1 : M3 addition: " << triangle_motif_counts_[2][0] << endl;
                motif_counts_[2][0] += triangle_motif_counts_[2][0];


                /////////////////////////////////////////////////////////////////////
                //B1 Dir D2: M2
                triangle_motif_counts_[2][1] = countCaseB(t_graph,
                            start_pos_u_v, end_pos_u_v,
                            start_pos_w_v, end_pos_w_v,
                            start_pos_w_u, end_pos_w_u,
                            index_array[41], index_array[4], index_array[55], index_array[28]);
                // cout << "case B1 D2 : M2 addition: " << triangle_motif_counts_[2][1] << endl;
                motif_counts_[2][1] += triangle_motif_counts_[2][1];


                /////////////////////////////////////////////////////////////////////
                //B1 Dir D3: M1
                triangle_motif_counts_[2][2] = countCaseB(t_graph,
                            start_pos_u_v, end_pos_u_v,
                            start_pos_w_v, end_pos_w_v,
                            start_pos_u_w, end_pos_u_w,
                            index_array[41], index_array[4], index_array[53], index_array[20]);
                // cout << "case B1 D3 : M1 addition: " << triangle_motif_counts_[2][2] << endl;
                motif_counts_[2][2] += triangle_motif_counts_[2][2];
             

                /////////////////////////////////////////////////////////////////////
                //B1 Dir D4: M4
                triangle_motif_counts_[2][3] = countCaseB(t_graph,
                            start_pos_u_v, end_pos_u_v,
                            start_pos_v_w, end_pos_v_w,
                            start_pos_w_u, end_pos_w_u,
                            index_array[40], index_array[0], index_array[54], index_array[24]);
                // cout << "case B1 D4 : M4 addition: " << triangle_motif_counts_[2][3] << endl;
                motif_counts_[2][3] += triangle_motif_counts_[2][3];

                /////////////////////////////////////////////////////////////////////
                //B1 Dir D5: M7
                triangle_motif_counts_[2][4] = countCaseB(t_graph,
                            start_pos_v_u, end_pos_v_u,
                            start_pos_v_w, end_pos_v_w,
                            start_pos_u_w, end_pos_u_w,
                            index_array[42], index_array[8], index_array[52], index_array[16]);
                // cout << "case B1 D5 : M7 addition: " << triangle_motif_counts_[2][4] << endl;
                motif_counts_[2][4] += triangle_motif_counts_[2][4];
             

                /////////////////////////////////////////////////////////////////////
                //B1 Dir D6: M6
                triangle_motif_counts_[2][5] = countCaseB(t_graph,
                            start_pos_v_u, end_pos_v_u,
                            start_pos_w_v, end_pos_w_v,
                            start_pos_w_u, end_pos_w_u,
                            index_array[43], index_array[12], index_array[55], index_array[28]);
                // cout << "case B1 D6 : M6 addition: " << triangle_motif_counts_[2][5] << endl;
                motif_counts_[2][5] += triangle_motif_counts_[2][5];

                /////////////////////////////////////////////////////////////////////
                //B1 Dir D7: M8
                triangle_motif_counts_[2][6] = countCaseB(t_graph,
                            start_pos_v_u, end_pos_v_u,
                            start_pos_w_v, end_pos_w_v,
                            start_pos_u_w, end_pos_u_w,
                            index_array[43], index_array[12], index_array[53], index_array[20]);
                // cout << "case B1 D7 : M8 addition: " << triangle_motif_counts_[2][6] << endl;
                motif_counts_[2][6] += triangle_motif_counts_[2][6];
             

                /////////////////////////////////////////////////////////////////////
                //B1 Dir D8: M5
                triangle_motif_counts_[2][7] = countCaseB(t_graph,
                            start_pos_v_u, end_pos_v_u,
                            start_pos_v_w, end_pos_v_w,
                            start_pos_w_u, end_pos_w_u,
                            index_array[42], index_array[8], index_array[54], index_array[24]);
                // cout << "case B1 D8 : M5 addition: " << triangle_motif_counts_[2][7] << endl;
                motif_counts_[2][7] += triangle_motif_counts_[2][7];


                // %%%%%%%%%%%%%%%%%%%%%%%%%
                //Case B2 (\pi_4)
                // %%%%%%%%%%%%%%%%%%%%%%%%%                
                
                /////////////////////////////////////////////////////////////////////
                //B2 Dir D1: M1
                triangle_motif_counts_[3][0] = countCaseB(t_graph,
                            start_pos_u_w, end_pos_u_w,
                            start_pos_v_w, end_pos_v_w,
                            start_pos_u_v, end_pos_u_v,
                            index_array[44], index_array[16], index_array[48], index_array[0]);
                // cout << "case B2 D1 : M1 addition: " << triangle_motif_counts_[3][0] << endl;
                motif_counts_[3][0] += triangle_motif_counts_[3][0];


                /////////////////////////////////////////////////////////////////////
                //B2 Dir D2: M7
                triangle_motif_counts_[3][1] = countCaseB(t_graph,
                            start_pos_w_u, end_pos_w_u,
                            start_pos_w_v, end_pos_w_v,
                            start_pos_u_v, end_pos_u_v,
                            index_array[47], index_array[28], index_array[49], index_array[4]);
                // cout << "case B2 D2 : M7 addition: " << triangle_motif_counts_[3][1] << endl;
                motif_counts_[3][1] += triangle_motif_counts_[3][1];

                /////////////////////////////////////////////////////////////////////
                //B2 Dir D3: M3
                triangle_motif_counts_[3][2] = countCaseB(t_graph,
                            start_pos_u_w, end_pos_u_w,
                            start_pos_w_v, end_pos_w_v,
                            start_pos_u_v, end_pos_u_v,
                            index_array[45], index_array[20], index_array[49], index_array[4]);
                // cout << "case B2 D3 : M3 addition: " << triangle_motif_counts_[3][2] << endl;
                motif_counts_[3][2] += triangle_motif_counts_[3][2];

                /////////////////////////////////////////////////////////////////////
                //B2 Dir D4: M8
                triangle_motif_counts_[3][3] = countCaseB(t_graph,
                            start_pos_w_u, end_pos_w_u,
                            start_pos_v_w, end_pos_v_w,
                            start_pos_u_v, end_pos_u_v,
                            index_array[46], index_array[24], index_array[48], index_array[0]);
                // cout << "case B2 D4 : M8 addition: " << triangle_motif_counts_[3][3] << endl;
                motif_counts_[3][3] += triangle_motif_counts_[3][3];

                /////////////////////////////////////////////////////////////////////
                //B2 Dir D5: M2
                triangle_motif_counts_[3][4] = countCaseB(t_graph,
                            start_pos_u_w, end_pos_u_w,
                            start_pos_v_w, end_pos_v_w,
                            start_pos_v_u, end_pos_v_u,
                            index_array[44], index_array[16], index_array[50], index_array[8]);
                // cout << "case B2 D5 : M2 addition: " << triangle_motif_counts_[3][4] << endl;
                motif_counts_[3][4] += triangle_motif_counts_[3][4];

                /////////////////////////////////////////////////////////////////////
                //B2 Dir D6: M5
                triangle_motif_counts_[3][5] = countCaseB(t_graph,
                            start_pos_w_u, end_pos_w_u,
                            start_pos_w_v, end_pos_w_v,
                            start_pos_v_u, end_pos_v_u,
                            index_array[47], index_array[28], index_array[51], index_array[12]);
                // cout << "case B2 D6 : M5 addition: " << triangle_motif_counts_[3][5] << endl;
                motif_counts_[3][5] += triangle_motif_counts_[3][5];


                /////////////////////////////////////////////////////////////////////
                //B2 Dir D7: M4
                triangle_motif_counts_[3][6] = countCaseB(t_graph,
                            start_pos_u_w, end_pos_u_w,
                            start_pos_w_v, end_pos_w_v,
                            start_pos_v_u, end_pos_v_u,
                            index_array[45], index_array[20], index_array[51], index_array[12]);
                // cout << "case B2 D7 : M4 addition: " << triangle_motif_counts_[3][6] << endl;
                motif_counts_[3][6] += triangle_motif_counts_[3][6];


                /////////////////////////////////////////////////////////////////////
                //B2 Dir D8: M6
                triangle_motif_counts_[3][7] = countCaseB(t_graph,
                            start_pos_w_u, end_pos_w_u,
                            start_pos_v_w, end_pos_v_w,
                            start_pos_v_u, end_pos_v_u,
                            index_array[46], index_array[24], index_array[50], index_array[8]);
                // cout << "case B2 D8 : M6 addition: " << triangle_motif_counts_[3][7] << endl;
                motif_counts_[3][7] += triangle_motif_counts_[3][7];


                // %%%%%%%%%%%%%%%%%%%%%%%%%
                //Case C1 (\pi_5)
                // %%%%%%%%%%%%%%%%%%%%%%%%%                

                /////////////////////////////////////////////////////////////////////
                //C1 Dir D1: M6
                triangle_motif_counts_[4][0] = countCaseC(t_graph,
                            start_pos_v_w, end_pos_v_w,
                            start_pos_u_v, end_pos_u_v,
                            start_pos_u_w, end_pos_u_w,
                            index_array[17], index_array[16], index_array[0], index_array[32]);
                // cout << "case C1 D1 : M6 addition: " << triangle_motif_counts_[4][0] << endl;
                motif_counts_[4][0] += triangle_motif_counts_[4][0];

                /////////////////////////////////////////////////////////////////////
                //C1 Dir D2: M1
                triangle_motif_counts_[4][1] = countCaseC(t_graph,
                            start_pos_w_v, end_pos_w_v,
                            start_pos_u_v, end_pos_u_v,
                            start_pos_w_u, end_pos_w_u,
                            index_array[29], index_array[28], index_array[4], index_array[33]);
                // cout << "case C1 D2 : M1 addition: " << triangle_motif_counts_[4][1] << endl;
                motif_counts_[4][1] += triangle_motif_counts_[4][1];

                /////////////////////////////////////////////////////////////////////
                //C1 Dir D3: M2
                triangle_motif_counts_[4][2] = countCaseC(t_graph,
                            start_pos_w_v, end_pos_w_v,
                            start_pos_u_v, end_pos_u_v,
                            start_pos_u_w, end_pos_u_w,
                            index_array[21], index_array[20], index_array[4], index_array[33]);
                // cout << "case C1 D3 : M2 addition: " << triangle_motif_counts_[4][2] << endl;
                motif_counts_[4][2] += triangle_motif_counts_[4][2];

                /////////////////////////////////////////////////////////////////////
                //C1 Dir D4: M8
                triangle_motif_counts_[4][3] = countCaseC(t_graph,
                            start_pos_v_w, end_pos_v_w,
                            start_pos_u_v, end_pos_u_v,
                            start_pos_w_u, end_pos_w_u,
                            index_array[25], index_array[24], index_array[0], index_array[32]);
                // cout << "case C1 D4 : M8 addition: " << triangle_motif_counts_[4][3] << endl;
                motif_counts_[4][3] += triangle_motif_counts_[4][3];

                /////////////////////////////////////////////////////////////////////
                //C1 Dir D5: M5
                triangle_motif_counts_[4][4] = countCaseC(t_graph,
                            start_pos_v_w, end_pos_v_w,
                            start_pos_v_u, end_pos_v_u,
                            start_pos_u_w, end_pos_u_w,
                            index_array[17], index_array[16], index_array[8], index_array[34]);
                // cout << "case C1 D5 : M5 addition: " << triangle_motif_counts_[4][4] << endl;
                motif_counts_[4][4] += triangle_motif_counts_[4][4];

                /////////////////////////////////////////////////////////////////////
                //C1 Dir D6: M3
                triangle_motif_counts_[4][5] = countCaseC(t_graph,
                            start_pos_w_v, end_pos_w_v,
                            start_pos_v_u, end_pos_v_u,
                            start_pos_w_u, end_pos_w_u,
                            index_array[29], index_array[28], index_array[12], index_array[35]);
                // cout << "case C1 D6 : M3 addition: " << triangle_motif_counts_[4][5] << endl;
                motif_counts_[4][5] += triangle_motif_counts_[4][5];
               

                /////////////////////////////////////////////////////////////////////
                //C1 Dir D7: M4
                triangle_motif_counts_[4][6] = countCaseC(t_graph,
                            start_pos_w_v, end_pos_w_v,
                            start_pos_v_u, end_pos_v_u,
                            start_pos_u_w, end_pos_u_w,
                            index_array[21], index_array[20], index_array[12], index_array[35]);
                // cout << "case C1 D7 : M4 addition: " << triangle_motif_counts_[4][6] << endl;
                motif_counts_[4][6] += triangle_motif_counts_[4][6];

                /////////////////////////////////////////////////////////////////////
                //C1 Dir D8: M7
                triangle_motif_counts_[4][7] = countCaseC(t_graph,
                            start_pos_v_w, end_pos_v_w,
                            start_pos_v_u, end_pos_v_u,
                            start_pos_w_u, end_pos_w_u,
                            index_array[25], index_array[24], index_array[8], index_array[34]);
                // cout << "case C1 D8 : M7 addition: " << triangle_motif_counts_[4][7] << endl;
                motif_counts_[4][7] += triangle_motif_counts_[4][7];



                // %%%%%%%%%%%%%%%%%%%%%%%%%
                //Case C2 (\pi_6)
                // %%%%%%%%%%%%%%%%%%%%%%%%%                

                /////////////////////////////////////////////////////////////////////
                //C2 Dir D1: M2
                triangle_motif_counts_[5][0] = countCaseC(t_graph,
                            start_pos_v_w, end_pos_v_w,
                            start_pos_u_w, end_pos_u_w,
                            start_pos_u_v, end_pos_u_v,
                            index_array[1], index_array[0], index_array[16], index_array[36]);
                // cout << "case C2 D1 : M2 addition: " << triangle_motif_counts_[5][0] << endl;
                motif_counts_[5][0] += triangle_motif_counts_[5][0];

                /////////////////////////////////////////////////////////////////////
                //C2 Dir D2: M5
                triangle_motif_counts_[5][1] = countCaseC(t_graph,
                            start_pos_w_v, end_pos_w_v,
                            start_pos_w_u, end_pos_w_u,
                            start_pos_u_v, end_pos_u_v,
                            index_array[5], index_array[4], index_array[28], index_array[39]);
                // cout << "case C2 D2 : M5 addition: " << triangle_motif_counts_[5][1] << endl;
                motif_counts_[5][1] += triangle_motif_counts_[5][1];

                /////////////////////////////////////////////////////////////////////
                //C2 Dir D3: M6
                triangle_motif_counts_[5][2] = countCaseC(t_graph,
                            start_pos_w_v, end_pos_w_v,
                            start_pos_u_w, end_pos_u_w,
                            start_pos_u_v, end_pos_u_v,
                            index_array[5], index_array[4], index_array[20], index_array[37]);
                // cout << "case C2 D3 : M6 addition: " << triangle_motif_counts_[5][2] << endl;
                motif_counts_[5][2] += triangle_motif_counts_[5][2];

                /////////////////////////////////////////////////////////////////////
                //C2 Dir D4: M4
                triangle_motif_counts_[5][3] = countCaseC(t_graph,
                            start_pos_v_w, end_pos_v_w,
                            start_pos_w_u, end_pos_w_u,
                            start_pos_u_v, end_pos_u_v,
                            index_array[1], index_array[0], index_array[24], index_array[38]);
                // cout << "case C2 D4 : M4 addition: " << triangle_motif_counts_[5][3] << endl;
                motif_counts_[5][3] += triangle_motif_counts_[5][3];


                /////////////////////////////////////////////////////////////////////
                //C2 Dir D5: M1
                triangle_motif_counts_[5][4] = countCaseC(t_graph,
                            start_pos_v_w, end_pos_v_w,
                            start_pos_u_w, end_pos_u_w,
                            start_pos_v_u, end_pos_v_u,
                            index_array[9], index_array[8], index_array[16], index_array[36]);
                // cout << "case C2 D5 : M1 addition: " << triangle_motif_counts_[5][4] << endl;
                motif_counts_[5][4] += triangle_motif_counts_[5][4];


                /////////////////////////////////////////////////////////////////////
                //C2 Dir D6: M7
                triangle_motif_counts_[5][5] = countCaseC(t_graph,
                            start_pos_w_v, end_pos_w_v,
                            start_pos_w_u, end_pos_w_u,
                            start_pos_v_u, end_pos_v_u,
                            index_array[13], index_array[12], index_array[28], index_array[39]);
                // cout << "case C2 D6 : M7 addition: " << triangle_motif_counts_[5][5] << endl;
                motif_counts_[5][5] += triangle_motif_counts_[5][5];

                /////////////////////////////////////////////////////////////////////
                //C2 Dir D7: M8
                triangle_motif_counts_[5][6] = countCaseC(t_graph,
                            start_pos_w_v, end_pos_w_v,
                            start_pos_u_w, end_pos_u_w,
                            start_pos_v_u, end_pos_v_u,
                            index_array[13], index_array[12], index_array[20], index_array[37]);
                // cout << "case C2 D7 : M8 addition: " << triangle_motif_counts_[5][6] << endl;
                motif_counts_[5][6] += triangle_motif_counts_[5][6];

                /////////////////////////////////////////////////////////////////////
                //C2 Dir D8: M3
                triangle_motif_counts_[5][7] = countCaseC(t_graph,
                            start_pos_v_w, end_pos_v_w,
                            start_pos_w_u, end_pos_w_u,
                            start_pos_v_u, end_pos_v_u,
                            index_array[9], index_array[8], index_array[24], index_array[38]);
                // cout << "case C2 D8 : M3 addition: " << triangle_motif_counts_[5][7] << endl;
                motif_counts_[5][7] += triangle_motif_counts_[5][7];


                /////////////////////
                // adjustemnts for D4
                /////////////////////
                for (VertexEdgeId t_pos = start_pos_u_v; t_pos < end_pos_u_v; t_pos++)
                {
                    VertexEdgeId equals_w_u = t_graph.edgeTimeIntervalCount(start_pos_w_u, end_pos_w_u, t_graph.times_[t_pos], t_graph.times_[t_pos]);
                    if(equals_w_u > 0)
                    {
                        VertexEdgeId equals_v_w = t_graph.edgeTimeIntervalCount(start_pos_v_w, end_pos_v_w, t_graph.times_[t_pos], t_graph.times_[t_pos]);
                        if(equals_v_w > 0)
                        {
                            cout << "somthing" << endl;
                            motif_counts_[2][3] -= equals_w_u * equals_v_w;
                            motif_counts_[3][3] -= equals_w_u * equals_v_w;
                            motif_counts_[4][3] -= equals_w_u * equals_v_w;
                            motif_counts_[5][3] -= equals_w_u * equals_v_w;
                        }
                    }
                }


                /////////////////////
                // adjustemnts for D7
                /////////////////////
                for (VertexEdgeId t_pos = start_pos_v_u; t_pos < end_pos_v_u; t_pos++)
                {
                    VertexEdgeId equals_u_w = t_graph.edgeTimeIntervalCount(start_pos_u_w, end_pos_u_w, t_graph.times_[t_pos], t_graph.times_[t_pos]);
                    
                    if(equals_u_w > 0)
                    {
                        VertexEdgeId equals_w_v = t_graph.edgeTimeIntervalCount(start_pos_w_v, end_pos_w_v, t_graph.times_[t_pos], t_graph.times_[t_pos]);
                        if(equals_w_v > 0)
                        {
                            cout << "somthing" << endl;
                            motif_counts_[2][6] -= equals_u_w * equals_w_v;
                            motif_counts_[3][6] -= equals_u_w * equals_w_v;
                            motif_counts_[4][6] -= equals_u_w * equals_w_v;
                            motif_counts_[5][6] -= equals_u_w * equals_w_v;
                        }
                    }
                }

                t.Stop();
                times_[2] += t.Millisecs();

                Count temp_motif_cnts = 0; // # of triangle counts found within this static triangle
                bool useless = true;
                for(int i=0; i < 6; i++)
                {
                    for(int j=0; j < 8; j++)
                    {
                        if(triangle_motif_counts_[i][j] != 0)
                        {
                            useless = false;
                            // temp_motif_cnts += triangle_motif_counts_[i][j];
                        }
                    }
                }
                if (useless)
                {
                    useless_static_triangles_++;
                    useless_mult_cnt_ += sum_mult;
                    // useless_time_ += t.Millisecs();
                }
                else
                {
                    // if(sum_mult == 0)
                    // {
                    //     cout << "error" << endl;
                    //     return;
                    // }
                    useful_mult_cnt_ += sum_mult;
                    // update temporal_static_cnt_ map
                    // if(temporal_static_cnt_.count(temp_motif_cnts) == 0)
                    // {
                    //     temporal_static_cnt_[temp_motif_cnts] = 1;
                    // }
                    // else
                    // {
                    //     temporal_static_cnt_[temp_motif_cnts]++;
                    // }
                    // useful_time_ += t.Millisecs();
                }
                // max_static_mult = max(sum_mult, max_static_mult);
                // min_static_mult = min(sum_mult, min_static_mult);
            }
        }
        
    }
    // cout << "total triangle cnt: " << cnt << endl;
    // cout << "max multiplicity of a static triangle: " << max_static_mult << endl;
    // cout << "min multiplicity of a static triangle: " << min_static_mult << endl;

    // // case A1 (\pi_1)
    // cout << endl;
    // cout << "case A1:" << endl;
    // cout << "D1: " << motif_counts_[0][0] << "  D2: " << motif_counts_[0][1] << "  D3: " << motif_counts_[0][2] << "  D4: " << motif_counts_[0][3] << 
    // "  D5: " << motif_counts_[0][4] << "  D6: " << motif_counts_[0][5] << "  D7: " << motif_counts_[0][6] << "  D8: " << motif_counts_[0][7] << endl;

    // // case A2 (\pi_2)
    // cout << endl;
    // cout << "case A2:" << endl;
    // cout << "D1: " << motif_counts_[1][0] << "  D2: " << motif_counts_[1][1] << "  D3: " << motif_counts_[1][2] << "  D4: " << motif_counts_[1][3] << 
    // "  D5: " << motif_counts_[1][4] << "  D6: " << motif_counts_[1][5] << "  D7: " << motif_counts_[1][6] << "  D8: " << motif_counts_[1][7] << endl;

    // // case B1 (\pi_3)
    // cout << endl;
    // cout << "case B1:" << endl;
    // cout << "D1: " << motif_counts_[2][0] << "  D2: " << motif_counts_[2][1] << "  D3: " << motif_counts_[2][2] << "  D4: " << motif_counts_[2][3] << 
    // "  D5: " << motif_counts_[2][4] << "  D6: " << motif_counts_[2][5] << "  D7: " << motif_counts_[2][6] << "  D8: " << motif_counts_[2][7] << endl;


    // // case B2 (\pi_4)
    // cout << endl;
    // cout << "case B2:" << endl;
    // cout << "D1: " << motif_counts_[3][0] << "  D2: " << motif_counts_[3][1] << "  D3: " << motif_counts_[3][2] << "  D4: " << motif_counts_[3][3] << 
    // "  D5: " << motif_counts_[3][4] << "  D6: " << motif_counts_[3][5] << "  D7: " << motif_counts_[3][6] << "  D8: " << motif_counts_[3][7] << endl;


    // // case C1 (\pi_5)
    // cout << endl;
    // cout << "case C1:" << endl;
    // cout << "D1: " << motif_counts_[4][0] << "  D2: " << motif_counts_[4][1] << "  D3: " << motif_counts_[4][2] << "  D4: " << motif_counts_[4][3] << 
    // "  D5: " << motif_counts_[4][4] << "  D6: " << motif_counts_[4][5] << "  D7: " << motif_counts_[4][6] << "  D8: " << motif_counts_[4][7] << endl;


    // // case C2 (\pi_6)
    // cout << endl;
    // cout << "case C2:" << endl;
    // cout << "D1: " << motif_counts_[5][0] << "  D2: " << motif_counts_[5][1] << "  D3: " << motif_counts_[5][2] << "  D4: " << motif_counts_[5][3] << 
    // "  D5: " << motif_counts_[5][4] << "  D6: " << motif_counts_[5][5] << "  D7: " << motif_counts_[5][6] << "  D8: " << motif_counts_[5][7] << endl;


    // getting the counts for each temporal triangle type from the counts for different temporal orderings and orientations 
    motif_type_counts_[0] = motif_counts_[0][7] + motif_counts_[1][5] + motif_counts_[2][2] + motif_counts_[3][0] + motif_counts_[4][1] + motif_counts_[5][4];
    motif_type_counts_[1] = motif_counts_[0][5] + motif_counts_[1][7] + motif_counts_[2][1] + motif_counts_[3][4] + motif_counts_[4][2] + motif_counts_[5][0];
    motif_type_counts_[2] = motif_counts_[0][4] + motif_counts_[1][1] + motif_counts_[2][0] + motif_counts_[3][2] + motif_counts_[4][5] + motif_counts_[5][7];
    motif_type_counts_[3] = motif_counts_[0][6] + motif_counts_[1][3] + motif_counts_[2][3] + motif_counts_[3][6] + motif_counts_[4][6] + motif_counts_[5][3];
    motif_type_counts_[4] = motif_counts_[0][2] + motif_counts_[1][0] + motif_counts_[2][7] + motif_counts_[3][5] + motif_counts_[4][4] + motif_counts_[5][1];
    motif_type_counts_[5] = motif_counts_[0][1] + motif_counts_[1][4] + motif_counts_[2][5] + motif_counts_[3][7] + motif_counts_[4][0] + motif_counts_[5][2];
    motif_type_counts_[6] = motif_counts_[0][0] + motif_counts_[1][2] + motif_counts_[2][4] + motif_counts_[3][1] + motif_counts_[4][7] + motif_counts_[5][5];
    motif_type_counts_[7] = motif_counts_[0][3] + motif_counts_[1][6] + motif_counts_[2][6] + motif_counts_[3][3] + motif_counts_[4][3] + motif_counts_[5][6];

    mult_graph.deleteGraph(); // free memory
}

void MotifCounter::hashE3E1Key(CSRTemporalGraph &t_graph,
                            VertexEdgeId t1_start_pos, VertexEdgeId t1_end_pos,
                            VertexEdgeId t2_start_pos, VertexEdgeId t2_end_pos,
                            VertexEdgeId t3_start_pos, VertexEdgeId t3_end_pos,
                            unordered_map<VertexEdgeId, map<tuple<TemporalTime, VertexEdgeId>, Count>>& e3_e1_map)
{
    if ((t1_end_pos - t1_start_pos == 0) || (t2_end_pos - t2_start_pos == 0 ) || (t3_end_pos - t3_start_pos == 0 ))
    {
        return;
    }
    for (VertexEdgeId t3_pos = t3_start_pos; t3_pos < t3_end_pos; t3_pos++)
    {
        // e1 in [t3 - \delta, t3]
        VertexEdgeId t1_on_t3_start = t_graph.edgeTimeMinLimitSearch(t1_start_pos, t1_end_pos, t_graph.times_[t3_pos] - delta_);
        VertexEdgeId t1_on_t3_end = t_graph.edgeTimeMaxLimitSearch(t1_start_pos, t1_end_pos, t_graph.times_[t3_pos]);
        if ((t1_on_t3_start == -1) || (t1_on_t3_end == -1) || (t1_on_t3_start == t1_on_t3_end)) continue;
        // e2 in [t3 - \delta, t3]
        VertexEdgeId t2_on_t3_start = t_graph.edgeTimeMinLimitSearch(t2_start_pos, t2_end_pos, t_graph.times_[t3_pos] - delta_);
        VertexEdgeId t2_on_t3_end = t_graph.edgeTimeMaxLimitSearch(t2_start_pos, t2_end_pos, t_graph.times_[t3_pos]);
        if ((t2_on_t3_start == -1) || (t2_on_t3_end == -1) || (t2_on_t3_start == t2_on_t3_end)) continue;
        for (VertexEdgeId t1_pos = t1_on_t3_start; t1_pos < t1_on_t3_end; t1_pos++)
        {
            VertexEdgeId t2_pos = t2_on_t3_start;
            while(t2_pos < t2_on_t3_end && t_graph.times_[t2_pos] < t_graph.times_[t1_pos])
            {
                t2_pos++;
            }
            if (t2_on_t3_end - t2_pos == 0)
                break;
            // e3_e1_map[e3][e1] += e2 temporal multiplicity
            tuple<TemporalTime, VertexEdgeId> t1_e1dst = {t_graph.times_[t1_pos],  t_graph.temporal_nbrs_[t1_pos].dst_};
            Util::unorderedmapMapAddCntAndCreatIfNotExist<VertexEdgeId, tuple<TemporalTime, VertexEdgeId>, Count>(e3_e1_map, t3_pos, t1_e1dst, t2_on_t3_end - t2_pos);
        }
    }
}

void MotifCounter::hashE2E1Key(CSRTemporalGraph &t_graph,
                            VertexEdgeId t1_start_pos, VertexEdgeId t1_end_pos,
                            VertexEdgeId t2_start_pos, VertexEdgeId t2_end_pos,
                            VertexEdgeId t3_start_pos, VertexEdgeId t3_end_pos,
                            unordered_map<VertexEdgeId, map<tuple<TemporalTime, VertexEdgeId>, Count>>& e2_e1_map)
{
    if ((t1_end_pos - t1_start_pos == 0) || (t2_end_pos - t2_start_pos == 0 ) || (t3_end_pos - t3_start_pos == 0 ))
    {
        return;
    }
    for (VertexEdgeId t2_pos = t2_start_pos; t2_pos < t2_end_pos; t2_pos++)
    {
        // e1 in [t2 - \delta, t2]
        VertexEdgeId t1_on_t2_start = t_graph.edgeTimeMinLimitSearch(t1_start_pos, t1_end_pos, t_graph.times_[t2_pos] - delta_);
        VertexEdgeId t1_on_t2_end = t_graph.edgeTimeMaxLimitSearch(t1_start_pos, t1_end_pos, t_graph.times_[t2_pos]);
        if ((t1_on_t2_start == -1) || (t1_on_t2_end == -1) || (t1_on_t2_start == t1_on_t2_end)) continue;
        // e3 in [t2, t2 + \delta]
        VertexEdgeId t3_on_t2_start = t_graph.edgeTimeMinLimitSearch(t3_start_pos, t3_end_pos, t_graph.times_[t2_pos]);
        VertexEdgeId t3_on_t2_end = t_graph.edgeTimeMaxLimitSearch(t3_start_pos, t3_end_pos, t_graph.times_[t2_pos] + delta_);
        if ((t3_on_t2_start == -1) || (t3_on_t2_end == -1) || (t3_on_t2_start == t3_on_t2_end)) continue;
        for (VertexEdgeId t1_pos = t1_on_t2_start; t1_pos < t1_on_t2_end; t1_pos++)
        {
            VertexEdgeId t3_pos = t3_on_t2_start;
            while(t3_pos < t3_on_t2_end && t_graph.times_[t3_pos] <= t_graph.times_[t1_pos] + delta_)
            {
                t3_pos++;
            }
            if (t3_pos - t3_on_t2_start == 0) continue;
            // e2_e1_map[e2][e1] += e2 temporal multiplicity
            tuple<TemporalTime, VertexEdgeId> t1_e1src = {t_graph.times_[t1_pos], t_graph.getSrcFromEdgeIndex(t1_pos)};
            Util::unorderedmapMapAddCntAndCreatIfNotExist<VertexEdgeId, tuple<TemporalTime, VertexEdgeId>, Count>(e2_e1_map, t2_pos, t1_e1src, t3_pos - t3_on_t2_start);
        }
    }
}

void MotifCounter::countTemporalChordal4Cycle(CSRGraph &s_dag, CSRTemporalGraph &t_graph, TemporalTime delta, TemporalTime delta1, TemporalTime delta2)
{
    CSRGraph mult_graph = t_graph.extractMultGraph(); // mult_graph is the temporal graph without temporal mutiplicity
    VertexEdgeId highest_mult = 0;
    for (VertexEdgeId i = 0; i < mult_graph.num_edges_; i++)
        highest_mult = max(highest_mult, mult_graph.temporal_start_pos_[i+1] - mult_graph.temporal_start_pos_[i]);
    highest_mult_ = highest_mult;


    // reload edge times from temporal_Nbrs_ to times_
    t_graph.reloadTimes();


    edge_count = new VertexEdgeId[64 * highest_mult];
    edge_count_cum = new VertexEdgeId[64 * highest_mult];
    delta_ = delta;
    delta1_ = delta1;
    delta2_ = delta2;

    // // we have 64 different types of edge counts as shown at the top of page
    // VertexEdgeId index_array[64];
    // for(int i=0; i < 64; i++)
    //     index_array[i] = i * highest_mult; // for each type of edge count we reserve highest_mult many spaces.

    Count cnt = 0;

    TemporalTime max_delta = max({delta_, delta1_, delta2_});

    // https://stackoverflow.com/questions/36240232/comparator-with-capture
    // TemporalTime *times_ = t_graph.times_;
    // auto timestampComp = [&times_](const VertexEdgeId & a, const VertexEdgeId & b) { return times_[a] < times_[b]; };

    unordered_map<VertexEdgeId, map<tuple<TemporalTime, VertexEdgeId>, Count>> e2_e1_map;
    unordered_map<VertexEdgeId, map<tuple<TemporalTime, VertexEdgeId>, Count>> e3_e1_map;

    for (VertexEdgeId i=0; i < s_dag.num_vertices_; i++)
    {
        VertexEdgeId u = i;
        for (VertexEdgeId j = s_dag.offsets_[i]; j < s_dag.offsets_[i+1]; j++)
        {
            VertexEdgeId v = s_dag.nbrs_[j]; // neighbor of u
            for (VertexEdgeId k = j+1; k < s_dag.offsets_[i+1]; k++)
            {
                VertexEdgeId w = s_dag.nbrs_[k]; // neighbor of u
                if(w == v)
                    continue;

                VertexEdgeId w_index = s_dag.getEdgeIndx(v,w); // check if w is a neigh of v
                if(w_index == -1)
                    continue;
                
                // we are looking at satic triangle <u,v,w>

                static_triangles_count_++;
                // cout << "u: " << u << ", v: " << v << ", w: " << w << endl;
                              
                // v is the mult_graph_indx_u_v's neighbor in mult_graph
                VertexEdgeId mult_graph_indx_u_v = mult_graph.getEdgeIndx(u, v);
                VertexEdgeId mult_graph_indx_u_w = mult_graph.getEdgeIndx(u, w);
                VertexEdgeId mult_graph_indx_v_u = mult_graph.getEdgeIndx(v, u);
                VertexEdgeId mult_graph_indx_v_w = mult_graph.getEdgeIndx(v, w);
                VertexEdgeId mult_graph_indx_w_u = mult_graph.getEdgeIndx(w, u);
                VertexEdgeId mult_graph_indx_w_v = mult_graph.getEdgeIndx(w, v);


                // starting point of temporal edges corresponding to each edge for example (u,v)
                VertexEdgeId start_pos_u_v = mult_graph_indx_u_v == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_u_v];
                VertexEdgeId start_pos_u_w = mult_graph_indx_u_w == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_u_w];
                VertexEdgeId start_pos_v_u = mult_graph_indx_v_u == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_v_u];
                VertexEdgeId start_pos_v_w = mult_graph_indx_v_w == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_v_w];
                VertexEdgeId start_pos_w_u = mult_graph_indx_w_u == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_w_u];
                VertexEdgeId start_pos_w_v = mult_graph_indx_w_v == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_w_v];


                // one past the actual end index
                VertexEdgeId end_pos_u_v = mult_graph_indx_u_v == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_u_v+1];
                VertexEdgeId end_pos_u_w = mult_graph_indx_u_w == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_u_w+1];
                VertexEdgeId end_pos_v_u = mult_graph_indx_v_u == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_v_u+1];
                VertexEdgeId end_pos_v_w = mult_graph_indx_v_w == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_v_w+1];
                VertexEdgeId end_pos_w_u = mult_graph_indx_w_u == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_w_u+1];
                VertexEdgeId end_pos_w_v = mult_graph_indx_w_v == -1 ? -1 : mult_graph.temporal_start_pos_[mult_graph_indx_w_v+1];


                TemporalTime start_time_u_v = start_pos_u_v == -1 ? -1 : t_graph.times_[start_pos_u_v];
                TemporalTime start_time_u_w = start_pos_u_w == -1 ? -1 : t_graph.times_[start_pos_u_w];
                TemporalTime start_time_v_u = start_pos_v_u == -1 ? -1 : t_graph.times_[start_pos_v_u];
                TemporalTime start_time_v_w = start_pos_v_w == -1 ? -1 : t_graph.times_[start_pos_v_w];
                TemporalTime start_time_w_u = start_pos_w_u == -1 ? -1 : t_graph.times_[start_pos_w_u];
                TemporalTime start_time_w_v = start_pos_w_v == -1 ? -1 : t_graph.times_[start_pos_w_v];

                TemporalTime end_time_u_v = end_pos_u_v == -1 ? -1 : t_graph.times_[end_pos_u_v-1];
                TemporalTime end_time_u_w = end_pos_u_w == -1 ? -1 : t_graph.times_[end_pos_u_w-1];
                TemporalTime end_time_v_u = end_pos_v_u == -1 ? -1 : t_graph.times_[end_pos_v_u-1];
                TemporalTime end_time_v_w = end_pos_v_w == -1 ? -1 : t_graph.times_[end_pos_v_w-1];
                TemporalTime end_time_w_u = end_pos_w_u == -1 ? -1 : t_graph.times_[end_pos_w_u-1];
                TemporalTime end_time_w_v = end_pos_w_v == -1 ? -1 : t_graph.times_[end_pos_w_v-1];


                if (
                    (
                        ( start_time_u_v == -1 || 
                            (
                                ( (start_time_v_w == -1) || (start_time_u_v > end_time_v_w + max_delta) || (end_time_u_v + max_delta < start_time_v_w) ) &&
                                ( (start_time_w_v == -1) || (start_time_u_v > end_time_w_v + max_delta) || (end_time_u_v + max_delta < start_time_w_v) )
                            ) ||
                            (
                                ( (start_time_u_w == -1) || (start_time_u_v > end_time_u_w + max_delta) || (end_time_u_v + max_delta < start_time_u_w) ) &&
                                ( (start_time_w_u == -1) || (start_time_u_v > end_time_w_u + max_delta) || (end_time_u_v + max_delta < start_time_w_u) )
                            )
                        ) &&
                        ( start_time_v_u == -1 || 
                            (
                                ( (start_time_v_w == -1) || (start_time_v_u > end_time_v_w + max_delta) || (end_time_v_u + max_delta < start_time_v_w) ) &&
                                ( (start_time_w_v == -1) || (start_time_v_u > end_time_w_v + max_delta) || (end_time_v_u + max_delta < start_time_w_v) )
                            ) ||
                            (
                                ( (start_time_u_w == -1) || (start_time_v_u > end_time_u_w + max_delta) || (end_time_v_u + max_delta < start_time_u_w) ) &&
                                ( (start_time_w_u == -1) || (start_time_v_u > end_time_w_u + max_delta) || (end_time_v_u + max_delta < start_time_w_u) )
                            )
                        )
                    ) ||
                    (
                        ( start_time_u_w == -1 || 
                            (
                                ( (start_time_v_w == -1) || (start_time_u_w > end_time_v_w + max_delta) || (end_time_u_w + max_delta < start_time_v_w) ) &&
                                ( (start_time_w_v == -1) || (start_time_u_w > end_time_w_v + max_delta) || (end_time_u_w + max_delta < start_time_w_v) )
                            ) ||
                            (
                                ( (start_time_u_v == -1) || (start_time_u_w > end_time_u_v + max_delta) || (end_time_u_w + max_delta < start_time_u_v) ) &&
                                ( (start_time_v_u == -1) || (start_time_u_w > end_time_v_u + max_delta) || (end_time_u_w + max_delta < start_time_v_u) )
                            )
                        ) &&
                        ( start_time_w_u == -1 || 
                            (
                                ( (start_time_v_w == -1) || (start_time_w_u > end_time_v_w + max_delta) || (end_time_w_u + max_delta < start_time_v_w) ) &&
                                ( (start_time_w_v == -1) || (start_time_w_u > end_time_w_v + max_delta) || (end_time_w_u + max_delta < start_time_w_v) )
                            ) ||
                            (
                                ( (start_time_u_v == -1) || (start_time_w_u > end_time_u_v + max_delta) || (end_time_w_u + max_delta < start_time_u_v) ) &&
                                ( (start_time_v_u == -1) || (start_time_w_u > end_time_v_u + max_delta) || (end_time_w_u + max_delta < start_time_v_u) )
                            )
                        )
                    ) ||
                    (
                        ( start_time_w_v == -1 || 
                            (
                                ( (start_time_u_w == -1) || (start_time_w_v > end_time_u_w + max_delta) || (end_time_w_v + max_delta < start_time_u_w) ) &&
                                ( (start_time_w_u == -1) || (start_time_w_v > end_time_w_u + max_delta) || (end_time_w_v + max_delta < start_time_w_u) )
                            ) ||
                            (
                                ( (start_time_u_v == -1) || (start_time_w_v > end_time_u_v + max_delta) || (end_time_w_v + max_delta < start_time_u_v) ) &&
                                ( (start_time_v_u == -1) || (start_time_w_v > end_time_v_u + max_delta) || (end_time_w_v + max_delta < start_time_v_u) )
                            )
                        ) &&
                        ( start_time_v_w == -1 || 
                            (
                                ( (start_time_u_w == -1) || (start_time_v_w > end_time_u_w + max_delta) || (end_time_v_w + max_delta < start_time_u_w) ) &&
                                ( (start_time_w_u == -1) || (start_time_v_w > end_time_w_u + max_delta) || (end_time_v_w + max_delta < start_time_w_u) )
                            ) ||
                            (
                                ( (start_time_u_v == -1) || (start_time_v_w > end_time_u_v + max_delta) || (end_time_v_w + max_delta < start_time_u_v) ) &&
                                ( (start_time_v_u == -1) || (start_time_v_w > end_time_v_u + max_delta) || (end_time_v_w + max_delta < start_time_v_u) )
                            )
                        )
                    )
                )
                {
                    skip_useless_cnt++;
                    continue;
                }
                

                // we are looking at a triangle u,v,w. Directions in the degeneracy DAG are (u,v), (u,w), and (v,w)
                // p: 6 directions in u->v, v->u, u->w, w->u, v->w, w->v
                // q: 8 patterns, T1-T8
                for(int p = 0; p < 6; p++)
                    for(int q = 0; q < 8; q++)
                        triangle_motif_counts_[p][q] = 0;
 

                // Dir Di is the equivalent of ordering \rho_i in the paper 
                // Mi is Triangle (motif) of type i: \Tau_i in the paper
                // %%%%%%%%%%%%%%%%%%%%%%%%%                
                //Case A1 (\pi_1)
                // %%%%%%%%%%%%%%%%%%%%%%%%%

                // /////////////////////////////////////////////////////////////////////
                // //A1 Dir D1: M7
                // triangle_motif_counts_[0][0] = countCaseA(t_graph,
                //             start_pos_u_v, end_pos_u_v,
                //             start_pos_u_w, end_pos_u_w,
                //             start_pos_v_w, end_pos_v_w,
                //             index_array[2], index_array[3], index_array[19], index_array[60]);
                // // cout << "case A1 D1 : M7 addition: " << triangle_motif_counts_[0][0] << endl;
                // motif_counts_[0][0] += triangle_motif_counts_[0][0];

                // // /////////////////////////////////////////////////////////////////////
                // // //A1 Dir D2: M6
                // triangle_motif_counts_[0][1] = countCaseA(t_graph,
                //             start_pos_u_v, end_pos_u_v,
                //             start_pos_w_u, end_pos_w_u,
                //             start_pos_w_v, end_pos_w_v,
                //             index_array[6], index_array[7], index_array[31], index_array[63]);
                // // cout << "case A1 D2 : M6 addition: " << triangle_motif_counts_[0][1] << endl;
                // motif_counts_[0][1] += triangle_motif_counts_[0][1];

                // // /////////////////////////////////////////////////////////////////////
                // // //A1 Dir D3: M5
                // triangle_motif_counts_[0][2] = countCaseA(t_graph,
                //             start_pos_u_v, end_pos_u_v,
                //             start_pos_u_w, end_pos_u_w,
                //             start_pos_w_v, end_pos_w_v,
                //             index_array[6], index_array[7], index_array[23], index_array[61]);
                // // cout << "case A1 D3 : M5 addition: " << triangle_motif_counts_[0][2] << endl;
                // motif_counts_[0][2] += triangle_motif_counts_[0][2];
             
                // // /////////////////////////////////////////////////////////////////////
                // // //A1 Dir D4: M8
                // triangle_motif_counts_[0][3] = countCaseA(t_graph,
                //             start_pos_u_v, end_pos_u_v,
                //             start_pos_w_u, end_pos_w_u,
                //             start_pos_v_w, end_pos_v_w,
                //             index_array[2], index_array[3], index_array[27], index_array[62]);
                // // cout << "case A1 D4 : M8 addition: " << triangle_motif_counts_[0][3] << endl;
                // motif_counts_[0][3] += triangle_motif_counts_[0][3];
             
                // // /////////////////////////////////////////////////////////////////////
                // // //A1 Dir D5: M3
                // triangle_motif_counts_[0][4] = countCaseA(t_graph,
                //             start_pos_v_u, end_pos_v_u,
                //             start_pos_u_w, end_pos_u_w,
                //             start_pos_v_w, end_pos_v_w,
                //             index_array[10], index_array[11], index_array[19], index_array[60]);
                // // cout << "case A1 D5 : M3 addition: " << triangle_motif_counts_[0][4] << endl;
                // motif_counts_[0][4] += triangle_motif_counts_[0][4];
             
                // /////////////////////////////////////////////////////////////////////
                // //A1 Dir D6: M2
                hashE2E1Key(t_graph,
                            start_pos_v_u, end_pos_v_u,
                            start_pos_w_u, end_pos_w_u,
                            start_pos_w_v, end_pos_w_v,
                            e2_e1_map);
                             
                // /////////////////////////////////////////////////////////////////////
                // //A1 Dir D7: M4
                // D7: v->u, u->w, w->v
                hashE3E1Key(t_graph,
                            start_pos_v_u, end_pos_v_u,
                            start_pos_u_w, end_pos_u_w,
                            start_pos_w_v, end_pos_w_v,
                            e3_e1_map);
             
                // // /////////////////////////////////////////////////////////////////////
                // // //A1 Dir D8: M1
                // triangle_motif_counts_[0][7] = countCaseA(t_graph,
                //             start_pos_v_u, end_pos_v_u,
                //             start_pos_w_u, end_pos_w_u,
                //             start_pos_v_w, end_pos_v_w,
                //             index_array[10], index_array[11], index_array[27], index_array[62]);
                // // cout << "case A1 D8 : M1 addition: " << triangle_motif_counts_[0][7] << endl;
                // motif_counts_[0][7] += triangle_motif_counts_[0][7];

                // %%%%%%%%%%%%%%%%%%%%%%%%%
                //Case A2 (\pi_2)
                // %%%%%%%%%%%%%%%%%%%%%%%%%                

                // /////////////////////////////////////////////////////////////////////
                // //A2 Dir D1: M5
                // triangle_motif_counts_[1][0] = countCaseA(t_graph,
                //             start_pos_u_w, end_pos_u_w,
                //             start_pos_u_v, end_pos_u_v,
                //             start_pos_v_w, end_pos_v_w,
                //             index_array[18], index_array[19], index_array[3], index_array[56]);
                // // cout << "case A2 D1 : M5 addition: " << triangle_motif_counts_[1][0] << endl;
                // motif_counts_[1][0] += triangle_motif_counts_[1][0];

                // /////////////////////////////////////////////////////////////////////
                // //A2 Dir D2: M3
                // triangle_motif_counts_[1][1] = countCaseA(t_graph,
                //             start_pos_w_u, end_pos_w_u,
                //             start_pos_u_v, end_pos_u_v,
                //             start_pos_w_v, end_pos_w_v,
                //             index_array[30], index_array[31], index_array[7], index_array[57]);
                // // cout << "case A2 D2 : M3 addition: " << triangle_motif_counts_[1][1] << endl;
                // motif_counts_[1][1] += triangle_motif_counts_[1][1];

                // /////////////////////////////////////////////////////////////////////
                // //A2 Dir D3: M7
                // triangle_motif_counts_[1][2] = countCaseA(t_graph,
                //             start_pos_u_w, end_pos_u_w,
                //             start_pos_u_v, end_pos_u_v,
                //             start_pos_w_v, end_pos_w_v,
                //             index_array[22], index_array[23], index_array[7], index_array[57]);
                // // cout << "case A2 D3 : M7 addition: " << triangle_motif_counts_[1][2] << endl;
                // motif_counts_[1][2] += triangle_motif_counts_[1][2];

                /////////////////////////////////////////////////////////////////////
                //A2 Dir D4: M4
                hashE3E1Key(t_graph,
                            start_pos_w_u, end_pos_w_u,
                            start_pos_u_v, end_pos_u_v,
                            start_pos_v_w, end_pos_v_w,
                            e3_e1_map);
                
                // /////////////////////////////////////////////////////////////////////
                // //A2 Dir D5: M6
                // triangle_motif_counts_[1][4] = countCaseA(t_graph,
                //             start_pos_u_w, end_pos_u_w,
                //             start_pos_v_u, end_pos_v_u,
                //             start_pos_v_w, end_pos_v_w,
                //             index_array[18], index_array[19], index_array[11], index_array[58]);
                // // cout << "case A2 D5 : M6 addition: " << triangle_motif_counts_[1][4] << endl;
                // motif_counts_[1][4] += triangle_motif_counts_[1][4];

                // /////////////////////////////////////////////////////////////////////
                // //A2 Dir D6: M1
                // triangle_motif_counts_[1][5] = countCaseA(t_graph,
                //             start_pos_w_u, end_pos_w_u,
                //             start_pos_v_u, end_pos_v_u,
                //             start_pos_w_v, end_pos_w_v,
                //             index_array[30], index_array[31], index_array[15], index_array[59]);
                // // cout << "case A2 D6 : M1 addition: " << triangle_motif_counts_[1][5] << endl;
                // motif_counts_[1][5] += triangle_motif_counts_[1][5];

                // /////////////////////////////////////////////////////////////////////
                // //A2 Dir D7: M8
                // triangle_motif_counts_[1][6] = countCaseA(t_graph,
                //             start_pos_u_w, end_pos_u_w,
                //             start_pos_v_u, end_pos_v_u,
                //             start_pos_w_v, end_pos_w_v,
                //             index_array[22], index_array[23], index_array[15], index_array[59]);
                // // cout << "case A2 D7 : M8 addition: " << triangle_motif_counts_[1][6] << endl;
                // motif_counts_[1][6] += triangle_motif_counts_[1][6];

                /////////////////////////////////////////////////////////////////////
                //A2 Dir D8: M2
                hashE2E1Key(t_graph,
                            start_pos_w_u, end_pos_w_u,
                            start_pos_v_u, end_pos_v_u,
                            start_pos_v_w, end_pos_v_w,
                            e2_e1_map);
                

                // %%%%%%%%%%%%%%%%%%%%%%%%%
                //Case B1 (\pi_3)
                // %%%%%%%%%%%%%%%%%%%%%%%%%                

                // /////////////////////////////////////////////////////////////////////
                // //B1 Dir D1: M3
                // 


                /////////////////////////////////////////////////////////////////////
                //B1 Dir D2: M2
                hashE2E1Key(t_graph,
                            start_pos_u_v, end_pos_u_v,
                            start_pos_w_v, end_pos_w_v,
                            start_pos_w_u, end_pos_w_u,
                            e2_e1_map);

                // /////////////////////////////////////////////////////////////////////
                // //B1 Dir D3: M1
                // triangle_motif_counts_[2][2] = countCaseB(t_graph,
                //             start_pos_u_v, end_pos_u_v,
                //             start_pos_w_v, end_pos_w_v,
                //             start_pos_u_w, end_pos_u_w,
                //             index_array[41], index_array[4], index_array[53], index_array[20]);
                // // cout << "case B1 D3 : M1 addition: " << triangle_motif_counts_[2][2] << endl;
                // motif_counts_[2][2] += triangle_motif_counts_[2][2];
             

                /////////////////////////////////////////////////////////////////////
                //B1 Dir D4: M4
                hashE3E1Key(t_graph,
                            start_pos_u_v, end_pos_u_v,
                            start_pos_v_w, end_pos_v_w,
                            start_pos_w_u, end_pos_w_u,
                            e3_e1_map);

                // /////////////////////////////////////////////////////////////////////
                // //B1 Dir D5: M7
                // triangle_motif_counts_[2][4] = countCaseB(t_graph,
                //             start_pos_v_u, end_pos_v_u,
                //             start_pos_v_w, end_pos_v_w,
                //             start_pos_u_w, end_pos_u_w,
                //             index_array[42], index_array[8], index_array[52], index_array[16]);
                // // cout << "case B1 D5 : M7 addition: " << triangle_motif_counts_[2][4] << endl;
                // motif_counts_[2][4] += triangle_motif_counts_[2][4];
             

                // /////////////////////////////////////////////////////////////////////
                // //B1 Dir D6: M6
                // triangle_motif_counts_[2][5] = countCaseB(t_graph,
                //             start_pos_v_u, end_pos_v_u,
                //             start_pos_w_v, end_pos_w_v,
                //             start_pos_w_u, end_pos_w_u,
                //             index_array[43], index_array[12], index_array[55], index_array[28]);
                // // cout << "case B1 D6 : M6 addition: " << triangle_motif_counts_[2][5] << endl;
                // motif_counts_[2][5] += triangle_motif_counts_[2][5];

                // /////////////////////////////////////////////////////////////////////
                // //B1 Dir D7: M8
                // triangle_motif_counts_[2][6] = countCaseB(t_graph,
                //             start_pos_v_u, end_pos_v_u,
                //             start_pos_w_v, end_pos_w_v,
                //             start_pos_u_w, end_pos_u_w,
                //             index_array[43], index_array[12], index_array[53], index_array[20]);
                // // cout << "case B1 D7 : M8 addition: " << triangle_motif_counts_[2][6] << endl;
                // motif_counts_[2][6] += triangle_motif_counts_[2][6];
             

                // /////////////////////////////////////////////////////////////////////
                // //B1 Dir D8: M5
                // triangle_motif_counts_[2][7] = countCaseB(t_graph,
                //             start_pos_v_u, end_pos_v_u,
                //             start_pos_v_w, end_pos_v_w,
                //             start_pos_w_u, end_pos_w_u,
                //             index_array[42], index_array[8], index_array[54], index_array[24]);
                // // cout << "case B1 D8 : M5 addition: " << triangle_motif_counts_[2][7] << endl;
                // motif_counts_[2][7] += triangle_motif_counts_[2][7];


                // %%%%%%%%%%%%%%%%%%%%%%%%%
                //Case B2 (\pi_4)
                // %%%%%%%%%%%%%%%%%%%%%%%%%                
                
                // /////////////////////////////////////////////////////////////////////
                // //B2 Dir D1: M1
                // triangle_motif_counts_[3][0] = countCaseB(t_graph,
                //             start_pos_u_w, end_pos_u_w,
                //             start_pos_v_w, end_pos_v_w,
                //             start_pos_u_v, end_pos_u_v,
                //             index_array[44], index_array[16], index_array[48], index_array[0]);
                // // cout << "case B2 D1 : M1 addition: " << triangle_motif_counts_[3][0] << endl;
                // motif_counts_[3][0] += triangle_motif_counts_[3][0];


                // /////////////////////////////////////////////////////////////////////
                // //B2 Dir D2: M7
                // triangle_motif_counts_[3][1] = countCaseB(t_graph,
                //             start_pos_w_u, end_pos_w_u,
                //             start_pos_w_v, end_pos_w_v,
                //             start_pos_u_v, end_pos_u_v,
                //             index_array[47], index_array[28], index_array[49], index_array[4]);
                // // cout << "case B2 D2 : M7 addition: " << triangle_motif_counts_[3][1] << endl;
                // motif_counts_[3][1] += triangle_motif_counts_[3][1];

                // /////////////////////////////////////////////////////////////////////
                // //B2 Dir D3: M3
                // triangle_motif_counts_[3][2] = countCaseB(t_graph,
                //             start_pos_u_w, end_pos_u_w,
                //             start_pos_w_v, end_pos_w_v,
                //             start_pos_u_v, end_pos_u_v,
                //             index_array[45], index_array[20], index_array[49], index_array[4]);
                // // cout << "case B2 D3 : M3 addition: " << triangle_motif_counts_[3][2] << endl;
                // motif_counts_[3][2] += triangle_motif_counts_[3][2];

                // /////////////////////////////////////////////////////////////////////
                // //B2 Dir D4: M8
                // triangle_motif_counts_[3][3] = countCaseB(t_graph,
                //             start_pos_w_u, end_pos_w_u,
                //             start_pos_v_w, end_pos_v_w,
                //             start_pos_u_v, end_pos_u_v,
                //             index_array[46], index_array[24], index_array[48], index_array[0]);
                // // cout << "case B2 D4 : M8 addition: " << triangle_motif_counts_[3][3] << endl;
                // motif_counts_[3][3] += triangle_motif_counts_[3][3];

                /////////////////////////////////////////////////////////////////////
                //B2 Dir D5: M2
                hashE2E1Key(t_graph,
                            start_pos_u_w, end_pos_u_w,
                            start_pos_v_w, end_pos_v_w,
                            start_pos_v_u, end_pos_v_u,
                            e2_e1_map);

                // /////////////////////////////////////////////////////////////////////
                // //B2 Dir D6: M5
                // triangle_motif_counts_[3][5] = countCaseB(t_graph,
                //             start_pos_w_u, end_pos_w_u,
                //             start_pos_w_v, end_pos_w_v,
                //             start_pos_v_u, end_pos_v_u,
                //             index_array[47], index_array[28], index_array[51], index_array[12]);
                // // cout << "case B2 D6 : M5 addition: " << triangle_motif_counts_[3][5] << endl;
                // motif_counts_[3][5] += triangle_motif_counts_[3][5];


                /////////////////////////////////////////////////////////////////////
                //B2 Dir D7: M4
                hashE3E1Key(t_graph,
                            start_pos_u_w, end_pos_u_w,
                            start_pos_w_v, end_pos_w_v,
                            start_pos_v_u, end_pos_v_u,
                            e3_e1_map);

                // /////////////////////////////////////////////////////////////////////
                // //B2 Dir D8: M6
                // triangle_motif_counts_[3][7] = countCaseB(t_graph,
                //             start_pos_w_u, end_pos_w_u,
                //             start_pos_v_w, end_pos_v_w,
                //             start_pos_v_u, end_pos_v_u,
                //             index_array[46], index_array[24], index_array[50], index_array[8]);
                // // cout << "case B2 D8 : M6 addition: " << triangle_motif_counts_[3][7] << endl;
                // motif_counts_[3][7] += triangle_motif_counts_[3][7];


                // %%%%%%%%%%%%%%%%%%%%%%%%%
                //Case C1 (\pi_5)
                // %%%%%%%%%%%%%%%%%%%%%%%%%                

                /////////////////////////////////////////////////////////////////////
                // //C1 Dir D1: M6
                // triangle_motif_counts_[4][0] = countCaseC(t_graph,
                //             start_pos_v_w, end_pos_v_w,
                //             start_pos_u_v, end_pos_u_v,
                //             start_pos_u_w, end_pos_u_w,
                //             index_array[17], index_array[16], index_array[0], index_array[32]);
                // // cout << "case C1 D1 : M6 addition: " << triangle_motif_counts_[4][0] << endl;
                // motif_counts_[4][0] += triangle_motif_counts_[4][0];

                // /////////////////////////////////////////////////////////////////////
                // //C1 Dir D2: M1
                // triangle_motif_counts_[4][1] = countCaseC(t_graph,
                //             start_pos_w_v, end_pos_w_v,
                //             start_pos_u_v, end_pos_u_v,
                //             start_pos_w_u, end_pos_w_u,
                //             index_array[29], index_array[28], index_array[4], index_array[33]);
                // // cout << "case C1 D2 : M1 addition: " << triangle_motif_counts_[4][1] << endl;
                // motif_counts_[4][1] += triangle_motif_counts_[4][1];

                /////////////////////////////////////////////////////////////////////
                //C1 Dir D3: M2
                hashE2E1Key(t_graph,
                            start_pos_w_v, end_pos_w_v,
                            start_pos_u_v, end_pos_u_v,
                            start_pos_u_w, end_pos_u_w,
                            e2_e1_map);

                // /////////////////////////////////////////////////////////////////////
                // //C1 Dir D4: M8
                // triangle_motif_counts_[4][3] = countCaseC(t_graph,
                //             start_pos_v_w, end_pos_v_w,
                //             start_pos_u_v, end_pos_u_v,
                //             start_pos_w_u, end_pos_w_u,
                //             index_array[25], index_array[24], index_array[0], index_array[32]);
                // // cout << "case C1 D4 : M8 addition: " << triangle_motif_counts_[4][3] << endl;
                // motif_counts_[4][3] += triangle_motif_counts_[4][3];

                // /////////////////////////////////////////////////////////////////////
                // //C1 Dir D5: M5
                // triangle_motif_counts_[4][4] = countCaseC(t_graph,
                //             start_pos_v_w, end_pos_v_w,
                //             start_pos_v_u, end_pos_v_u,
                //             start_pos_u_w, end_pos_u_w,
                //             index_array[17], index_array[16], index_array[8], index_array[34]);
                // // cout << "case C1 D5 : M5 addition: " << triangle_motif_counts_[4][4] << endl;
                // motif_counts_[4][4] += triangle_motif_counts_[4][4];

                // /////////////////////////////////////////////////////////////////////
                // //C1 Dir D6: M3
                // triangle_motif_counts_[4][5] = countCaseC(t_graph,
                //             start_pos_w_v, end_pos_w_v,
                //             start_pos_v_u, end_pos_v_u,
                //             start_pos_w_u, end_pos_w_u,
                //             index_array[29], index_array[28], index_array[12], index_array[35]);
                // // cout << "case C1 D6 : M3 addition: " << triangle_motif_counts_[4][5] << endl;
                // motif_counts_[4][5] += triangle_motif_counts_[4][5];
               

                /////////////////////////////////////////////////////////////////////
                //C1 Dir D7: M4
                hashE3E1Key(t_graph,
                            start_pos_w_v, end_pos_w_v,
                            start_pos_v_u, end_pos_v_u,
                            start_pos_u_w, end_pos_u_w,
                            e3_e1_map);

                // /////////////////////////////////////////////////////////////////////
                // //C1 Dir D8: M7
                // triangle_motif_counts_[4][7] = countCaseC(t_graph,
                //             start_pos_v_w, end_pos_v_w,
                //             start_pos_v_u, end_pos_v_u,
                //             start_pos_w_u, end_pos_w_u,
                //             index_array[25], index_array[24], index_array[8], index_array[34]);
                // // cout << "case C1 D8 : M7 addition: " << triangle_motif_counts_[4][7] << endl;
                // motif_counts_[4][7] += triangle_motif_counts_[4][7];



                // %%%%%%%%%%%%%%%%%%%%%%%%%
                //Case C2 (\pi_6)
                // %%%%%%%%%%%%%%%%%%%%%%%%%                

                /////////////////////////////////////////////////////////////////////
                //C2 Dir D1: M2
                hashE2E1Key(t_graph,
                            start_pos_v_w, end_pos_v_w,
                            start_pos_u_w, end_pos_u_w,
                            start_pos_u_v, end_pos_u_v,
                            e2_e1_map);

                // /////////////////////////////////////////////////////////////////////
                // //C2 Dir D2: M5
                // triangle_motif_counts_[5][1] = countCaseC(t_graph,
                //             start_pos_w_v, end_pos_w_v,
                //             start_pos_w_u, end_pos_w_u,
                //             start_pos_u_v, end_pos_u_v,
                //             index_array[5], index_array[4], index_array[28], index_array[39]);
                // // cout << "case C2 D2 : M5 addition: " << triangle_motif_counts_[5][1] << endl;
                // motif_counts_[5][1] += triangle_motif_counts_[5][1];

                // /////////////////////////////////////////////////////////////////////
                // //C2 Dir D3: M6
                // triangle_motif_counts_[5][2] = countCaseC(t_graph,
                //             start_pos_w_v, end_pos_w_v,
                //             start_pos_u_w, end_pos_u_w,
                //             start_pos_u_v, end_pos_u_v,
                //             index_array[5], index_array[4], index_array[20], index_array[37]);
                // // cout << "case C2 D3 : M6 addition: " << triangle_motif_counts_[5][2] << endl;
                // motif_counts_[5][2] += triangle_motif_counts_[5][2];

                /////////////////////////////////////////////////////////////////////
                //C2 Dir D4: M4
                hashE3E1Key(t_graph,
                            start_pos_v_w, end_pos_v_w,
                            start_pos_w_u, end_pos_w_u,
                            start_pos_u_v, end_pos_u_v,
                            e3_e1_map);

                // /////////////////////////////////////////////////////////////////////
                // //C2 Dir D5: M1
                // triangle_motif_counts_[5][4] = countCaseC(t_graph,
                //             start_pos_v_w, end_pos_v_w,
                //             start_pos_u_w, end_pos_u_w,
                //             start_pos_v_u, end_pos_v_u,
                //             index_array[9], index_array[8], index_array[16], index_array[36]);
                // // cout << "case C2 D5 : M1 addition: " << triangle_motif_counts_[5][4] << endl;
                // motif_counts_[5][4] += triangle_motif_counts_[5][4];


                // /////////////////////////////////////////////////////////////////////
                // //C2 Dir D6: M7
                // triangle_motif_counts_[5][5] = countCaseC(t_graph,
                //             start_pos_w_v, end_pos_w_v,
                //             start_pos_w_u, end_pos_w_u,
                //             start_pos_v_u, end_pos_v_u,
                //             index_array[13], index_array[12], index_array[28], index_array[39]);
                // // cout << "case C2 D6 : M7 addition: " << triangle_motif_counts_[5][5] << endl;
                // motif_counts_[5][5] += triangle_motif_counts_[5][5];

                // /////////////////////////////////////////////////////////////////////
                // //C2 Dir D7: M8
                // triangle_motif_counts_[5][6] = countCaseC(t_graph,
                //             start_pos_w_v, end_pos_w_v,
                //             start_pos_u_w, end_pos_u_w,
                //             start_pos_v_u, end_pos_v_u,
                //             index_array[13], index_array[12], index_array[20], index_array[37]);
                // // cout << "case C2 D7 : M8 addition: " << triangle_motif_counts_[5][6] << endl;
                // motif_counts_[5][6] += triangle_motif_counts_[5][6];

                // /////////////////////////////////////////////////////////////////////
                // //C2 Dir D8: M3
                // triangle_motif_counts_[5][7] = countCaseC(t_graph,
                //             start_pos_v_w, end_pos_v_w,
                //             start_pos_w_u, end_pos_w_u,
                //             start_pos_v_u, end_pos_v_u,
                //             index_array[9], index_array[8], index_array[24], index_array[38]);
                // // cout << "case C2 D8 : M3 addition: " << triangle_motif_counts_[5][7] << endl;
                // motif_counts_[5][7] += triangle_motif_counts_[5][7];


                // /////////////////////
                // // adjustemnts for D4
                // /////////////////////
                // for (VertexEdgeId t_pos = start_pos_u_v; t_pos < end_pos_u_v; t_pos++)
                // {
                //     VertexEdgeId equals_w_u = t_graph.edgeTimeIntervalCount(start_pos_w_u, end_pos_w_u, t_graph.times_[t_pos], t_graph.times_[t_pos]);
                //     if(equals_w_u > 0)
                //     {
                //         VertexEdgeId equals_v_w = t_graph.edgeTimeIntervalCount(start_pos_v_w, end_pos_v_w, t_graph.times_[t_pos], t_graph.times_[t_pos]);
                //         if(equals_v_w > 0)
                //         {
                //             cout << "somthing" << endl;
                //             motif_counts_[2][3] -= equals_w_u * equals_v_w;
                //             motif_counts_[3][3] -= equals_w_u * equals_v_w;
                //             motif_counts_[4][3] -= equals_w_u * equals_v_w;
                //             motif_counts_[5][3] -= equals_w_u * equals_v_w;
                //         }
                //     }
                // }


                // /////////////////////
                // // adjustemnts for D7
                // /////////////////////
                // for (VertexEdgeId t_pos = start_pos_v_u; t_pos < end_pos_v_u; t_pos++)
                // {
                //     VertexEdgeId equals_u_w = t_graph.edgeTimeIntervalCount(start_pos_u_w, end_pos_u_w, t_graph.times_[t_pos], t_graph.times_[t_pos]);
                    
                //     if(equals_u_w > 0)
                //     {
                //         VertexEdgeId equals_w_v = t_graph.edgeTimeIntervalCount(start_pos_w_v, end_pos_w_v, t_graph.times_[t_pos], t_graph.times_[t_pos]);
                //         if(equals_w_v > 0)
                //         {
                //             cout << "somthing" << endl;
                //             motif_counts_[2][6] -= equals_u_w * equals_w_v;
                //             motif_counts_[3][6] -= equals_u_w * equals_w_v;
                //             motif_counts_[4][6] -= equals_u_w * equals_w_v;
                //             motif_counts_[5][6] -= equals_u_w * equals_w_v;
                //         }
                //     }
                // }


            }
        }
    }

    // for debug
    // for (auto M4_it = e3_e1_map.begin(); M4_it != e3_e1_map.end(); M4_it++)
    // {
    //     VertexEdgeId e4 = M4_it->first;
    //     cout << "e4: " << e4 << " time= " << t_graph.times_[e4] << endl;
    //     for (auto M4_e2_it = e3_e1_map[e4].begin(); M4_e2_it != e3_e1_map[e4].end(); M4_e2_it++)
    //     {
    //         cout << M4_e2_it->first << ", " << M4_e2_it->second << endl;
    //     }
    // }

    // for (auto M2_it = e2_e1_map.begin(); M2_it != e2_e1_map.end(); M2_it++)
    // {
    //     VertexEdgeId e4 = M2_it->first;
    //     cout << "e4: " << e4 << " time= " << t_graph.times_[e4] << endl;
    //     for (auto M2_e2_it = e2_e1_map[e4].begin(); M2_e2_it != e2_e1_map[e4].end(); M2_e2_it++)
    //     {
    //         cout << M2_e2_it->first << ", " << M2_e2_it->second << endl;
    //     }
    // }

    
    // merge M4 and M2 hash map to get the chordal-4-cycle count
    for (auto M4_it = e3_e1_map.begin(); M4_it != e3_e1_map.end(); M4_it++)
    {
        VertexEdgeId e4 = M4_it->first;
        if (t_graph.times_[e4] == 1256694849){
            cout << "e4: " << e4 << endl;
        }
        // if M2 has e4 as key, cnt += multipliciy M4[e4] * multiplicity M2[e4] that meet time constraints
        if (e2_e1_map.find(e4) != e2_e1_map.end()) // M2, t1
        {
            Count cum_multi_M4 = 0; // M4's e4 cumulative multiplicity
            unordered_set<VertexEdgeId> e1src_set;
            unordered_map<VertexEdgeId, Count> shrinkage_e1_cnt; // shrinkage[e1] = # of t_graph.temporal_nbrs_[M4_e2_it->first].dst_ == e1.src
            for (map<tuple<TemporalTime, VertexEdgeId>, Count>::reverse_iterator M2_e1_it = e2_e1_map[e4].rbegin(); M2_e1_it != e2_e1_map[e4].rend(); M2_e1_it++)
            {
                tuple<TemporalTime, VertexEdgeId> t1_e1src = M2_e1_it->first;
                VertexEdgeId e1_src = get<1>(t1_e1src);
                e1src_set.insert(e1_src);
            }
            
            map<tuple<TemporalTime, VertexEdgeId>, Count>::reverse_iterator M4_e2_it = e3_e1_map[e4].rbegin();
            // M2_e1 iterate from largest timestamp to smallest timestamp
            for (map<tuple<TemporalTime, VertexEdgeId>, Count>::reverse_iterator M2_e1_it = e2_e1_map[e4].rbegin(); M2_e1_it != e2_e1_map[e4].rend(); M2_e1_it++)
            {
                tuple<TemporalTime, VertexEdgeId> t1_e1src = M2_e1_it->first;
                TemporalTime t1 = get<0>(t1_e1src);
                VertexEdgeId e1_src = get<1>(t1_e1src);
                // M4's t2 >= M2's t1
                while(M4_e2_it != e3_e1_map[e4].rend() && (get<0>(M4_e2_it->first) >= t1))
                {
                    // if (t_graph.temporal_nbrs_[M4_e2_it->first].dst_ != e1_src){ // make sure e2.dst != e1.src 
                    cum_multi_M4 += M4_e2_it->second;
                    // update shrinkage count
                    VertexEdgeId e2_dst = get<1>(M4_e2_it->first);
                    if (e1src_set.find(e2_dst) != e1src_set.end())
                    {
                        Util::unorderedmapAddCntAndCreatIfNotExist<VertexEdgeId, Count>(shrinkage_e1_cnt, e2_dst, M4_e2_it->second);
                    }
                    M4_e2_it++;
                }
                // if (M2_e1_it->second * (cum_multi_M4 - shrinkage_e1_cnt[e1_src]) > 0){
                //     cout << "t1: " << get<0>(M2_e1_it->first) << ", t4: " << t_graph.times_[e4] << endl;
                //     cout << "e1 multiplicity: " << M2_e1_it->second << ", cum_multi_M4: " << cum_multi_M4 << ", shrinkage_e1_cnt: " << shrinkage_e1_cnt[e1_src] << endl;
                // }
                cnt += M2_e1_it->second * (cum_multi_M4 - shrinkage_e1_cnt[e1_src]);
            }
        }
    }

    cout << "chordal-4-cycle cnt: " << cnt << endl;

    mult_graph.deleteGraph(); // free memory
}


void MotifCounter::printCounts()
{
    cout << endl;
    cout << "counts of temporal triangles for each motif type:" << endl;
    for(int i=0; i < 8; i++)
        cout << "type " << i+1 << ": " << motif_type_counts_[i] << endl;
}

void MotifCounter::printCountsFile(const char *path)
{   
    FILE* output_file = fopen(path, "w");
    for(int i=0; i < 8; i++)
    {
        fprintf(output_file, "%ld", motif_type_counts_[i]);
        fprintf(output_file, "\n");
    }
    fclose(output_file);
}

void MotifCounter::freeMemory()
{
    if (edge_count != nullptr)
        delete[] edge_count; 
    if (edge_count_cum != nullptr)
        delete[] edge_count_cum;
}




