# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

addtest(filecoin_weight_calculator_test
    weight_calculator_test.cpp
    )
target_link_libraries(filecoin_weight_calculator_test
    filecoin_ipfs_datastore_in_memory
    filecoin_weight_calculator
    )
