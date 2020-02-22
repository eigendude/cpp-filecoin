#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

filecoin_add_library(filecoin_bls_provider
    impl/bls_provider_impl.cpp
    libbls_signatures.c
    )

target_link_libraries(filecoin_bls_provider
    filecoin_outcome
    )
