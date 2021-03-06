/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CPP_FILECOIN_CORE_API_RPC_JSON_HPP
#define CPP_FILECOIN_CORE_API_RPC_JSON_HPP

#include <rapidjson/document.h>
#include <cppcodec/base64_rfc4648.hpp>

#include "api/api.hpp"
#include "api/rpc/json_errors.hpp"
#include "common/enum.hpp"
#include "primitives/address/address_codec.hpp"

#define COMMA ,

#define ENCODE(type) Value encode(const type &v)

#define DECODE(type) static void decode(type &v, const Value &j)

namespace fc::api {
  using common::Blob;
  using crypto::signature::BlsSignature;
  using crypto::signature::Secp256k1Signature;
  using crypto::signature::Signature;
  using primitives::BigInt;
  using primitives::block::BlockHeader;
  using primitives::sector::PoStProof;
  using primitives::ticket::EPostProof;
  using primitives::ticket::EPostTicket;
  using primitives::ticket::Ticket;
  using primitives::tipset::HeadChangeType;
  using rapidjson::Document;
  using rapidjson::Value;
  using vm::actor::builtin::miner::SectorPreCommitInfo;
  using vm::actor::builtin::payment_channel::Merge;
  using vm::actor::builtin::payment_channel::ModularVerificationParameter;
  using base64 = cppcodec::base64_rfc4648;
  using SignatureType = crypto::signature::Type;
  using primitives::sector::RegisteredProof;

  struct Request {
    uint64_t id;
    std::string method;
    Document params;
  };

  struct Response {
    struct Error {
      int64_t code;
      std::string message;
    };

    boost::optional<uint64_t> id;
    boost::variant<Error, Document> result;
  };

  struct Codec {
    rapidjson::MemoryPoolAllocator<> &allocator;

    template <typename T>
    static void decodeEnum(T &v, const Value &j) {
      v = T{decode<std::underlying_type_t<T>>(j)};
    }

    static std::string AsString(const Value &j) {
      if (!j.IsString()) {
        outcome::raise(JsonError::WRONG_TYPE);
      }
      return {j.GetString(), j.GetStringLength()};
    }

    static const Value &Get(const Value &j, const char *key) {
      if (!j.IsObject()) {
        outcome::raise(JsonError::WRONG_TYPE);
      }
      auto it = j.FindMember(key);
      if (it == j.MemberEnd()) {
        outcome::raise(JsonError::OUT_OF_RANGE);
      }
      return it->value;
    }

    void Set(Value &j, std::string_view key, Value &&value) {
      j.AddMember(encode(key), value, allocator);
    }

    template <typename T>
    void Set(Value &j, std::string_view key, const T &v) {
      Set(j, key, encode(v));
    }

    static std::vector<uint8_t> decodeBase64(const Value &j) {
      if (j.IsNull()) {
        return {};
      }
      return base64::decode(AsString(j));
    }

    static auto AsDocument(const Value &j) {
      Document document;
      static_cast<Value &>(document) = Value{j, document.GetAllocator()};
      return document;
    }

    ENCODE(Request) {
      Value j{rapidjson::kObjectType};
      Set(j, "jsonrpc", "2.0");
      Set(j, "id", v.id);
      Set(j, "method", v.method);
      Set(j, "params", Value{v.params, allocator});
      return j;
    }

    DECODE(Request) {
      decode(v.id, Get(j, "id"));
      v.method = AsString(Get(j, "method"));
      v.params = AsDocument(Get(j, "params"));
    }

    ENCODE(Response) {
      Value j{rapidjson::kObjectType};
      Set(j, "jsonrpc", "2.0");
      Set(j, "id", v.id);
      visit_in_place(
          v.result,
          [&](const Response::Error &error) { Set(j, "error", error); },
          [&](const Document &result) {
            Set(j, "result", Value{result, allocator});
          });
      return j;
    }

