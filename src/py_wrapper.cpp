#include <python3.10/pyconfig.h>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "wordle.hpp"

// define boost.python module
BOOST_PYTHON_MODULE(pywordle)
{
    using namespace boost::python;

    //def("testAll", &testAllMultithread);
    class_<WordleSolver>("WordleSolver")
        .def("remainingWords", &WordleSolver::remainingWords)
        .def("remainingWordList", &WordleSolver::remainingWordList)
        .def("sampleWords", &WordleSolver::sampleWords)
        .def("reset", &WordleSolver::reset)
        .def("setTarget", &WordleSolver::setTarget)
        .def("makeGuess", &WordleSolver::makeGuess)
        .def("wordsWithPos", &WordleSolver::wordsWithPos)
        .def("wordsWithFreq", &WordleSolver::wordsWithFreq)
        .def("generateCrossTable", &WordleSolver::generateCrossTable)
    ;

    class_<std::vector<int> >("IntVec")
        .def(vector_indexing_suite<std::vector<int> >())
        .def("size", &vector_indexing_suite<std::vector<int> >::size)
    ;

    class_<std::vector<vector<int>> >("IntVecVec")
        .def(vector_indexing_suite<std::vector<vector<int>> >())
        .def("size", &vector_indexing_suite<std::vector<vector<int>> >::size)
    ;
}