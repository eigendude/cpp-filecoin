#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

add_library(piece
        piece.cpp
        piece_error.cpp
        )

target_link_libraries(piece
        cid
        outcome
        )
