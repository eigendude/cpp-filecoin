/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CPP_FILECOIN_BASE_LEVELDB_TEST_HPP
#define CPP_FILECOIN_BASE_LEVELDB_TEST_HPP

#include "testutil/storage/base_fs_test.hpp"

#include "filecoin/storage/leveldb/leveldb.hpp"

namespace test {

  struct BaseLevelDB_Test : public BaseFS_Test {
    using LevelDB = fc::storage::LevelDB;

    explicit BaseLevelDB_Test(const fs::path &path);

    void open();

    void SetUp() override;

    void TearDown() override;

    std::shared_ptr<LevelDB> db_;
  };

}  // namespace test

#endif  // CPP_FILECOIN_BASE_LEVELDB_TEST_HPP
