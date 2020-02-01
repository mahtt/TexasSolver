//
// Created by Xuefeng Huang on 2020/1/31.
//

#include "ranges/RiverRangeManager.h"

#include <utility>

RiverRangeManager::RiverRangeManager() = default;

RiverRangeManager::RiverRangeManager(shared_ptr<Compairer> handEvaluator) {
    this->handEvaluator = std::move(handEvaluator);
}

const vector<RiverCombs> &
RiverRangeManager::getRiverCombos(int player, const vector<RiverCombs> &riverCombos, const vector<int> &board) {
    vector<PrivateCards> preflopCombos(riverCombos.size());
    for(int i = 0;i < riverCombos.size();i ++){
        preflopCombos[i] = riverCombos[i].private_cards;
    }
    return getRiverCombos(player,preflopCombos,board);
}

const vector<RiverCombs> &
RiverRangeManager::getRiverCombos(int player, const vector<PrivateCards> &riverCombos, const vector<int> &board) {
    uint64_t board_long = Card::boardInts2long(board);
    return this->getRiverCombos(player,riverCombos,board_long);
}

const vector<RiverCombs> &
RiverRangeManager::getRiverCombos(int player, const vector<PrivateCards> &preflopCombos, uint64_t board_long) {
    unordered_map<uint64_t , vector<RiverCombs>>* riverRanges;

    if (player == 0)
        riverRanges = &p1RiverRanges;
    else if (player == 1)
        riverRanges = &p2RiverRanges;
    else
        throw runtime_error(fmt::format("player {} not found",player));

    uint64_t key = board_long;

    if (riverRanges->find(key) != riverRanges->end())
        return (*riverRanges)[key];

    int count = 0;

    for (auto one_hand : preflopCombos) {
        if (!Card::boardsHasIntercept(
                one_hand.toBoardLong(), board_long
        ))
            count++;
    }

    int index = 0;
    vector<RiverCombs> riverCombos = vector<RiverCombs>(count);

    for (int hand = 0; hand < preflopCombos.size(); hand++)
    {
        PrivateCards preflopCombo = preflopCombos[hand];


        if (Card::boardsHasIntercept(
                preflopCombo.toBoardLong(), board_long
        )){
            continue;
        }

        int rank = this->handEvaluator->get_rank(preflopCombo.toBoardLong(),board_long);
        RiverCombs riverCombo = RiverCombs(Card::long2board(board_long),preflopCombo,rank, hand);
        riverCombos[index++] = riverCombo;
    }

    std::sort(riverCombos.begin(),riverCombos.end(),[ ]( const RiverCombs& lhs, const RiverCombs& rhs )
    {
        return lhs.rank > rhs.rank;
    });

    (*riverRanges)[key] =  std::move(riverCombos);

    return (*riverRanges)[key];
}
