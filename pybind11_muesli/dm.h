#include <type_traits>
#include <pybind11/functional.h>

template <typename T>
class DM
{
public:

    // Constructors
    DM();
    DM(int col, int row);
    DM(int col, int row, const T& initial_value);

    // Destructor
    ~DM();

    // Functions
    int getSize() const;
    void fill(const T& value);
    void printmatrix();
    void map(const std::function<T(T)> &f);

    // Attributes
    // Number of elements
    int n;
    // Number of cols
    int ncol;
    // Number of rows
    int nrow;
    // Pointer to array of elements
    int* elements;
};