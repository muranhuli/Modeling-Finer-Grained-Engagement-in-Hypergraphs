# general info
deg_nbr_kcore_hcore.cpp:求超图的边度数、邻居节点个数、kcore、hcore  
kh-core.cpp:完成超图的(k,h)的动态插入删除算法，主要思想是对k利用已有的插入算法，对h利用coredecomp算法  
timeOfInsertErase.cpp:超图的动态插入删除所需要的时间，按照每条边的长度来进行插入删除，并记录时间  
computkhcore.cpp:计算超图的(k,h)的个数，对每一种(k,h)有多少个点是，即计算(k,h)在超图中的数量
construct_kh_data:将超图转换为普通图，一个为k-core图，一个为(k,h_max)-core图