    DECODE(Response) {
      decode(v.id, Get(j, "id"));
      if (j.HasMember("error")) {
        v.result = decode<Response::Error>(Get(j, "error"));
      } else {
        v.result = AsDocument(Get(j, "result"));
      }
    }

    ENCODE(Response::Error) {
      Value j{rapidjson::kObjectType};
      Set(j, "code", v.code);
      Set(j, "message", v.message);
      return j;
    }

    DECODE(Response::Error) {
      decode(v.code, Get(j, "code"));
      v.message = AsString(Get(j, "message"));
    }

    ENCODE(int64_t) {
      return Value{v};
    }

    DECODE(int64_t) {
      if (!j.IsInt64()) {
        outcome::raise(JsonError::WRONG_TYPE);
      }
      v = j.GetInt64();
    }

    ENCODE(uint64_t) {
      return Value{v};
    }

    DECODE(uint64_t) {
      if (!j.IsUint64()) {
        outcome::raise(JsonError::WRONG_TYPE);
      }
      v = j.GetUint64();
    }

    ENCODE(std::string_view) {
      return {v.data(), static_cast<rapidjson::SizeType>(v.size()), allocator};
    }

    ENCODE(gsl::span<const uint8_t>) {
      return encode(base64::encode(v.data(), v.size()));
    }

    template <size_t N>
    DECODE(std::array<uint8_t COMMA N>) {
      auto bytes = decodeBase64(j);
      if (bytes.size() != N) {
        outcome::raise(JsonError::WRONG_LENGTH);
      }
      std::copy(bytes.begin(), bytes.end(), v.begin());
    }

    DECODE(std::vector<uint8_t>) {
      v = decodeBase64(j);
    }

    DECODE(Buffer) {
      v = Buffer{decodeBase64(j)};
    }

    ENCODE(CID) {
      OUTCOME_EXCEPT(str, v.toString());
      Value j{rapidjson::kObjectType};
      Set(j, "/", str);
      return j;
    }

    DECODE(CID) {
      OUTCOME_EXCEPT(cid, CID::fromString(AsString(Get(j, "/"))));
      v = std::move(cid);
    }

    ENCODE(Ticket) {
      Value j{rapidjson::kObjectType};
      Set(j, "VRFProof", gsl::make_span(v.bytes));
      return j;
    }

    DECODE(Ticket) {
      decode(v.bytes, Get(j, "VRFProof"));
    }

    ENCODE(TipsetKey) {
      return encode(v.cids);
    }

    DECODE(TipsetKey) {
      return decode(v.cids, j);
    }

    ENCODE(Address) {
      return encode(primitives::address::encodeToString(v));
    }

    DECODE(Address) {
      OUTCOME_EXCEPT(decoded,
                     primitives::address::decodeFromString(AsString(j)));
      v = std::move(decoded);
    }

    ENCODE(Signature) {
      uint64_t type;
      gsl::span<const uint8_t> data;
      visit_in_place(
          v,
          [&](const BlsSignature &bls) {
            type = SignatureType::BLS;
            data = gsl::make_span(bls);
          },
          [&](const Secp256k1Signature &secp) {
            type = SignatureType::SECP256K1;
            data = gsl::make_span(secp);
          });
      Value j{rapidjson::kObjectType};
      Set(j, "Type", type);
      Set(j, "Data", data);
      return j;
    }

    DECODE(Signature) {
      uint64_t type;
      decode(type, Get(j, "Type"));
      auto &data = Get(j, "Data");
      if (type == SignatureType::BLS) {
        v = decode<BlsSignature>(data);
      } else if (type == SignatureType::SECP256K1) {
        v = decode<Secp256k1Signature>(data);
      } else {
        outcome::raise(JsonError::WRONG_ENUM);
      }
    }

    ENCODE(EPostTicket) {
      Value j{rapidjson::kObjectType};
      Set(j, "Partial", gsl::make_span(v.partial));
      Set(j, "SectorID", v.sector_id);
      Set(j, "ChallengeIndex", v.challenge_index);
      return j;
    }

