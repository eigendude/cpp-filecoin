#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

addtest(filecoin_graphsync_acceptance_test
    graphsync_acceptance_test.cpp
    graphsync_acceptance_common.cpp
    )
target_link_libraries(filecoin_graphsync_acceptance_test
    filecoin_graphsync
    filecoin_ipfs_merkledag_service
    p2p::asio_scheduler
    )

addtest(filecoin_graphsync_extension_test
    extensions_test.cpp
    )
target_link_libraries(filecoin_graphsync_extension_test
    filecoin_graphsync
    )
