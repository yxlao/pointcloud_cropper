include(ExternalProject)

ExternalProject_Add(
    ext_tbb
    PREFIX tbb
    URL https://github.com/wjakob/tbb/archive/141b0e310e1fb552bdca887542c9c1a8544d6503.zip # Sept 2020
    URL_HASH SHA256=503e818765747b8228689dafcf9b54f1f81b4709e756e47d7e179b6d03566a5e
    DOWNLOAD_DIR "${THIRD_PARTY_DOWNLOAD_DIR}/tbb"
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DTBB_BUILD_TBBMALLOC=ON
        -DTBB_BUILD_TBBMALLOC_PROXYC=OFF
        -DTBB_BUILD_SHARED=OFF
        -DTBB_BUILD_STATIC=ON
        -DTBB_BUILD_TESTS=OFF
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
)

ExternalProject_Get_property(ext_tbb INSTALL_DIR)
set(TBB_INCLUDE_DIR "${INSTALL_DIR}/include")
set(TBB_LIB_DIR "${INSTALL_DIR}/lib")
set(TBB_LIBRARIES tbb_static tbbmalloc_static)