    DECODE(EPostTicket) {
      decode(v.partial, Get(j, "Partial"));
      decode(v.sector_id, Get(j, "SectorID"));
      decode(v.challenge_index, Get(j, "ChallengeIndex"));
    }

    ENCODE(PoStProof) {
      Value j{rapidjson::kObjectType};
      Set(j, "RegisteredProof", common::to_int(v.registered_proof));
      Set(j, "ProofBytes", gsl::make_span(v.proof));
      return j;
    }

    DECODE(PoStProof) {
      std::underlying_type_t<RegisteredProof> registered_proof;
      decode(registered_proof, Get(j, "RegisteredProof"));
      v.registered_proof = RegisteredProof{registered_proof};
      decode(v.proof, Get(j, "ProofBytes"));
    }

    ENCODE(EPostProof) {
      Value j{rapidjson::kObjectType};
      Set(j, "Proofs", v.proofs);
      Set(j, "PostRand", gsl::make_span(v.post_rand));
      Set(j, "Candidates", v.candidates);
      return j;
    }

    DECODE(EPostProof) {
      decode(v.proofs, Get(j, "Proofs"));
      decode(v.post_rand, Get(j, "PostRand"));
      decode(v.candidates, Get(j, "Candidates"));
    }

    ENCODE(BigInt) {
      return encode(boost::lexical_cast<std::string>(v));
    }

    DECODE(BigInt) {
      v = BigInt{AsString(j)};
    }

    ENCODE(BlockHeader) {
      Value j{rapidjson::kObjectType};
      Set(j, "Miner", v.miner);
      Set(j, "Ticket", v.ticket);
      Set(j, "EPostProof", v.epost_proof);
      Set(j, "Parents", v.parents);
      Set(j, "ParentWeight", v.parent_weight);
      Set(j, "Height", v.height);
      Set(j, "ParentStateRoot", v.parent_state_root);
      Set(j, "ParentMessageReceipts", v.parent_message_receipts);
      Set(j, "Messages", v.messages);
      Set(j, "BLSAggregate", v.bls_aggregate);
      Set(j, "Timestamp", v.timestamp);
      Set(j, "BlockSig", v.block_sig);
      Set(j, "ForkSignaling", v.fork_signaling);
      return j;
    }

    DECODE(BlockHeader) {
      decode(v.miner, Get(j, "Miner"));
      decode(v.ticket, Get(j, "Ticket"));
      decode(v.epost_proof, Get(j, "EPostProof"));
      decode(v.parents, Get(j, "Parents"));
      decode(v.parent_weight, Get(j, "ParentWeight"));
      decode(v.height, Get(j, "Height"));
      decode(v.parent_state_root, Get(j, "ParentStateRoot"));
      decode(v.parent_message_receipts, Get(j, "ParentMessageReceipts"));
      decode(v.messages, Get(j, "Messages"));
      decode(v.bls_aggregate, Get(j, "BLSAggregate"));
      decode(v.timestamp, Get(j, "Timestamp"));
      decode(v.block_sig, Get(j, "BlockSig"));
      decode(v.fork_signaling, Get(j, "ForkSignaling"));
    }

    ENCODE(Tipset) {
      Value j{rapidjson::kObjectType};
      Set(j, "Cids", v.cids);
      Set(j, "Blocks", v.blks);
      Set(j, "Height", v.height);
      return j;
    }

    DECODE(Tipset) {
      decode(v.cids, Get(j, "Cids"));
      decode(v.blks, Get(j, "Blocks"));
      decode(v.height, Get(j, "Height"));
    }

    ENCODE(MessageReceipt) {
      Value j{rapidjson::kObjectType};
      Set(j, "ExitCode", static_cast<uint64_t>(v.exit_code));
      Set(j, "Return", gsl::make_span(v.return_value));
      Set(j, "GasUsed", v.gas_used);
      return j;
    }

