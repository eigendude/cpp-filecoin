/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CPP_FILECOIN_CORE_VM_ACTOR_BUILTIN_MINER_ACTOR_MINER_ACTOR_HPP
#define CPP_FILECOIN_CORE_VM_ACTOR_BUILTIN_MINER_ACTOR_MINER_ACTOR_HPP

#include <libp2p/multi/uvarint.hpp>

#include "primitives/address/address_codec.hpp"
#include "primitives/big_int.hpp"
#include "primitives/rle_bitset/rle_bitset.hpp"

namespace fc::vm::actor::builtin::miner {
  using libp2p::multi::UVarint;
  using primitives::BigInt;
  using primitives::RleBitset;
  using primitives::address::Address;

  struct SectorPreCommitInfo {
    uint64_t sector;
    common::Blob<32> comm_r;
    uint64_t seal_epoch;
    std::vector<uint64_t> deal_ids;
  };

  struct PreCommittedSector {
    SectorPreCommitInfo info;
    uint64_t received_epoch;
  };

  struct MinerActorState {
    std::map<uint64_t, PreCommittedSector> precommitted_sectors;
    CID sectors;
    CID proving_set;
    CID info;
    RleBitset fault_set;
    uint64_t last_fault_sumbission;
    BigInt power;
    bool active;
    uint64_t slashed_at;
    uint64_t election_period_start;
  };

  struct MinerInfo {
    Address owner;
    Address worker;
    std::string peer_id;
    uint64_t sector_size;
  };

  template <class Stream,
            typename = std::enable_if_t<
                std::remove_reference_t<Stream>::is_cbor_encoder_stream>>
  Stream &operator<<(Stream &&s, const SectorPreCommitInfo &info) {
    return s << (s.list() << info.sector << info.comm_r << info.seal_epoch
                          << info.deal_ids);
  }

  template <class Stream,
            typename = std::enable_if_t<
                std::remove_reference_t<Stream>::is_cbor_decoder_stream>>
  Stream &operator>>(Stream &&s, SectorPreCommitInfo &info) {
    s.list() >> info.sector >> info.comm_r >> info.seal_epoch >> info.deal_ids;
    return s;
  }

  template <class Stream,
            typename = std::enable_if_t<
                std::remove_reference_t<Stream>::is_cbor_encoder_stream>>
  Stream &operator<<(Stream &&s, const PreCommittedSector &sector) {
    return s << (s.list() << sector.info << sector.received_epoch);
  }

  template <class Stream,
            typename = std::enable_if_t<
                std::remove_reference_t<Stream>::is_cbor_decoder_stream>>
  Stream &operator>>(Stream &&s, PreCommittedSector &sector) {
    s.list() >> sector.info >> sector.received_epoch;
    return s;
  }

  template <class Stream,
            typename = std::enable_if_t<
                std::remove_reference_t<Stream>::is_cbor_encoder_stream>>
  Stream &operator<<(Stream &&s, const MinerActorState &state) {
    std::map<std::string, PreCommittedSector> precommitted_sectors;
    for (auto &[sector, value] : state.precommitted_sectors) {
      UVarint sector_uvarint{sector};
      auto key = sector_uvarint.toBytes();
      precommitted_sectors[std::string(
          reinterpret_cast<const char *>(key.data()), key.size())] = value;
    }
    return s << (s.list() << precommitted_sectors << state.sectors
                          << state.proving_set << state.info << state.fault_set
                          << state.last_fault_sumbission << state.power
                          << state.active << state.slashed_at
                          << state.election_period_start);
  }

  template <class Stream,
            typename = std::enable_if_t<
                std::remove_reference_t<Stream>::is_cbor_decoder_stream>>
  Stream &operator>>(Stream &&s, MinerActorState &state) {
    std::map<std::string, PreCommittedSector> precommitted_sectors;
    s.list() >> precommitted_sectors >> state.sectors >> state.proving_set
        >> state.info >> state.fault_set >> state.last_fault_sumbission
        >> state.power >> state.active >> state.slashed_at
        >> state.election_period_start;
    state.precommitted_sectors.clear();
    for (auto &[key, value] : precommitted_sectors) {
      auto sector = value.info.sector;
      // TODO: check `key == uvarint(sector).bytes`
      state.precommitted_sectors[sector] = std::move(value);
    }
    return s;
  }

  template <class Stream,
            typename = std::enable_if_t<
                std::remove_reference_t<Stream>::is_cbor_encoder_stream>>
  Stream &operator<<(Stream &&s, const MinerInfo &info) {
    return s << (s.list() << info.owner << info.worker << info.peer_id
                          << info.sector_size);
  }

  template <class Stream,
            typename = std::enable_if_t<
                std::remove_reference_t<Stream>::is_cbor_decoder_stream>>
  Stream &operator>>(Stream &&s, MinerInfo &info) {
    s.list() >> info.owner >> info.worker >> info.peer_id >> info.sector_size;
    return s;
  }
}  // namespace fc::vm::actor::builtin::miner

#endif  // CPP_FILECOIN_CORE_VM_ACTOR_BUILTIN_MINER_ACTOR_MINER_ACTOR_HPP