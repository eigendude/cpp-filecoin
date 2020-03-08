/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CPP_FILECOIN_CORE_VM_RUNTIME_IMPL_RUNTIME_IMPL_HPP
#define CPP_FILECOIN_CORE_VM_RUNTIME_IMPL_RUNTIME_IMPL_HPP

#include "filecoin/crypto/randomness/randomness_provider.hpp"
#include "filecoin/storage/ipfs/datastore.hpp"
#include "filecoin/vm/actor/invoker.hpp"
#include "filecoin/vm/runtime/actor_state_handle.hpp"
#include "filecoin/vm/runtime/env.hpp"
#include "filecoin/vm/runtime/runtime.hpp"
#include "filecoin/vm/state/state_tree.hpp"

namespace fc::vm::runtime {

  using actor::Actor;
  using actor::Invoker;
  using crypto::randomness::ChainEpoch;
  using crypto::randomness::Randomness;
  using crypto::randomness::RandomnessProvider;
  using indices::Indices;
  using primitives::address::Address;
  using state::StateTree;
  using storage::ipfs::IpfsDatastore;

  class RuntimeImpl : public Runtime {
   public:
    RuntimeImpl(std::shared_ptr<Env> env,
                UnsignedMessage message,
                Address immediate_caller,
                BigInt gas_available,
                BigInt gas_used,
                ActorSubstateCID current_actor_state);

    /** \copydoc Runtime::getCurrentEpoch() */
    ChainEpoch getCurrentEpoch() const override;

    /** \copydoc Runtime::getRandomness() */
    Randomness getRandomness(DomainSeparationTag tag,
                             ChainEpoch epoch) const override;

    /** \copydoc Runtime::getRandomness() */
    Randomness getRandomness(DomainSeparationTag tag,
                             ChainEpoch epoch,
                             Serialization seed) const override;

    /** \copydoc Runtime::getImmediateCaller() */
    Address getImmediateCaller() const override;

    /** \copydoc Runtime::getCurrentReceiver() */
    Address getCurrentReceiver() const override;

    /** \copydoc Runtime::getTopLevelBlockWinner() */
    Address getTopLevelBlockWinner() const override;

    /** \copydoc Runtime::acquireState() */
    std::shared_ptr<ActorStateHandle> acquireState() const override;

    /** \copydoc Runtime::getBalance() */
    outcome::result<BigInt> getBalance(const Address &address) const override;

    /** \copydoc Runtime::getValueReceived() */
    BigInt getValueReceived() const override;

    /** \copydoc Runtime::getCurrentIndices() */
    std::shared_ptr<Indices> getCurrentIndices() const override;

    /** \copydoc Runtime::getActorCodeID() */
    outcome::result<CodeId> getActorCodeID(
        const Address &address) const override;

    /** \copydoc Runtime::send() */
    fc::outcome::result<InvocationOutput> send(Address to_address,
                                               MethodNumber method_number,
                                               MethodParams params,
                                               BigInt value) override;

    /** \copydoc Runtime::createActor() */
    outcome::result<void> createActor(const Address &address,
                                      const Actor &actor) override;

    /** \copydoc Runtime::deleteActor() */
    outcome::result<void> deleteActor() override;

    /** \copydoc Runtime::getIpfsDatastore() */
    std::shared_ptr<IpfsDatastore> getIpfsDatastore() override;

    /** \copydoc Runtime::getMessage() */
    std::reference_wrapper<const UnsignedMessage> getMessage() override;

    outcome::result<void> chargeGas(const BigInt &amount) override;

    ActorSubstateCID getCurrentActorState() override;

    outcome::result<void> commit(const ActorSubstateCID &new_state) override;

    BigInt gasUsed() const;

    static outcome::result<void> transfer(Actor &from,
                                          Actor &to,
                                          const BigInt &amount);

    outcome::result<Address> resolveAddress(const Address &address) override;

    outcome::result<bool> verifyPoSt(uint64_t sector_size,
                                     const PoStVerifyInfo &info) override;

    outcome::result<bool> verifySeal(uint64_t sector_size,
                                     const SealVerifyInfo &info) override;

    outcome::result<bool> verifyConsensusFault(
        const BlockHeader &block_header_1,
        const BlockHeader &block_header_2) override;

   private:
    outcome::result<Actor> getOrCreateActor(const Address &address);
    std::shared_ptr<Runtime> createRuntime(
        const UnsignedMessage &message,
        const ActorSubstateCID &current_actor_state) const;

   private:
    std::shared_ptr<Env> env_;
    std::shared_ptr<StateTree> state_tree_;
    UnsignedMessage message_;
    Address immediate_caller_;
    BigInt gas_available_;
    BigInt gas_used_;
    ActorSubstateCID current_actor_state_;
  };

}  // namespace fc::vm::runtime

#endif  // CPP_FILECOIN_CORE_VM_RUNTIME_IMPL_RUNTIME_IMPL_HPP