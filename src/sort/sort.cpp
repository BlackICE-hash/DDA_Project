#include "dls/sort.hpp"

namespace dls {

// Explicit instantiations to ensure coverage in static library dls_core
template void mergeSort<int>(std::vector<int>&, Metrics&, DefaultLess<int>);
template void mergeSort<Document, DocumentScoreDesc>(std::vector<Document>&, Metrics&, DocumentScoreDesc);
template void mergeSort<Document, DocumentIdAsc>(std::vector<Document>&, Metrics&, DocumentIdAsc);

template void quickSort<int>(std::vector<int>&, Metrics&, DefaultLess<int>);
template void quickSort<Document, DocumentScoreDesc>(std::vector<Document>&, Metrics&, DocumentScoreDesc);
template void quickSort<Document, DocumentIdAsc>(std::vector<Document>&, Metrics&, DocumentIdAsc);

template void quickSortEx<int>(std::vector<int>&, Metrics&, PivotStrategy, DefaultLess<int>);
template void quickSortEx<Document, DocumentScoreDesc>(std::vector<Document>&, Metrics&, PivotStrategy, DocumentScoreDesc);
template void quickSortEx<Document, DocumentIdAsc>(std::vector<Document>&, Metrics&, PivotStrategy, DocumentIdAsc);

} // namespace dls
