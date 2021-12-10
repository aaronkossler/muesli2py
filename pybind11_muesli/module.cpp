#include <pybind11/pybind11.h>
#include "include/muesli.h"
#include "include/dm.h"
#include "include/da.h"

namespace py = pybind11;

PYBIND11_MODULE(muesli, muesli_handle) {
    bind_muesli(muesli_handle);
    bind_da(muesli_handle);
    bind_dm(muesli_handle);
}