    DECODE(MessageReceipt) {
      uint64_t exit_code;
      decode(exit_code, Get(j, "ExitCode"));
      v.exit_code = exit_code;
      decode(v.return_value, Get(j, "Return"));
      decode(v.gas_used, Get(j, "GasUsed"));
    }

    ENCODE(MsgWait) {
      Value j{rapidjson::kObjectType};
      Set(j, "Receipt", v.receipt);
      Set(j, "TipSet", v.tipset);
      return j;
    }

    DECODE(MsgWait) {
      decode(v.receipt, Get(j, "Receipt"));
      decode(v.tipset, Get(j, "TipSet"));
    }

    ENCODE(MinerPower) {
      Value j{rapidjson::kObjectType};
      Set(j, "MinerPower", v.miner);
      Set(j, "TotalPower", v.total);
      return j;
    }

    DECODE(MinerPower) {
      decode(v.miner, Get(j, "MinerPower"));
      decode(v.total, Get(j, "TotalPower"));
    }

    ENCODE(StorageParticipantBalance) {
      Value j{rapidjson::kObjectType};
      Set(j, "Locked", v.locked);
      Set(j, "Available", v.available);
      return j;
    }

    DECODE(StorageParticipantBalance) {
      decode(v.locked, Get(j, "Locked"));
      decode(v.available, Get(j, "Available"));
    }

    ENCODE(RleBitset) {
      return encode(std::vector<uint64_t>{v.begin(), v.end()});
    }

    DECODE(RleBitset) {
      std::vector<uint64_t> values;
      decode(values, j);
      v = {values.begin(), values.end()};
    }

    ENCODE(UnsignedMessage) {
      Value j{rapidjson::kObjectType};
      Set(j, "To", v.to);
      Set(j, "From", v.from);
      Set(j, "Nonce", v.nonce);
      Set(j, "Value", v.value);
      Set(j, "GasPrice", v.gasPrice);
      Set(j, "GasLimit", v.gasLimit);
      Set(j, "Method", v.method.method_number);
      Set(j, "Params", gsl::make_span(v.params));
      return j;
    }

    DECODE(UnsignedMessage) {
      decode(v.to, Get(j, "To"));
      decode(v.from, Get(j, "From"));
      decode(v.nonce, Get(j, "Nonce"));
      decode(v.value, Get(j, "Value"));
      decode(v.gasPrice, Get(j, "GasPrice"));
      decode(v.gasLimit, Get(j, "GasLimit"));
      decode(v.method.method_number, Get(j, "Method"));
      decode(v.params, Get(j, "Params"));
    }

    ENCODE(SignedMessage) {
      Value j{rapidjson::kObjectType};
      Set(j, "Message", v.message);
      Set(j, "Signature", v.signature);
      return j;
    }

    DECODE(SignedMessage) {
      decode(v.message, Get(j, "Message"));
      decode(v.signature, Get(j, "Signature"));
    }

    ENCODE(SectorPreCommitInfo) {
      Value j{rapidjson::kObjectType};
      Set(j, "RegisteredProof", common::to_int(v.registered_proof));
      Set(j, "SectorNumber", v.sector);
      Set(j, "SealedCID", v.sealed_cid);
      Set(j, "SealRandEpoch", v.seal_epoch);
      Set(j, "DealIDs", v.deal_ids);
      Set(j, "Expiration", v.expiration);
      return j;
    }

    DECODE(SectorPreCommitInfo) {
      decodeEnum(v.registered_proof, Get(j, "RegisteredProof"));
      decode(v.sector, Get(j, "SectorNumber"));
      decode(v.sealed_cid, Get(j, "SealedCID"));
      decode(v.seal_epoch, Get(j, "SealRandEpoch"));
      decode(v.deal_ids, Get(j, "DealIDs"));
      decode(v.expiration, Get(j, "Expiration"));
    }

