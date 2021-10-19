/* File: da.h */
class da {
public:
  da(int size);
  da(int size, int value);
  ~da();
  int get_size();
  void fill(int value);
  void printarray();
  int size;
  int* elements;
};
