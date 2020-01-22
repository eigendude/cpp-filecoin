/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CPP_FILECOIN_CORE_VM_ACTOR_ACTOR_HPP
#define CPP_FILECOIN_CORE_VM_ACTOR_ACTOR_HPP

#include "common/buffer.hpp"
#include "primitives/address/address.hpp"
#include "primitives/big_int.hpp"
#include "primitives/cid/cid.hpp"

namespace fc::vm::actor {

  using fc::common::Buffer;
  using primitives::BigInt;
  using primitives::address::Address;

  /**
   * Consider MethodNum numbers to be similar in concerns as for offsets in
   * function tables (in programming languages), and for tags in ProtocolBuffer
   * fields. Tags in ProtocolBuffers recommend assigning a unique tag to a field
   * and never reusing that tag. If a field is no longer used, the field name
   * may change but should still remain defined in the code to ensure the tag
   * number is not reused accidentally. The same should apply to the MethodNum
   * associated with methods in Filecoin VM Actors.
   */
  struct MethodNumber {
    uint64_t method_number;

    inline bool operator==(const MethodNumber &other) const {
      return method_number == other.method_number;
    }
  };

  template <class Stream,
            typename = std::enable_if_t<
                std::remove_reference<Stream>::type::is_cbor_encoder_stream>>
  Stream &operator<<(Stream &&s, const MethodNumber &method) noexcept {
    return s << method.method_number;
  }

  template <class Stream,
            typename = std::enable_if_t<
                std::remove_reference<Stream>::type::is_cbor_decoder_stream>>
  Stream &operator>>(Stream &&s, MethodNumber &method) noexcept {
    return s >> method.method_number;
  }

  /**
   * MethodParams is serialized parameters to the method call
   */
  class MethodParams : public Buffer {
    using Buffer::Buffer;

   public:
    inline bool operator==(const MethodParams &other) const {
      return Buffer::operator==(static_cast<const Buffer &>(other));
    }
  };

  /**
   * CodeID identifies an actor's code (either one of the builtin actors, or, in
   * the future, potentially a CID of VM code for a custom actor)
   */
  class CodeId : public CID {
   public:
    CodeId() = default;
    explicit CodeId(CID cid) : CID{std::move(cid)} {}
  };

  class ActorSubstateCID : public CID {
   public:
    ActorSubstateCID() = default;
    explicit ActorSubstateCID(CID cid) : CID{std::move(cid)} {}
  };

  /**
   * Common actor state interface represents the on-chain storage all actors
   * keep
   */
  struct Actor {
    /// Identifies the code this actor executes
    CodeId code{};
    /// CID of the root of optional actor-specific sub-state
    ActorSubstateCID head{};
    /// Expected sequence number of the next message sent by this actor
    uint64_t nonce{};
    /// Balance of tokens held by this actor
    BigInt balance{};
  };

  bool operator==(const Actor &lhs, const Actor &rhs);

  template <class Stream,
            typename = std::enable_if_t<
                std::remove_reference_t<Stream>::is_cbor_encoder_stream>>
  Stream &operator<<(Stream &&s, const Actor &actor) {
    return s << (s.list() << actor.code << actor.head << actor.nonce
                          << actor.balance);
  }

  template <class Stream,
            typename = std::enable_if_t<
                std::remove_reference_t<Stream>::is_cbor_decoder_stream>>
  Stream &operator>>(Stream &&s, Actor &actor) {
    s.list() >> actor.code >> actor.head >> actor.nonce >> actor.balance;
    return s;
  }

  /** Check if code specifies builtin actor implementation */
  bool isBuiltinActor(const CodeId &code);

  /** Check if only one instance of actor should exists */
  bool isSingletonActor(const CodeId &code);

  extern const CID kEmptyObjectCid;

  extern const CodeId kAccountCodeCid, kCronCodeCid, kStoragePowerCodeCid,
      kStorageMarketCodeCid, kStorageMinerCodeCid, kMultisigCodeCid,
      kInitCodeCid, kPaymentChannelCodeCid;

  inline static const auto kInitAddress = Address::makeFromId(0);
  inline static const auto kStoragePowerAddress = Address::makeFromId(2);
  inline static const auto kCronAddress = Address::makeFromId(4);
}  // namespace fc::vm::actor

#endif  // CPP_FILECOIN_CORE_VM_ACTOR_ACTOR_HPP
