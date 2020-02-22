#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

add_library(filecoin_market_actor
    actor.cpp
    )
target_link_libraries(filecoin_market_actor
    filecoin_actor
    filecoin_array
    filecoin_balance_table
    filecoin_piece
    )
