#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

addtest(filecoin_rle_plus_codec_test
    rle_plus_codec_test.cpp
    )

target_link_libraries(filecoin_rle_plus_codec_test
    filecoin_rle_plus_codec
    )
