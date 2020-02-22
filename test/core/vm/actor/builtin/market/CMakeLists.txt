#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

addtest(filecoin_market_actor_test
    test.cpp
    )
target_link_libraries(filecoin_market_actor_test
    filecoin_ipfs_datastore_in_memory
    filecoin_market_actor
    filecoin_state_tree
    )
