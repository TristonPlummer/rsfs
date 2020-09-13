# RuneScape Cache Library
A C++ library for reading from the RuneScape filesystem.

## Usage
### Getting a file from an index.
```c++
auto id = 4151;
rsfs::RSFileSystem fs("./data/js5/");
auto& index = fs.getIndex(19);
auto data = index.data(id >> 8u, id & 0xFFu);
```