    ENCODE(SectorOnChainInfo) {
      Value j{rapidjson::kObjectType};
      Set(j, "Info", v.info);
      Set(j, "ActivationEpoch", v.activation_epoch);
      Set(j, "DealWeight", v.deal_weight);
      Set(j, "PledgeRequirement", v.pledge_requirement);
      Set(j, "DeclaredFaultEpoch", v.declared_fault_duration);
      Set(j, "DeclaredFaultDuration", v.declared_fault_duration);
      return j;
    }

    DECODE(SectorOnChainInfo) {
      decode(v.info, Get(j, "Info"));
      decode(v.activation_epoch, Get(j, "ActivationEpoch"));
      decode(v.deal_weight, Get(j, "DealWeight"));
      decode(v.pledge_requirement, Get(j, "PledgeRequirement"));
      decode(v.declared_fault_duration, Get(j, "DeclaredFaultEpoch"));
      decode(v.declared_fault_duration, Get(j, "DeclaredFaultDuration"));
    }

    ENCODE(ChainSectorInfo) {
      Value j{rapidjson::kObjectType};
      Set(j, "Info", v.info);
      Set(j, "ID", v.id);
      return j;
    }

    DECODE(ChainSectorInfo) {
      decode(v.info, Get(j, "Info"));
      decode(v.id, Get(j, "ID"));
    }

    ENCODE(ModularVerificationParameter) {
      Value j{rapidjson::kObjectType};
      Set(j, "Actor", v.actor);
      Set(j, "Method", v.method.method_number);
      Set(j, "Data", gsl::make_span(v.data));
      return j;
    }

    DECODE(ModularVerificationParameter) {
      decode(v.actor, Get(j, "Actor"));
      decode(v.method.method_number, Get(j, "Method"));
      decode(v.data, Get(j, "Data"));
    }

    ENCODE(Merge) {
      Value j{rapidjson::kObjectType};
      Set(j, "Lane", v.lane);
      Set(j, "Nonce", v.nonce);
      return j;
    }

    DECODE(Merge) {
      decode(v.lane, Get(j, "Lane"));
      decode(v.nonce, Get(j, "Nonce"));
    }

    ENCODE(SignedVoucher) {
      Value j{rapidjson::kObjectType};
      Set(j, "TimeLockMin", v.time_lock_min);
      Set(j, "TimeLockMax", v.time_lock_max);
      Set(j, "SecretPreimage", gsl::make_span(v.secret_preimage));
      Set(j, "Extra", v.extra);
      Set(j, "Lane", v.lane);
      Set(j, "Nonce", v.nonce);
      Set(j, "Amount", v.amount);
      Set(j, "MinCloseHeight", v.min_close_height);
      Set(j, "Merges", v.merges);
      Set(j, "Signature", v.signature);
      return j;
    }

    DECODE(SignedVoucher) {
      decode(v.time_lock_min, Get(j, "TimeLockMin"));
      decode(v.time_lock_max, Get(j, "TimeLockMax"));
      decode(v.secret_preimage, Get(j, "SecretPreimage"));
      decode(v.extra, Get(j, "Extra"));
      decode(v.lane, Get(j, "Lane"));
      decode(v.nonce, Get(j, "Nonce"));
      decode(v.amount, Get(j, "Amount"));
      decode(v.min_close_height, Get(j, "MinCloseHeight"));
      decode(v.merges, Get(j, "Merges"));
      decode(v.signature, Get(j, "Signature"));
    }

    ENCODE(HeadChange) {
      const char *type{};
      if (v.type == HeadChangeType::CURRENT) {
        type = "current";
      } else if (v.type == HeadChangeType::REVERT) {
        type = "revert";
      } else if (v.type == HeadChangeType::APPLY) {
        type = "apply";
      }
      Value j{rapidjson::kObjectType};
      Set(j, "Type", type);
      Set(j, "Val", v.value);
      return j;
    }

