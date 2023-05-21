#include "ETTC/Graph.h"
#include "ETTC/t_triangle_counting.h"
#include "timer.h"
#include <string.h>


using namespace std;

int main(int argc, char *argv[])
{
    TemporalTime delta = stoi(argv[2]);  // delta_{1,3} in the paper
    TemporalTime delta1 = stoi(argv[3]); // delta_{1,2} in the paper
    TemporalTime delta2 = stoi(argv[4]); // delta_{2,3} in the paper

    bool benchmarking = false;
    if(argc > 6)
    {
        string arg = argv[6];
        if (arg == "-b")
        {
            benchmarking = true;
        }
    }
    const int NUM_TRIAL = 10;
    
    if(delta > delta1 + delta2)  // difference between first and third edge cannot be more than the sum
        delta = delta1 + delta2; // of difference between first and second edges and the difference between second and third edges

    TemporalGraph temporal_graph = loadTemporalGraph(argv[1]);  // read the input temporal graph

    cout << "Temporal graph loaded." << endl;
    
    Graph static_graph = temporal_graph.ExtractStaticGraph();   // extract the static graph

    cout << endl;
    cout << "Static graph extracted." << endl;

    CSRGraph static_csr_graph = static_graph.convertToCSR();   // convert the static graph into CSR format
    cout << endl;
    cout << "Static graph converted into CSR format." << endl;

    CSRTemporalGraph csr_temporal_graph = temporal_graph.convertToCSR();  // convert the input temporal graph into CSR format
    cout << endl;
    cout << "Temporal graph converted into CSR format." << endl;

    cout << endl;
    cout << "degeneracy ordering of vertices." << endl;
    static_csr_graph.findDegenOrdering();  // find the degeneracy ordering of the static graph (CSR format)

    cout << endl;
    cout << "Relabel vertices of the CSR temporal graph by degeneracy order of the static graph" << endl;
    csr_temporal_graph.relabelByDegenOrder(static_csr_graph.degen_order_, static_csr_graph.sort_by_degen_);  // relabel vertices of the temporal graph (CSR format) by the degeneracy ordering of the static graph

    cout << endl;
    cout << "Relabel vertices of the static CSR graph by degeneracy order" << endl;
    static_csr_graph.relabelByDegenOrder();   // relabel vertices of the static graph (CSR format) by the degenracy ordering of the static graph

    cout << endl;
    cout << "CSR static graph converted to CSRDAG" << endl;
    CSRDAG csr_dag(static_csr_graph);  // convert static graph (CSR format) to a DAG. In other words, we orient the static graph w.r.t. its degeneracy ordering.


    cout << endl;
    cout << "Degeneracy: " << csr_dag.out_edge_dag_.maxDegree() << endl; // max out-degree of the DAG oriented w.r.t. the degeneracy ordering
                                                                        //  is equal to the degeneracy of the graph

    // Counting temporal triangles for delta, delta_1, and delta_2
    MotifCounter motif_counter;
    motif_counter.countTemporalTriangle(csr_dag.out_edge_dag_, csr_temporal_graph, delta, delta1, delta2);

    cout << "\n///////////////// \nInfo\n/////////////////" << endl << endl;
    cout << "Number of vertices: " << csr_temporal_graph.num_vertices_ << endl;
    cout << "Number of edges: " << csr_temporal_graph.num_edges_ << endl;
    cout << "Number of static edges: " << csr_dag.out_edge_dag_.num_edges_ << endl;
    cout << "Number of static triangles: " << motif_counter.static_triangles_count_ << endl;
    cout << "Number of useless static triangles: " << motif_counter.useless_static_triangles_ << endl;
    cout << "Max multiplicity in one direction on a pair: " << motif_counter.highest_mult_ << endl;
    cout << "Avg multiplicity on a pair: " << csr_temporal_graph.num_edges_ / csr_dag.out_edge_dag_.num_edges_ << endl;
    // cout << "Time on useless static triangles: " << motif_counter.useless_time_ << endl;
    // cout << "Time on useful static triangles: " << motif_counter.useful_time_ << endl;
    csr_temporal_graph.printTimeSpan();
    cout << "Avg multiplicity of useless static triangles: " << (double) motif_counter.useless_mult_cnt_ / motif_counter.useless_static_triangles_ << endl;
    cout << "Avg multiplicity of useful static triangles: " << (double) motif_counter.useful_mult_cnt_ / (motif_counter.static_triangles_count_ - motif_counter.useless_static_triangles_) << endl;
    cout << "Number of useless static triangles skipped: " << motif_counter.skip_useless_cnt << endl;
    
    // const char s[2] = "/";
    // char* token = strtok(argv[1], s);
    // token = strtok(NULL, "/");
    // token = strtok(token, ".");
    // auto path = "statistics/" + string(token) + ".txt";
    // FILE* output_file = fopen(path.c_str(), "w");
    // for(auto x: motif_counter.temporal_static_cnt_)
    // {
    //     fprintf(output_file, "%ld %ld\n", x.first, x.second);
    // }
    // fclose(output_file);

    // motif_counter.printCounts();
    motif_counter.printCountsFile(argv[5]);

    // free memory
    static_graph.deleteGraph();
    // cout << "a1" << endl;
    temporal_graph.deleteGraph();
    // cout << "a2" << endl;
    static_csr_graph.deleteGraph();
    // cout << "a3" << endl;
    csr_temporal_graph.deleteGraph();
    // cout << "a4" << endl;

    motif_counter.freeMemory();
    // cout << "a5" << endl;

    if (benchmarking)
    {
        cout << "benchmarking" << endl;
        Timer t;
        double avg_time = 0;
        TemporalGraph temporal_graph = loadTemporalGraph(argv[1]);  // read the input temporal graph
        // Timer detailed_t[8];
        // double detailed_avg_time[8] = {0};
        Timer other_t;
        // double other_avg_time = 0;
        // other, enumerate static, findindex, populate edge count, and countCase
        double detailed_avg_time[5] = {0};
        t.Start();
        for(int trial = 0; trial < NUM_TRIAL; trial++)
        {
            other_t.Start();
            // detailed_t[0].Start();
            Graph static_graph = temporal_graph.ExtractStaticGraph();   // extract the static graph
            // detailed_t[0].Stop();
            // detailed_avg_time[0] += detailed_t[0].Millisecs();
            
            // detailed_t[1].Start();
            CSRGraph static_csr_graph = static_graph.convertToCSR();   // convert the static graph into CSR format
            // detailed_t[1].Stop();
            // detailed_avg_time[1] += detailed_t[1].Millisecs();

            // detailed_t[2].Start();
            CSRTemporalGraph csr_temporal_graph = temporal_graph.convertToCSR();  // convert the input temporal graph into CSR format
            // detailed_t[2].Stop();
            // detailed_avg_time[2] += detailed_t[2].Millisecs();

            // detailed_t[3].Start();
            static_csr_graph.findDegenOrdering();  // find the degeneracy ordering of the static graph (CSR format)
            // detailed_t[3].Stop();
            // detailed_avg_time[3] += detailed_t[3].Millisecs();

            // detailed_t[4].Start();
            csr_temporal_graph.relabelByDegenOrder(static_csr_graph.degen_order_, static_csr_graph.sort_by_degen_);  // relabel vertices of the temporal graph (CSR format) by the degeneracy ordering of the static graph
            // detailed_t[4].Stop();
            // detailed_avg_time[4] += detailed_t[4].Millisecs();
            
            // detailed_t[5].Start();
            static_csr_graph.relabelByDegenOrder();   // relabel vertices of the static graph (CSR format) by the degenracy ordering of the static graph
            // detailed_t[5].Stop();
            // detailed_avg_time[5] += detailed_t[5].Millisecs();
            
            // detailed_t[6].Start();
            CSRDAG csr_dag(static_csr_graph);
            // detailed_t[6].Stop();
            // detailed_avg_time[6] += detailed_t[6].Millisecs();
            other_t.Stop();
            detailed_avg_time[0] += other_t.Millisecs();

            // detailed_t[7].Start();
            MotifCounter motif_counter;
            motif_counter.countTemporalTriangle(csr_dag.out_edge_dag_, csr_temporal_graph, delta, delta1, delta2);
            // detailed_t[7].Stop();
            // detailed_avg_time[7] += detailed_t[7].Millisecs();
            for(int i = 0; i < 3 ; i++)
                detailed_avg_time[i+2] += motif_counter.times_[i]; 

            // free memory
            static_graph.deleteGraph();
            static_csr_graph.deleteGraph();
            csr_temporal_graph.deleteGraph();
            motif_counter.freeMemory();
        }
        // free memory
        temporal_graph.deleteGraph();

        t.Stop();
        avg_time += t.Millisecs();
        avg_time /= (double)NUM_TRIAL;
        cout << "Avg of " << NUM_TRIAL << " trials temporal triangle count (ms) is: " << avg_time << " ms." << endl;
        double sum_except_enumerate = 0;
        for(int i=0; i < 5; i++)
        {
            detailed_avg_time[i] /= (double)NUM_TRIAL;
            sum_except_enumerate += detailed_avg_time[i];
        }
        detailed_avg_time[1] = avg_time - sum_except_enumerate;
        cout << "Time spent on other, enumerate static, findindex, populate edge count, and countCase, and sum are: " << endl;
        for(int i=0; i < 5; i++)
        {
            cout << detailed_avg_time[i] << ", ";
        }
        cout << avg_time;
        cout << endl;

    }

    return 0;
 }

 