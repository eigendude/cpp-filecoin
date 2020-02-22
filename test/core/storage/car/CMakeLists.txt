#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

addtest(filecoin_car_test
    car_test.cpp
    )
target_link_libraries(filecoin_car_test
    filecoin_car
    filecoin_ipfs_datastore_in_memory
    )
target_include_directories(filecoin_car_test PUBLIC
  "${CMAKE_CURRENT_BINARY_DIR}"
  )

configure_file(car_test.hpp.in car_test.hpp @ONLY)
