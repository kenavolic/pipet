set (TARGET_NAME ${PIPET_LIB}_test)

set (PIPET_TST
    filter_test.cpp
    pipet_test.cpp
    reflect_test.cpp
    typelist_test.cpp
    utils_test.cpp
)

set (PIPET_EXTRA_TST
    bit_test.cpp
    cxstring_test.cpp
    random_test.cpp
)

if (PIPET_INCLUDE_EXTRA)
    set (PIPET_TST ${PIPET_TST} ${PIPET_EXTRA_TST})
endif()

set (PIPET_TST_UTILS
    test_common.h
)

create_test_sourcelist( 
    ${TARGET_NAME}
    pipet_test_driver.cpp
    ${PIPET_TST}
)

add_executable(${TARGET_NAME} pipet_test_driver.cpp ${PIPET_TST} ${PIPET_TST_UTILS})
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "tests")
target_link_libraries(${TARGET_NAME} ${PIPET_LIB} gtest gtest_main)
target_compile_features(${TARGET_NAME} PUBLIC cxx_std_17)

foreach(TST ${PIPET_TST})
    get_filename_component(TNAME ${TST} NAME_WE)
    add_test(NAME ${TNAME} COMMAND ${TARGET_NAME} ${TNAME})
endforeach()