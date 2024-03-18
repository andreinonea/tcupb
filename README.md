# TradeClassificationAlgorithms++

C++ port of [Trade-Classification-Algorithms](https://github.com/jktis/Trade-Classification-Algorithms) Python implementation.

Developed for the Polytehnic University of Bucharest.

## Dependencies

* [fast-cpp-csv-parser](https://github.com/ben-strasser/fast-cpp-csv-parser)
* `DateTime` from [DataFrame](https://github.com/hosseinmoein/DataFrame)

## Building

```
git clone https://github.com/andreinonea/tcupb && cd tcupb
mkdir build && cd build
cmake ..
cmake --build .
```

CMake will search for `DataFrame` in the system first, and download and build from Git if `find_package` fails.
If you have a sysroot prepared, you may point CMake to it.

```
cmake -DCMAKE_PREFIX_PATH=/path/to/sysroot ..
```

## Authors

* Andrei N. Onea <andrei@onea.ro>
* Alexandru-Vasile Mireuță <TODO>
