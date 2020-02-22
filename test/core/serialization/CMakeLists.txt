#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

addtest(filecoin_serialization_vectors_test
    serialization_vectors_test.cpp
    )
target_link_libraries(filecoin_serialization_vectors_test
    filecoin_rpc
    )
target_include_directories(filecoin_serialization_vectors_test PUBLIC
  "${CMAKE_CURRENT_BINARY_DIR}"
  )

configure_file(serialization_vectors.hpp.in serialization_vectors.hpp @ONLY)
