Repository structure
--------------------

*  `common/`: the hash function and bitmap data structure used by algorithms
*  `Algorithm/`: the implementation of our algorithm and baseline
*  `benchmark.h`: C++ header of some benchmarks about accuracy

Requirements
-------

- cmake
- g++



代码文件在`ExptRealWorldData\Algorithm`路径下

其中`Periodicsketch.h`, `cover-cover.h`, `cover-count-v2.h`分别对应`Periodic sketch`, `double cover-min sketch`, `cover-min sketch + count-min sketch`算法

在`ExptRealWorldData\benchmark.h`的`TopKError`函数中修改使用Hash函数的个数：

```
    void TopKError(double alpha){
        AbsVector FPIs = {
                new Ours<2>(100000),
                new Baseline(100000),
        };
```

要运行相应的算法时，将对应的.h的文件重命名为`Ours.h`，对项目make之后即可运行，其中`maccdc.pcap`是可以用于测试的MACCDC数据集：

```
$ cd ./ExptRealWorldData/
$ cmake .
$ make
$ ./Periodic maccdc.pcap
```

