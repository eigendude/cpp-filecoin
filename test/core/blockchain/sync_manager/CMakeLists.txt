# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

addtest(filecoin_sync_target_bucket_test
    sync_target_bucket_test.cpp
    )
target_link_libraries(filecoin_sync_target_bucket_test
    filecoin_sync_target_bucket
    )

addtest(filecoin_sync_bucket_set_test
    sync_bucket_set_test.cpp
    )
target_link_libraries(filecoin_sync_bucket_set_test
    filecoin_sync_bucket_set
    )

addtest(filecoin_sync_manager_test
    sync_manager_test.cpp
    )
target_link_libraries(filecoin_sync_manager_test
    filecoin_sync_manager
    )
