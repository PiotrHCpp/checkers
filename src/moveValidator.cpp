#include "moveValidator.hpp"

MoveValidator::MoveValidator(BoardPtr occupiedFields, Color color) :
    occupiedFields(std::move(occupiedFields)), color(color)
{ }

bool MoveValidator::operator()(const CheckersMove& move)
{
    const bool isLandingFieldEmpty = false == (*occupiedFields)[move.getLandingField()];
    const bool isStartingFieldNotEmpty = true == (*occupiedFields)[move.getStartingField()];
    return isMoveValid(move) && isLandingFieldEmpty && isStartingFieldNotEmpty;
}

bool MoveValidator::isMoveValid(const CheckersMove& move) const
{
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

void MoveValidator::setLandingfieldOccupied(const CheckersMove& move)
{
    (*occupiedFields)[move.getLandingField()] = true;
}