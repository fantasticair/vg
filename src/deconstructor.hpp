#ifndef VG_DECONSTRUCTOR_HPP_INCLUDED
#define VG_DECONSTRUCTOR_HPP_INCLUDED
#include <vector>
#include <string>
#include <ostream>
#include <sstream>
#include "genotypekit.hpp"
#include "Variant.h"
#include "handle.hpp"
#include "genotypekit.hpp"
#include "traversal_finder.hpp"

/** \file
* Deconstruct is getting rewritten.
* New functionality:
* -Detect superbubbles and bubbles
* -Fix command line interface.
* -harmonize on XG / raw graph (i.e. deprecate index)
* -Use unroll/DAGify if needed to avoid cycles

** Much of this is taken from Brankovic's
** "Linear-Time Superbubble Identification Algorithm for Genome Assembly"
*/
namespace vg{
using namespace std;

class Deconstructor{
public:

    Deconstructor();
    ~Deconstructor();

    // deconstruct the entire graph to cout
    void deconstruct(vector<string> refpaths, const PathPositionHandleGraph* grpah, SnarlManager* snarl_manager,
                     bool path_restricted_traversals, int ploidy, bool include_nested,
                     const unordered_map<string, string>* path_to_sample = nullptr); 
    
private:

    // write a vcf record for the given site.  returns true if a record was written
    // (need to have a path going through the site)
    bool deconstruct_site(const Snarl* site);

    // convert traversals to strings.  returns mapping of traversal (offset in travs) to allele
    vector<int> get_alleles(vcflib::Variant& v, const vector<SnarlTraversal>& travs, int ref_path_idx,
                            char prev_char, bool use_start);

    // write traversal path names as genotypes
    void get_genotypes(vcflib::Variant& v, const vector<string>& names, const vector<int>& trav_to_allele);

    // given a set of traversals associated with a particular sample, select a set of size <ploidy> for the VCF
    // the highest-frequency ALT traversal is chosen
    // the bool returned is true if multiple traversals map to different alleles, more than ploidy.
    pair<vector<int>, bool> choose_traversals(const vector<int>& travs, const vector<int>& trav_to_allele,
                                              const vector<string>& trav_to_name);


    // check to see if a snarl is too big to exhaustively traverse
    bool check_max_nodes(const Snarl* snarl);

    // get traversals from the exhaustive finder.  if they have nested visits, fill them in (exhaustively)
    // with node visits
    vector<SnarlTraversal> explicit_exhaustive_traversals(const Snarl* snarl);
    
    // output vcf object
    vcflib::VariantCallFile outvcf;
    
    // toggle between exhaustive and path restricted traversal finder
    bool path_restricted = false;

    // the max ploidy we expect.
    int ploidy;

    // the graph
    const PathPositionHandleGraph* graph;

    // the snarl manager
    SnarlManager* snarl_manager;

    // the traversal finders. we always use a path traversal finder to get the reference path
    unique_ptr<PathTraversalFinder> path_trav_finder;
    // we optionally use another (exhaustive for now) traversal finder if we don't want to rely on paths
    unique_ptr<TraversalFinder> trav_finder;

    // the ref paths
    set<string> ref_paths;

    // keep track of the non-ref paths as they will be our samples
    set<string> sample_names;

    // map the path name to the sample in the vcf
    const unordered_map<string, string>* path_to_sample;

    // upper limit of degree-2+ nodes for exhaustive traversal
    int max_nodes_for_exhaustive = 100;    
};

}
#endif
