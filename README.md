# gwt
Introduction
Graph-indexing wavelet tree (gWT) (1) is a software for graph similarity search and is applicable to massive graph databases. gWT represents graphs as a set of words called bag-of-words(2), and solves the graph similarity search problem as a semi-conjunctive query which searches for graphs matching to at least k query words. To solve the semi-conjunctive query efficiently, gWT uses a novel recursive search algorithm on wavelet tree (3). Unlike state-of-the-art graph search methods, it does not require a frequent subgraph mining to index graph databases. gWT is applicable to more than 20 million graphs.

Recently, gWT has been reimplemented on wavelet matrix (6), another interpretation of wavelet tree. Thus, the source codes of the current version are much simpler than those of the previous versions.

Quick Start
To compile gWT, please type the following: tar -xzvf gwt-X.X.X.tar.bz2 cd gwm-X.X.X/src make To create an index-file from a graph database, the gwt-build command can be used as follows: ./gwt-build -iteration 2 ../dat/mutagen.gsp index where mutagen.gsp is a graph database file and index is an output file for an indexed graph database.

To search for graphs similar to a query from the indexed graph database by using the gwt-build command, the gwt-search command can be used as follows: ./gwt-search -kthreshold 0.8 index ../dat/mutagen_query.gsp where -kthreshold indicates a similarity threshold and mutagen_query.gsp is a query file.

Usage
Usage: gwt-build [OPTION]... DATABASEFILE INDEXFILE

where [OPTION]...  is a list of zero or more optional arguments
      DATABASEFILE  is the name of a graph database
      INDEXFILE     is the name of an indexfile
Additional arguments (input and output files may be specified): -iteration [# of iteration]: the number of WLKernel computations (default: 2)

Usage: gwt-search [OPTION]... INDEXFILE QUERYFILE
   
where [OPTION]...  is a list of zero or more optional arguments
      INDEXFILE    is the name of an index file
      OUTFILE      is the name of a query graph file 
Additional arguments (input and output files may be specified): -kthreshold [float value]: similarity threshold (default: 0.8) 

Format of input file
The file format of input files is the same as that of gSpan (5). Example files are included in ./gwt-X.X.X/dat/. "t # N" means the Nth graph, "v M L" means that the Mth vertex in this graph has label L, "e P Q L" means that there is an edge connecting the Pth vertex with the Qth vertex. 
The edge has label L. M, N, P, Q, and L are integers.

Format of output file
"id:N M1:S1 M2:S2 M3:S2..." means there are graphs M1, M2, M3... in your graph database similar to the Nth graph in your query file. S1,S2,S3... are similarities.

Reference
(1) Y.Tabei and K.Tsuda: Kernel-based Similarity Search in Massive Graph Databases with Wavelet Trees, Eleventh SIAM International Conference on Data Mining (SDM), 2011. Link to the PDF
(2) N.Shervashidze and K. M. Borgwardt : Fast subtree kernels on graphs. In Adavances in Neural Information Processing Systems (NIPS), 2010.
(3) R.Grossi, A.Gupta, and J.Vitter : High-order entropy-compressed text indexes. In Proceedings of the 14th Annual ACM-SIAM Symposium on Discrete Algorithms (SODA), pages 635-645, 2003.
(4) Link to wat-array, c++ library of wavelet tree
(5) http://www.cs.ucsb.edu/~xyan/software/gSpan.htm
(6) F. Claude and G. Navarro: The Wavelet Matrix, SPIRE 2012.

Licence: GNU General Public License


