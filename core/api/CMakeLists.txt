#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

add_library(filecoin_api
    make.cpp
    )
target_link_libraries(filecoin_api
    filecoin_address
    filecoin_chain_store
    filecoin_cid
    filecoin_interpreter
    filecoin_message
    filecoin_state_tree
    )

add_library(filecoin_rpc
    rpc/json_errors.cpp
    rpc/make.cpp
    rpc/ws.cpp
    )
target_link_libraries(filecoin_rpc
    filecoin_api
    filecoin_tipset
    )
