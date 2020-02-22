#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

add_library(filecoin_proof_verifier
        impl/proof_verifier_impl.cpp
        )

target_link_libraries(filecoin_proof_verifier
        filecoin_outcome
        filecoin_sector
        filecoin_proofs
        )
