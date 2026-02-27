#pragma once
#include <cstdint>
#include <iostream>
#include <fstream>

struct SystemEventMessage{
    uint64_t timestamp;
    char eventCode;
};

struct StockDirectoryMessage{
    std::string ticker;
    char marketCatagory;
    char financialStatusIndicator;
    uint32_t roundLotSize;
    char roundLotsOnly;
    char issueClassification;
    char issueSubtype[2];
};

struct StockTradingAction{
    uint64_t timestamp;
    std::string ticker;
    char tradingState;
    std::string tradingActionReason;
};

struct NonCrossTradeMessage{
    uint64_t timestamp;
    uint64_t referenceNumber;
    char buyOrSell;
    uint32_t shares;
    std::string ticker;
    double price;
    uint64_t matchNumber;
};

struct stockTradingAction{
    uint64_t timestamp;
    std::string ticker;
    char tradingState;
};

struct ShortSaleIndicator{
    uint64_t timestamp;
    std::string ticker;
    char status;
};

struct MarketParticipantPosition{
    std::string MPIdentifier;
    std::string ticker;
    char participantState;
};

struct BasicAddOrderMessage{
    uint64_t timestamp;
    uint64_t orderReferenceNumber;
    char buyOrSell;
    uint32_t shareCount;
    std::string ticker;
    double price;
};

struct MPID_AddOrder{
    uint64_t timestamp;
    uint64_t orderReferenceNumber;
    char buyOrSell;
    uint32_t shareCount;
    std::string ticker;
    double price;
    std::string MPIdentifier;
};

struct PartialCancellationOrder{
    uint64_t timestamp;
    uint64_t orderReferenceNumber;
    uint32_t shareCount;
};

struct DeleteOrderMessage{
    uint64_t timestamp;
    uint64_t referenceNumber;
};

struct OrderExecutedMessage{
    uint64_t timestamp;
    uint64_t orderReferenceNumber;
    uint32_t executedShares;
    uint64_t matchNumber;
};

struct OrderReplaceMessage{
    uint64_t timestamp;
    uint64_t oldReferenceNumber;
    uint64_t newReferenceNumber;
    uint32_t shareCount;
    double price;
};

struct  MWCB_DeclineMessage{
    uint64_t timestamp;
    double priceLevel1;
    double priceLevel2;
    double priceLevel3;
};

struct PriceOrderExecutedMessage{
    uint64_t timestamp;
    uint64_t orderReferenceNumber;
    uint32_t executedShares;
    uint64_t matchNumber;
    char printable;
    uint32_t price;
};

struct IPOQuoteUpdate{
    uint64_t timestamp;
    std::string ticker;
    uint32_t releaseTime;
    char releaseStatus;
    uint32_t price;
};

struct NOIIMessage{
    uint64_t timestamp;
    uint64_t pairedShares;
    uint64_t imbalanceShares;
    char imbalanceDirection;
    std::string ticker;
    uint32_t farPrice;
    uint32_t nearPrice;
    uint32_t referencePrice;
    char crossType;
    char priceVariationIndicator;
};
// Need to add "Message" before every struct
