#include "includes/define_python.hpp"
#include "includes/matrix_market_interface.hpp"
#include "includes/ublas_interface.hpp"
#include "includes/ublas_complex_interface.hpp"
#include "python/add_matrix_market_interface_to_python.hpp"

namespace Quest::Python{

    void AddMatrixMarketInterfaceToPython(pybind11::module& m){
        namespace py = pybind11;

        m.def("ReadMatrixMarketMatrix", ReadMatrixMarketMatrix <Quest::CompressedMatrix>);
        m.def("ReadMatrixMarketMatrix", ReadMatrixMarketMatrix <Quest::ComplexCompressedMatrix>);
        m.def("WriteMatrixMarketMatrix", WriteMatrixMarketMatrix <Quest::CompressedMatrix>);
        m.def("WriteMatrixMarketMatrix", WriteMatrixMarketMatrix <Quest::ComplexCompressedMatrix>);

        m.def("ReadMatrixMarketVector", ReadMatrixMarketVector <Quest::Vector>);
        m.def("ReadMatrixMarketVector", ReadMatrixMarketVector <Quest::ComplexVector>);
        m.def("WriteMatrixMarketVector", WriteMatrixMarketVector <Quest::Vector>);
        m.def("WriteMatrixMarketVector", WriteMatrixMarketVector <Quest::ComplexVector>);

    }

}