#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

addtest(filecoin_api_json_test
    json_test.cpp
    )
target_link_libraries(filecoin_api_json_test
    filecoin_rpc
    )
