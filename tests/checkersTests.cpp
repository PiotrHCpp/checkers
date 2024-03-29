#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/gameInterfaces.hpp"
#include "../src/checkers.hpp"

struct NetworkClientSenderMock : public NetworkClientSender
{
    MOCK_METHOD1(sendToOpponent, void(Move));
};

struct UiUpdaterMock : public UiUpdater
{
    MOCK_METHOD1(updateGameState, void(Move));
};

struct CheckersTests : public testing::Test
{
    testing::StrictMock<NetworkClientSenderMock> networkClientSenderMock;
    testing::StrictMock<UiUpdaterMock> uiUpdaterMock;
    MoveExecutor& moveExecutor = checkers;
    NetworkClientReceiver& networkClientReceiver = checkers;

    Checkers checkers;
    
    CheckersTests(Color color) : checkers(Checkers(networkClientSenderMock, uiUpdaterMock, color))
    { }

    void checkLocalValidMove(const Move& move)
    {
        EXPECT_CALL(networkClientSenderMock, sendToOpponent(move));
        ASSERT_TRUE(moveExecutor.tryLocalMove(move));
    }
 
    void checkReceivedMove(Move move)
    {
        EXPECT_CALL(uiUpdaterMock, updateGameState(move));
        networkClientReceiver.receiveFromOpponent(move);
    }
};

struct CheckersTestsWhenBlacksAreMine : public CheckersTests
{
    CheckersTestsWhenBlacksAreMine() : CheckersTests(Color::black) {}
};

struct CheckersTestWhenWhitesAreMine : public CheckersTests
{
    CheckersTestWhenWhitesAreMine() : CheckersTests(Color::white) {}
};

TEST_F(CheckersTestWhenWhitesAreMine, checkersMusntAllowInvalidMove)
{
    Move someInvalidMove = "18-29";
    ASSERT_FALSE(moveExecutor.tryLocalMove(someInvalidMove));
}

TEST_F(CheckersTestWhenWhitesAreMine, cantAllowMoveOutOfLeftEdge)
{
    Move someInvalidMove = "16-20";
    ASSERT_FALSE(moveExecutor.tryLocalMove(someInvalidMove));
}

struct CheckersTestValidMoves : public CheckersTestWhenWhitesAreMine, public ::testing::WithParamInterface<Move>
{
};

TEST_P(CheckersTestValidMoves, test)
{
    checkLocalValidMove(GetParam());
}

INSTANTIATE_TEST_SUITE_P(P, CheckersTestValidMoves, ::testing::Values("18-23", "16-21", "19-23"));

TEST_F(CheckersTestWhenWhitesAreMine, whiteCantMoveTwoTimesInARow)
{
    Move whiteValidMove = "19-23";
    Move secondWhiteValidMove = "18-22";
    checkLocalValidMove(whiteValidMove);
    ASSERT_FALSE(moveExecutor.tryLocalMove(secondWhiteValidMove));    
}

TEST_F(CheckersTestWhenWhitesAreMine, whiteCanMoveSecondTimeAfterBlacksMove)
{
    Move whiteValidMove = "19-23";
    Move blackValidMove = "33-28";
    Move secondWhiteValidMove = "18-22";

    checkLocalValidMove(whiteValidMove);
    checkReceivedMove(blackValidMove);
    checkLocalValidMove(secondWhiteValidMove);
}

TEST_F(CheckersTestsWhenBlacksAreMine, BlacksCanMoveAfterReceivingWhitesMoveFromTheNetwork)
{
    Move whiteValidMove = "19-23";
    Move blackValidMove = "33-28";

    checkReceivedMove(whiteValidMove);
    checkLocalValidMove(blackValidMove);
}

TEST_F(CheckersTestsWhenBlacksAreMine, BlacksCanMoveToTheRight)
{
    Move whiteValidMove = "19-23";
    Move blackValidMove = "33-29";

    checkReceivedMove(whiteValidMove);
    checkLocalValidMove(blackValidMove);
}

TEST_F(CheckersTestsWhenBlacksAreMine, cantAllowMoveOutOfTheRightEdge)
{
    Move whiteValidMove = "19-23";
    Move someInvalidMove = "35-31";

    checkReceivedMove(whiteValidMove);
    ASSERT_FALSE(moveExecutor.tryLocalMove(someInvalidMove));
}

TEST_F(CheckersTestWhenWhitesAreMine, whitesCantMoveBackwards)
{
    Move whiteValidMove = "20-24";
    Move blackValidMove = "33-29";
    Move whiteBackwardMove = "24-20";

    checkLocalValidMove(whiteValidMove);
    checkReceivedMove(blackValidMove);
    ASSERT_FALSE(moveExecutor.tryLocalMove(whiteBackwardMove));
}

TEST_F(CheckersTestsWhenBlacksAreMine, BlacksCantStart)
{
    Move blackValidMove = "33-29";
    ASSERT_FALSE(moveExecutor.tryLocalMove(blackValidMove));
}

TEST_F(CheckersTestWhenWhitesAreMine, CantMoveWhitesToTheOccupiedField)
{
    Move whiteValidMove = "19-23";
    Move blackValidMove = "33-29";
    Move whiteMoveToOccupiedField = "18-23";

    checkLocalValidMove(whiteValidMove);
    checkReceivedMove(blackValidMove);
    ASSERT_FALSE(moveExecutor.tryLocalMove(whiteMoveToOccupiedField));
}

TEST_F(CheckersTestWhenWhitesAreMine, CantMoveWhitesToTheFieldOccupiedByBlacks)
{
    Move whiteValidMove = "19-23";
    Move blackValidMove = "32-28";
    Move whiteMoveToOccupiedField = "23-28";

    checkLocalValidMove(whiteValidMove);
    checkReceivedMove(blackValidMove);
    ASSERT_FALSE(moveExecutor.tryLocalMove(whiteMoveToOccupiedField));
}

TEST_F(CheckersTestWhenWhitesAreMine, MoveWhiteStone)
{
    Move whiteValidMove = "19-23";
    Move blackValidMove = "32-28";
    Move whiteMoveToOccupiedField = "23-28";

    checkLocalValidMove(whiteValidMove);
    checkReceivedMove(blackValidMove);
    ASSERT_FALSE(moveExecutor.tryLocalMove(whiteMoveToOccupiedField));
}

TEST_F(CheckersTestWhenWhitesAreMine, CantMoveWhitesFromEmptyField)
{
    Move whiteInvalidMove = "21-26";
    ASSERT_FALSE(moveExecutor.tryLocalMove(whiteInvalidMove));
}
