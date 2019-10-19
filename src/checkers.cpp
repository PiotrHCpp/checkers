#include <string>
#include "checkers.hpp"
#include "checkersMove.hpp"

Checkers::Checkers(NetworkClientSender& ncs, UiUpdater& uIU, Color color) : networkClientSender(ncs), uiUpdater(uIU), isMyTurn(true), color(color)
{ }

bool Checkers::isMoveValid(Move m) const
{
    CheckersMove move(m);
    const bool isStoneOnTheLeftEdge = 6 == move.getStartingField() %10;
    const bool isMoveToTheDownLeft = 4 == move.getFieldDifference();
    const bool isMoveToTheDownRight = 5 == move.getFieldDifference();
    const bool isMoveToTheUpLeft = -5 == move.getFieldDifference();
    const bool isMoveToTheUpRight = -4 == move.getFieldDifference();
    const bool isStoneOnTheRightEdge = 5 == move.getStartingField() %10;
    const bool allConditionsForWhites = isMoveToTheDownRight or (isMoveToTheDownLeft and !isStoneOnTheLeftEdge);
    const bool allConditionsForBlacks = isMoveToTheUpLeft or (isMoveToTheUpRight and !isStoneOnTheRightEdge);
    return ((allConditionsForWhites and color == Color::white) or (allConditionsForBlacks and color == Color::black));
}

void Checkers::receiveFromOpponent(Move move)
{
    uiUpdater.updateGameState(move);
    isMyTurn = true;
}

bool Checkers::tryLocalMove(Move move)
{
    if (isMyTurn && isMoveValid(move))
    {
        networkClientSender.sendToOpponent(move);
        isMyTurn = false;
        return true;
    }
    return false;
}
