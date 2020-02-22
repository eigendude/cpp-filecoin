#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

add_subdirectory(block_validator)
add_subdirectory(message_pool)
add_subdirectory(production)

add_library(filecoin_weight_calculator
    impl/weight_calculator_impl.cpp
    )
target_link_libraries(filecoin_weight_calculator
    filecoin_state_tree
    filecoin_tipset
    )

add_library(filecoin_sync_manager
    impl/sync_manager_impl.cpp
    )
target_link_libraries(filecoin_sync_manager
    filecoin_block
    Boost::boost
    filecoin_cid
    filecoin_syncer_state
    filecoin_sync_bucket_set
    filecoin_sync_target_bucket
    filecoin_tipset
    )

add_library(filecoin_syncer_state
    syncer_state.cpp
    )
target_link_libraries(filecoin_syncer_state
    filecoin_clock
    filecoin_tipset
    )

add_library(filecoin_sync_target_bucket
    impl/sync_target_bucket.cpp
    )
target_link_libraries(filecoin_sync_target_bucket
    Boost::boost
    filecoin_tipset
    )

add_library(filecoin_sync_bucket_set
    impl/sync_bucket_set.cpp
    )
target_link_libraries(filecoin_sync_bucket_set
    filecoin_sync_target_bucket
    )
