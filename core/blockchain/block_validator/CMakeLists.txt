#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

add_library(filecoin_block_validator
    impl/block_validator_impl.cpp
    impl/syntax_rules.cpp
    impl/consensus_rules.cpp
    )
target_link_libraries(filecoin_block_validator
    filecoin_bls_provider
    filecoin_outcome
    filecoin_buffer
    filecoin_address
    filecoin_clock
    filecoin_interpreter
    filecoin_tipset
    filecoin_power_table
    )
