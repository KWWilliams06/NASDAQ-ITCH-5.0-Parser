#pragma once

#include "Messages.h"
#include <cstdint>
#include <fstream>
#include <string>

class MessageParser{

    private:
        std::ifstream inFile;
        uint16_t fileLength;


        uint64_t readTimestamp();
        uint32_t readShareCount();
        std::string readTicker();
        std::string readMPID();
        uint64_t readRefNumber();
        double readPrice4();
        double readPrice8();

    public:
        MessageParser(const std::string& fileName); // Constructor

        bool hasMoreMessages(); // Does the file have more messages to parse?

        void skipMessage();

        char getMessageType(); // Returns the type of message being parsed

        SystemEventMessage parseSystemEvent(); // Parses a SystemEventMessage after determining its of that type

        StockDirectoryMessage parseStockDirectory();

        StockTradingAction parseTradingAction();

        ShortSaleIndicator parseShortSaleIndicator();

        MarketParticipantPosition parseMarketParticipantPosition();

        BasicAddOrderMessage parseBasicAddOrder();

        MPID_AddOrder parseMPIDAddOrder();

        DeleteOrderMessage parseDeleteOrder();

        PartialCancellationOrder parsePartialCancellation();

        OrderExecutedMessage parseOrderExecuted();

        OrderReplaceMessage parseOrderReplace();

        NonCrossTradeMessage parseNonCrossTradeMessage();

        MWCB_DeclineMessage parseCircutBreaker();

        PriceOrderExecutedMessage parseOrderExecuttionWithPrice();

        IPOQuoteUpdate parseIPOUpdate();

        NOIIMessage parseNOIIMessage();

        CrossTradeMessage parseCrossTradeMessage();

        MWCB_StatusMessage parseMWCB_StatusMessage();

        LULD_AuctionCollar parseLULD_AuctionCollar();

        OperationalHalt parseOperationalHalt();

        RetailInterestMessage parseRetailInterestMessage();

        DirectListingCaptialRaiseMessage parseDirectListingCapitalRaise();

        BrokenTradeMessage parseBrokenTradeMessage();
};
