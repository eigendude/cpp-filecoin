#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

add_custom_target(filecoin_adt
    DEPENDS
    filecoin_address_key
    filecoin_array
    filecoin_balance_table
    filecoin_map
    filecoin_multimap
    filecoin_uvarint_key
    )

add_library(filecoin_array INTERFACE)
target_link_libraries(filecoin_array INTERFACE
    filecoin_amt
    )

add_library(filecoin_map INTERFACE)
target_link_libraries(filecoin_map INTERFACE
    filecoin_hamt
    )

add_library(filecoin_balance_table_hamt
    impl/balance_table_hamt.cpp
    )
target_link_libraries(filecoin_balance_table_hamt
    filecoin_address_key
    filecoin_hamt
    )

add_library(filecoin_multimap
    impl/multimap.cpp
    )
target_link_libraries(filecoin_multimap
    filecoin_amt
    filecoin_hamt
    )

add_library(filecoin_uvarint_key
    uvarint_key.cpp
    )
target_link_libraries(filecoin_uvarint_key
    p2p::p2p_uvarint
    )

add_library(filecoin_address_key
    address_key.cpp
    )
target_link_libraries(filecoin_address_key
    filecoin_address
    )

add_library(filecoin_balance_table
    balance_table.cpp
    )
target_link_libraries(filecoin_balance_table
    filecoin_address_key
    filecoin_map
    )
