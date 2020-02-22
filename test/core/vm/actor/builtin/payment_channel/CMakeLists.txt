#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

addtest(filecoin_payment_channel_actor_test
    payment_channel_actor_test.cpp
    )
target_link_libraries(filecoin_payment_channel_actor_test
    filecoin_ipfs_datastore_in_memory
    filecoin_payment_channel_actor
    filecoin_state_tree
    )