    DECODE(HeadChange) {
      auto type = AsString(Get(j, "Type"));
      if (type == "current") {
        v.type = HeadChangeType::CURRENT;
      } else if (type == "revert") {
        v.type = HeadChangeType::REVERT;
      } else if (type == "apply") {
        v.type = HeadChangeType::APPLY;
      } else {
        outcome::raise(JsonError::WRONG_ENUM);
      }
      decode(v.value, Get(j, "Val"));
    }

    template <typename T>
    ENCODE(Chan<T>) {
      return encode(v.id);
    }

    template <typename T>
    DECODE(Chan<T>) {
      return decode(v.id, j);
    }

    ENCODE(Actor) {
      Value j{rapidjson::kObjectType};
      Set(j, "Code", v.code);
      Set(j, "Head", v.head);
      Set(j, "Nonce", v.nonce);
      Set(j, "Balance", v.balance);
      return j;
    }

    DECODE(Actor) {
      decode(v.code, Get(j, "Code"));
      decode(v.head, Get(j, "Head"));
      decode(v.nonce, Get(j, "Nonce"));
      decode(v.balance, Get(j, "Balance"));
    }

    ENCODE(InvocResult) {
      Value j{rapidjson::kObjectType};
      Set(j, "Msg", v.message);
      Set(j, "MsgRct", v.receipt);
      Set(j, "InternalExecutions", v.internal_executions);
      Set(j, "Error", v.error);
      return j;
    }

    DECODE(InvocResult) {
      decode(v.message, Get(j, "Msg"));
      decode(v.receipt, Get(j, "MsgRct"));
      decode(v.internal_executions, Get(j, "InternalExecutions"));
      v.error = AsString(Get(j, "Error"));
    }

    ENCODE(ExecutionResult) {
      Value j{rapidjson::kObjectType};
      Set(j, "Msg", v.message);
      Set(j, "MsgRct", v.receipt);
      Set(j, "Error", v.error);
      return j;
    }

    DECODE(ExecutionResult) {
      decode(v.message, Get(j, "Msg"));
      decode(v.receipt, Get(j, "MsgRct"));
      v.error = AsString(Get(j, "Error"));
    }

    ENCODE(DealProposal) {
      Value j{rapidjson::kObjectType};
      Set(j, "PieceCID", v.piece_cid);
      Set(j, "PieceSize", static_cast<uint64_t>(v.piece_size));
      Set(j, "Client", v.client);
      Set(j, "Provider", v.provider);
      Set(j, "StartEpoch", v.start_epoch);
      Set(j, "EndEpoch", v.end_epoch);
      Set(j, "StoragePricePerEpoch", v.storage_price_per_epoch);
      Set(j, "ProviderCollateral", v.provider_collateral);
      Set(j, "ClientCollateral", v.client_collateral);
      return j;
    }

    DECODE(DealProposal) {
      decode(v.piece_cid, Get(j, "PieceCID"));
      v.piece_size = decode<uint64_t>(Get(j, "PieceSize"));
      decode(v.client, Get(j, "Client"));
      decode(v.provider, Get(j, "Provider"));
      decode(v.start_epoch, Get(j, "StartEpoch"));
      decode(v.end_epoch, Get(j, "EndEpoch"));
      decode(v.storage_price_per_epoch, Get(j, "StoragePricePerEpoch"));
      decode(v.provider_collateral, Get(j, "ProviderCollateral"));
      decode(v.client_collateral, Get(j, "ClientCollateral"));
    }

    ENCODE(DealState) {
      Value j{rapidjson::kObjectType};
      Set(j, "SectorStartEpoch", v.sector_start_epoch);
      Set(j, "LastUpdatedEpoch", v.last_updated_epoch);
      Set(j, "SlashEpoch", v.slash_epoch);
      return j;
    }

