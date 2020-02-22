#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

add_library(filecoin_proofs
        proofs.cpp
        proofs_error.cpp
        )

target_link_libraries(filecoin_proofs
        filecoin_ffi
        filecoin_outcome
        filecoin_blob
        filecoin_logger
        filecoin_comm_cid
        filecoin_piece
        filecoin_address
        filecoin_sector
        )
