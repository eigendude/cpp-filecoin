#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

addtest(filecoin_block_validator_test
    block_validator_test.cpp
    )
target_link_libraries(filecoin_block_validator_test
    filecoin_block_validator
    )