    DECODE(DealState) {
      decode(v.sector_start_epoch, Get(j, "SectorStartEpoch"));
      decode(v.last_updated_epoch, Get(j, "LastUpdatedEpoch"));
      decode(v.slash_epoch, Get(j, "SlashEpoch"));
    }

    ENCODE(MarketDeal) {
      Value j{rapidjson::kObjectType};
      Set(j, "Proposal", v.proposal);
      Set(j, "State", v.state);
      return j;
    }

    DECODE(MarketDeal) {
      decode(v.proposal, Get(j, "Proposal"));
      decode(v.state, Get(j, "State"));
    }

    ENCODE(BlockMsg) {
      Value j{rapidjson::kObjectType};
      Set(j, "Header", v.header);
      Set(j, "BlsMessages", v.bls_messages);
      Set(j, "SecpkMessages", v.secp_messages);
      return j;
    }

    DECODE(BlockMsg) {
      decode(v.header, Get(j, "Header"));
      decode(v.bls_messages, Get(j, "BlsMessages"));
      decode(v.secp_messages, Get(j, "SecpkMessages"));
    }

    template <typename T>
    ENCODE(boost::optional<T>) {
      if (v) {
        return encode(*v);
      }
      return {};
    }

    template <typename T>
    DECODE(boost::optional<T>) {
      if (!j.IsNull()) {
        v = decode<T>(j);
      }
    }

    template <typename T>
    ENCODE(std::vector<T>) {
      Value j{rapidjson::kArrayType};
      j.Reserve(v.size(), allocator);
      for (auto &elem : v) {
        j.PushBack(encode(elem), allocator);
      }
      return j;
    }

    template <typename T>
    DECODE(std::vector<T>) {
      if (!j.IsArray()) {
        outcome::raise(JsonError::WRONG_TYPE);
      }
      v.reserve(j.Size());
      for (auto it = j.Begin(); it != j.End(); ++it) {
        v.emplace_back(decode<T>(*it));
      }
    }

    template <typename T>
    ENCODE(std::map<std::string COMMA T>) {
      Value j{rapidjson::kObjectType};
      j.MemberReserve(v.size(), allocator);
      for (auto &pair : v) {
        Set(j, pair.first, pair.second);
      }
      return j;
    }

    template <typename T>
    DECODE(std::map<std::string COMMA T>) {
      for (auto it = j.MemberBegin(); it != j.MemberEnd(); ++it) {
        v.emplace(AsString(it->name), decode<T>(it->value));
      }
    }

    template <size_t i = 0, typename T>
    void encodeTuple(Value &j, const T &tuple) {
      if constexpr (i < std::tuple_size_v<T>) {
        j.PushBack(encode(std::get<i>(tuple)), allocator);
        encodeTuple<i + 1>(j, tuple);
      }
    }

    template <typename... T>
    ENCODE(std::tuple<T...>) {
      Value j{rapidjson::kArrayType};
      j.Reserve(sizeof...(T), allocator);
      encodeTuple(j, v);
      return j;
    }

    template <size_t i = 0, typename... T>
    DECODE(std::tuple<T...>) {
      if (!j.IsArray()) {
        outcome::raise(JsonError::WRONG_TYPE);
      }
      if constexpr (i < sizeof...(T)) {
        decode(std::get<i>(v), j[i]);
        decode<i + 1>(v, j);
      }
    }

    template <typename T>
    static T decode(const Value &j) {
      T v;
      decode(v, j);
      return v;
    }
  };

  template <typename T>
  static Document encode(const T &v) {
    Document document;
    static_cast<Value &>(document) = Codec{document.GetAllocator()}.encode(v);
    return document;
  }

  template <typename T>
  outcome::result<T> decode(const Value &j) {
    try {
      return Codec::decode<T>(j);
    } catch (std::system_error &e) {
      return outcome::failure(e.code());
    }
  }
}  // namespace fc::api

#endif  // CPP_FILECOIN_CORE_API_RPC_JSON_HPP
