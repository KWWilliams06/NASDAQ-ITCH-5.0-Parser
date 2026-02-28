#include "MessageParser.h"
#include "Messages.h"
#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>

// Constructor
MessageParser::MessageParser(const std::string& fileName){
    inFile.open(fileName, std::ios::in | std::ios::binary);
    inFile.read((char*)&fileLength, 2);
    if(inFile){
        std::cout << "File opened successfully!" << std::endl;

    } else {
        std::cerr << "Failed to open file!" << std::endl;
    }
}

uint64_t MessageParser::readTimestamp(){
    uint64_t rawTimestamp;
    inFile.read((char*)&rawTimestamp, 6);
    rawTimestamp = __builtin_bswap64(rawTimestamp << 16);
    return rawTimestamp;
}

double MessageParser::readPrice8(){
    uint64_t rawPrice3;
    inFile.read((char*)&rawPrice3, 8);
    rawPrice3 = __builtin_bswap64(rawPrice3);
    return rawPrice3 / 100000000.0;
}


uint64_t MessageParser::readRefNumber(){
    uint64_t rawRefNumber;
    inFile.read((char*)&rawRefNumber, 8);
    rawRefNumber = __builtin_bswap64(rawRefNumber);
    return rawRefNumber;
}

double MessageParser::readPrice4(){
    uint32_t rawPrice;
    inFile.read((char*)&rawPrice, 4);
    rawPrice = __builtin_bswap32(rawPrice);
    return rawPrice / 10000.0;
}

uint32_t MessageParser::readShareCount(){
    uint32_t rawShareCount;
    inFile.read((char*)&rawShareCount, 4);
    rawShareCount = __builtin_bswap32(rawShareCount); // Convert from little to big endian
    return rawShareCount;
}

std::string MessageParser::readMPID(){
    char rawIdentifier[4];
    inFile.read((char*)&rawIdentifier, 4);
    std::string MPIdentifier;
    MPIdentifier = std::string(rawIdentifier, 4);
    MPIdentifier.erase(MPIdentifier.find_last_not_of(' ') + 1);
    return MPIdentifier;
}

std::string MessageParser::readTicker(){
    char rawTicker[8];
    inFile.read((char*)&rawTicker, 8);
    std::string ticker;
    ticker = std::string(rawTicker, 8);
    ticker.erase(ticker.find_last_not_of(' ') + 1);
    return ticker;
}

bool MessageParser::hasMoreMessages(){
    return inFile.good() && !inFile.eof();
}

void MessageParser::skipMessage(){
    uint16_t length;
    inFile.seekg(-2, std::ios::cur);
    inFile.read((char*)&length, 2);
    length = __builtin_bswap16(length);
    inFile.seekg(length + 2, std::ios::cur);
}

// Gets the type of the message to be parsed.
char MessageParser::getMessageType(){
    return inFile.peek();
}

// Parses a message of type "System Event"
SystemEventMessage MessageParser::parseSystemEvent(){
    SystemEventMessage msg;
    inFile.seekg(5, std::ios::cur);
    msg.timestamp = MessageParser::readTimestamp(); // Converts timestamp to little endian
    inFile.read((char*)&msg.eventCode, sizeof(char));
    std::cout << "Event Code:" << msg.eventCode << std::endl;
    std::cout << "Timestamp:" << msg.timestamp << std::endl;
    inFile.seekg(2, std::ios::cur);
    return msg;
}

StockDirectoryMessage MessageParser::parseStockDirectory(){
    StockDirectoryMessage msg;
    inFile.seekg(11, std::ios::cur);
    msg.ticker = MessageParser::readTicker();
    std::cout << "Ticker:" << msg.ticker << std::endl;
    // Read in Market Catagory
    inFile.read((char*)&msg.marketCatagory, 1);
    std::cout << "Catagory:" << msg.marketCatagory << std::endl;
    // Read in Financial Status
    inFile.read((char*)&msg.financialStatusIndicator, 1);
    std::cout << "Financial Status:" << msg.financialStatusIndicator << std::endl;
    // Read in round lot size
    uint32_t rawRoundLot;
    inFile.read((char*)&rawRoundLot, 4);
    msg.roundLotSize = __builtin_bswap32(rawRoundLot); // Convert from little to big endian
    std::cout << "Round lot:" << msg.roundLotSize << std::endl;
    // Read in round lots only
    inFile.read((char*)&msg.roundLotsOnly, 1);
    std::cout << "Round lots only?:" << msg.roundLotsOnly << std::endl;
    // Read in issue classification
    inFile.read((char*)&msg.issueClassification, 1);
    std::cout << "Issue classification:" << msg.issueClassification << std::endl;
    // Read in issue subtype
    inFile.read((char*)&msg.issueSubtype, 2);
    std::cout << "Issue subtype:" << msg.issueSubtype << std::endl;
    inFile.seekg(12, std::ios::cur);
    return msg;
}
StockTradingAction MessageParser::parseTradingAction(){
    StockTradingAction msg;
    inFile.seekg(5, std::ios::cur);
    msg.timestamp = MessageParser::readTimestamp();
    std::cout << "Timestamp:" << msg.timestamp << std::endl;
    msg.ticker = MessageParser::readTicker();
    std::cout << "Ticker:" << msg.ticker << std::endl;
    inFile.read((char*)&msg.tradingState, 1);
    std::cout << "Trading State:" << msg.tradingState << std::endl;
    inFile.seekg(7, std::ios::cur);
    return msg;
}

ShortSaleIndicator MessageParser::parseShortSaleIndicator(){
    ShortSaleIndicator msg;
    inFile.seekg(5, std::ios::cur);
    msg.timestamp = MessageParser::readTimestamp();
    std::cout << "Timestamp:" << msg.timestamp << std::endl;
    msg.ticker = MessageParser::readTicker();
    std::cout << "Ticker:" << msg.ticker << std::endl;
    inFile.read((char*)&msg.status, 1);
    std::cout << "Short Sale Status:" << msg.status << std::endl;
    inFile.seekg(2, std::ios::cur);
    return msg;
}

MarketParticipantPosition MessageParser::parseMarketParticipantPosition(){
    MarketParticipantPosition msg;
    inFile.seekg(11, std::ios::cur);
    msg.MPIdentifier = MessageParser::readMPID();
    std::cout << "MPID:" << msg.MPIdentifier << std::endl;
    msg.ticker = MessageParser::readTicker();
    std::cout << "Ticker:" << msg.ticker << std::endl;
    inFile.seekg(2, std::ios::cur);
    inFile.read((char*)&msg.participantState, 1);
    std::cout << "Participant State:" << msg.participantState << std::endl;
    inFile.seekg(2, std::ios::cur);
    return msg;
}

BasicAddOrderMessage MessageParser::parseBasicAddOrder(){
        BasicAddOrderMessage msg;
        inFile.seekg(5, std::ios::cur);
        msg.timestamp = MessageParser::readTimestamp();
        std::cout << "Timestamp:" << msg.timestamp << std::endl;
        uint64_t rawRefNumber;
        inFile.read((char*)&rawRefNumber, 8);
        msg.orderReferenceNumber = __builtin_bswap64(rawRefNumber);
        std::cout << "Reference Number:" << msg.orderReferenceNumber << std::endl;
        inFile.read((char*)&msg.buyOrSell, 1);
        std::cout << "Buy or Sell:" << msg.buyOrSell << std::endl;
        msg.shareCount = MessageParser::readShareCount();
        std::cout << "Shares:" << msg.shareCount << std::endl;
        msg.ticker = MessageParser::readTicker();
        std::cout << "Ticker:" << msg.ticker << std::endl;
        msg.price = MessageParser::readPrice4();
        std::cout << "Price:" << msg.price << std::endl;
        inFile.seekg(2, std::ios::cur);
        return msg;
}

MPID_AddOrder MessageParser::parseMPIDAddOrder(){
    MPID_AddOrder msg;
    inFile.seekg(5, std::ios::cur);
    msg.timestamp = MessageParser::readTimestamp();
    std::cout << "Timestamp:" << msg.timestamp << std::endl;
    uint64_t rawRefNumber;
    inFile.read((char*)&rawRefNumber, 8);
    msg.orderReferenceNumber = __builtin_bswap64(rawRefNumber);
    std::cout << "Reference Number:" << msg.orderReferenceNumber << std::endl;
    inFile.read((char*)&msg.buyOrSell, 1);
    std::cout << "Buy or Sell:" << msg.buyOrSell << std::endl;
    msg.shareCount = MessageParser::readShareCount();
    std::cout << "Shares:" << msg.shareCount << std::endl;
    msg.ticker = MessageParser::readTicker();
    std::cout << "Ticker:" << msg.ticker << std::endl;
    msg.price = MessageParser::readPrice4();
    std::cout << "Price:" << msg.price << std::endl;
    msg.MPIdentifier = MessageParser::readMPID();
    std::cout << "MPID:" << msg.MPIdentifier << std::endl;
    inFile.seekg(2, std::ios::cur);
    return msg;
}
DeleteOrderMessage MessageParser::parseDeleteOrder(){
    DeleteOrderMessage msg;
    inFile.seekg(5, std::ios::cur);
    msg.timestamp = MessageParser::readTimestamp();
    std::cout << "Timestamp:" << msg.timestamp << std::endl;
    uint64_t rawRefNumber;
    inFile.read((char*)&rawRefNumber, 8);
    msg.referenceNumber = __builtin_bswap64(rawRefNumber);
    std::cout << "Reference Number:" << msg.referenceNumber << std::endl;
    inFile.seekg(2, std::ios::cur);
    return msg;
}

PartialCancellationOrder MessageParser::parsePartialCancellation(){
    PartialCancellationOrder msg;
    inFile.seekg(5, std::ios::cur);
    msg.timestamp = MessageParser::readTimestamp();
    std::cout << "Timestamp:" << msg.timestamp << std::endl;
    msg.orderReferenceNumber = MessageParser::readRefNumber();
    std::cout << "Reference Number:" << msg.orderReferenceNumber << std::endl;
    msg.shareCount = MessageParser::readShareCount();
    std::cout << "Cancel count:" << msg.shareCount << std::endl;
    inFile.seekg(2, std::ios::cur);
    return msg;
}

OrderExecutedMessage MessageParser::parseOrderExecuted(){
    OrderExecutedMessage msg;
    inFile.seekg(5, std::ios::cur);
    msg.timestamp = MessageParser::readTimestamp();
    std::cout << "Timestamp:" << msg.timestamp << std::endl;
    msg.orderReferenceNumber = MessageParser::readRefNumber();
    std::cout << "Reference Number:" << msg.orderReferenceNumber << std::endl;
    msg.executedShares = readShareCount();
    std::cout << "Fill Count:" << msg.executedShares << std::endl;
    msg.matchNumber = MessageParser::readRefNumber(); // Works because theyre the same size
    std::cout << "Reference Number:" << msg.orderReferenceNumber << std::endl;
    inFile.seekg(2, std::ios::cur);
    return msg;
}

OrderReplaceMessage MessageParser::parseOrderReplace(){
    OrderReplaceMessage msg;
    inFile.seekg(5, std::ios::cur);
    msg.timestamp = MessageParser::readTimestamp();
    std::cout << "Timestamp:" << msg.timestamp << std::endl;
    msg.oldReferenceNumber = MessageParser::readRefNumber();
    std::cout << "Old Reference Number:" << msg.oldReferenceNumber << std::endl;
    msg.newReferenceNumber = MessageParser::readRefNumber();
    std::cout << "New Reference Number:" << msg.newReferenceNumber << std::endl;
    msg.shareCount = MessageParser::readShareCount();
    msg.price = MessageParser::readPrice4();
    std::cout << "Price:" << msg.price << std::endl;
    inFile.seekg(2, std::ios::cur);
    return msg;
}

NonCrossTradeMessage MessageParser::parseNonCrossTradeMessage(){
    NonCrossTradeMessage msg;
    inFile.seekg(5, std::ios::cur);
    msg.timestamp = MessageParser::readTimestamp();
    std::cout << "Timestamp:" << msg.timestamp << std::endl;
    msg.referenceNumber = MessageParser::readRefNumber();
    std::cout << "Reference Number:" << msg.referenceNumber << std::endl;
    inFile.read((char*)&msg.buyOrSell, 1);
    std::cout << "Buy or Sell:" << msg.buyOrSell << std::endl;
    msg.shares = MessageParser::readShareCount();
    std::cout << "Shares:" << msg.shares << std::endl;
    msg.ticker = MessageParser::readTicker();
    std::cout << "Ticker:" << msg.ticker << std::endl;
    msg.price = MessageParser::readPrice4();
    std::cout << "Price:" << msg.price << std::endl;
    msg.matchNumber = MessageParser::readRefNumber(); // Works because theyre the same size
    std::cout << "Match Number:" << msg.matchNumber << std::endl;
    inFile.seekg(2, std::ios::cur);
    return msg;
}

MWCB_DeclineMessage MessageParser::parseCircutBreaker(){
    MWCB_DeclineMessage msg;
    inFile.seekg(5, std::ios::cur);
    msg.timestamp = MessageParser::readTimestamp();
    std::cout << "Timestamp:" << msg.timestamp << std::endl;
    msg.priceLevel1 = MessageParser::readPrice8();
    std::cout << "Price 1:" << msg.priceLevel1 << std::endl;
    msg.priceLevel2 = MessageParser::readPrice8();
    std::cout << "Price 2:" << msg.priceLevel2 << std::endl;
    msg.priceLevel3 = MessageParser::readPrice8();
    std::cout << "Price 3:" << msg.priceLevel3 << std::endl;
    inFile.seekg(2, std::ios::cur);
    return msg;
}

PriceOrderExecutedMessage MessageParser::parseOrderExecuttionWithPrice(){
    PriceOrderExecutedMessage msg;
    inFile.seekg(5, std::ios::cur);
    msg.timestamp = MessageParser::readTimestamp();
    std::cout << "Timestamp:" << msg.timestamp << std::endl;
    msg.orderReferenceNumber = MessageParser::readRefNumber();
    std::cout << "Reference Number:" << msg.orderReferenceNumber << std::endl;
    msg.executedShares = MessageParser::readShareCount();
    std::cout << "Share Count:" << msg.executedShares << std::endl;
    msg.matchNumber = MessageParser::readRefNumber();
    std::cout << "Reference Number:" << msg.orderReferenceNumber << std::endl;
    inFile.read((char*)&msg.printable, 1);
    std::cout << "Printable?:" << msg.printable << std::endl;
    msg.price = MessageParser::readPrice4();
    std::cout << "Price:" << msg.price << std::endl;
    inFile.seekg(2, std::ios::cur);
    return msg;
}

IPOQuoteUpdate MessageParser::parseIPOUpdate(){
    IPOQuoteUpdate msg;
    inFile.seekg(5, std::ios::cur);
    msg.timestamp = MessageParser::readTimestamp();
    std::cout << "Timestamp:" << msg.timestamp << std::endl;
    msg.ticker = MessageParser::readTicker();
    std::cout << "Ticker:" << msg.ticker << std::endl;
    uint32_t rawReleaseTime;
    inFile.read((char*)&rawReleaseTime, 4);
    rawReleaseTime = __builtin_bswap32(rawReleaseTime);
    msg.releaseTime = rawReleaseTime;
    std::cout << "Release Time:" << msg.releaseTime << std::endl;
    inFile.read((char*)&msg.releaseStatus, 1);
    std::cout << "Status:" << msg.releaseStatus << std::endl;
    msg.price = readPrice4();
    std::cout << "Price:" << msg.price << std::endl;
    inFile.seekg(2, std::ios::cur);
    return msg;
}

NOIIMessage MessageParser::parseNOIIMessage(){
    NOIIMessage msg;
    inFile.seekg(5, std::ios::cur);
    msg.timestamp = MessageParser::readTimestamp();
    std::cout << "Timestamp:" << msg.timestamp << std::endl;
    inFile.read((char*)&msg.pairedShares, 8);
    msg.pairedShares = __builtin_bswap64(msg.pairedShares);
    std::cout << "Paired Shares:" << msg.pairedShares << std::endl;
    inFile.read((char*)&msg.imbalanceShares, 8);
    msg.imbalanceShares = __builtin_bswap64(msg.imbalanceShares);
    std::cout << "Imbalance Shares:" << msg.imbalanceShares << std::endl;
    inFile.read((char*)&msg.imbalanceDirection, 1);
    std::cout << "Imbalance Direction:" << msg.imbalanceDirection << std::endl;
    msg.ticker = readTicker();
    std::cout << "Ticker:" << msg.ticker << std::endl;
    msg.farPrice = MessageParser::readPrice4();
    std::cout << "Far Price:" << msg.farPrice << std::endl;
    msg.nearPrice = MessageParser::readPrice4();
    std::cout << "Near Price:" << msg.nearPrice << std::endl;
    msg.referencePrice = MessageParser::readPrice4();
    std::cout << "Reference Price:" << msg.referencePrice << std::endl;
    inFile.read((char*)&msg.crossType, 1);
    std::cout << "Cross Type:" << msg.crossType << std::endl;
    inFile.read((char*)&msg.priceVariationIndicator, 1);
    std::cout << "Price Variation:" << msg.priceVariationIndicator << std::endl;
    inFile.seekg(2, std::ios::cur);
    return msg;
}

CrossTradeMessage MessageParser::parseCrossTradeMessage(){
    CrossTradeMessage msg;
    inFile.seekg(5, std::ios::cur);
    msg.timestamp = MessageParser::readTimestamp();
    std::cout << "Timestamp:" << msg.timestamp << std::endl;
    msg.shareCount = readShareCount();
    std::cout << "Shares:" << msg.shareCount << std::endl;
    msg.ticker = readTicker();
    std::cout << "Ticker:" << msg.ticker << std::endl;
    msg.crossPrice = readPrice4();
    std::cout << "Cross Price:" << msg.crossPrice << std::endl;
    msg.matchNumber = readRefNumber();
    std::cout << "Match Number:" << msg.matchNumber << std::endl;
    inFile.read((char*)&msg.crossType, 1);
    std::cout << "Cross Type:" << msg.crossType << std::endl;
    inFile.seekg(2, std::ios::cur);
    return msg;
}
MWCB_StatusMessage parseMWCB_StatusMessage(){
    MWCB_StatusMessage msg;
    inFile.seekg(5, std::ios::cur);
    msg.timestamp = MessageParser::readTimestamp();
    std::cout << "Timestamp:" << msg.timestamp << std::endl;
    inFile.read((char*)&msg.breachLevel, 1);
    std::cout << "Breach Level:" << msg.breachLevel << std::endl;
    inFile.seekg(2, std::ios::cur);
    return msg;
}

LULD_AuctionCollar parseLULD_AuctionCollar(){
    LULD_AuctionCollar msg;
    inFile.seekg(5, std::ios::cur);
    msg.timestamp = MessageParser::readTimestamp();
    std::cout << "Timestamp:" << msg.timestamp << std::endl;
    msg.ticker = MessageParser.readTicker();
    std::cout << "Ticker:" << msg.ticker << std::endl;
    msg.referencePrice = MessageParser::readPrice4();
    std::cout << "Reference Price:" << msg.referencePrice << std::endl;
    msg.upperAuctionPrice = MessageParser::readPrice4();
    std::cout << "Upper Auction Price:" << msg.upperAuctionPrice << std::endl;
    msg.lowerAuctionPrice = MessageParser::readPrice4();
    std::cout << "Lower Auction Price:" << msg.lowerAuctionPrice << std::endl;
    inFile.read((char*)&msg.extensionCount, 4);
    msg.extensionCount = __builtin_bswap32(msg.extensionCount);
    std::cout << "Extension Count:" << msg.extensionCount << std::endl;
    inFile.seekg(2, std::ios::cur);
    return msg;
}

OperationalHalt parseOperationalHalt(){
    OperationalHalt msg;
    inFile.seekg(5, std::ios::cur);
    msg.timestamp = MessageParser::readTimestamp();
    std::cout << "Timestamp:" << msg.timestamp << std::endl;
    msg.ticker = MessageParser::readTicker();
    std::cout << "Ticker:" << msg.ticker << std::endl;
    inFile.read((char*)&msg.mktCode, 1);
    std::cout << "Market Code:" << msg.mktCode << std::endl;
    inFile.read((char*)&msg.haltAction, 1);
    std::cout << "Halt Action:" << msg.haltAction << std::endl;
    inFile.seekg(2, std::ios::cur);
    return msg;
}

RetailInterestMessage parseRetailInterestMessage(){
    RetailInterestMessage msg;
    inFile.seekg(5, std::ios::cur);
    msg.timestamp = MessageParser::readTimestamp();
    std::cout << "Timestamp:" << msg.timestamp << std::endl;
    msg.ticker = MessageParser::readTicker();
    std::cout << "Ticker:" << msg.ticker << std::endl;
    inFile.read((char*)&msg.interestFlag, 1);
    std::cout << "Interest Flag:" << msg.interestFlag << std::endl;
    inFile.seekg(2, std::ios::cur);
    return msg;
}

DirectListingCapitalRaiseMessage parseDirectListingCapitalRaise(){
    DirectListingCapitalRaiseMessage msg;
    inFile.seekg(5, std::ios::cur);
    msg.timestamp = MessageParser::readTimestamp();
    std::cout << "Timestamp:" << msg.timestamp << std::endl;
    msg.ticker = MessageParser::readTicker();
    std::cout << "Ticker:" << msg.ticker << std::endl;
    inFile.read((char*)&msg.eligibilityStatus, 1);
    std::cout << "Eligibility:" << msg.eligibilityStatus << std::endl;
    msg.minPrice = MessageParser::readPrice4();
    std::cout << "Min Price:" << msg.minPrice << std::endl;
    msg.maxPrice = MessageParser::readPrice4();
    std::cout << "Max Price:" << msg.maxPrice << std::endl;
    msg.nearExecutionPrice = MessageParser::readPrice4();
    std::cout << "Near Execution Price:" << msg.nearExecutionPrice << std::endl;
    inFile.read((char*)&msg.nearExecutionTime, 8);
    msg.nearExecutionTime = __builtin_bswap64(msg.nearExecutionTime);
    std::cout << "Near Execution Time:" << msg.nearExecutionTime << std::endl;
    msg.lowPriceCollar = MessageParser::readPrice4();
    std::cout << "Low Price Collar:" << msg.lowPriceCollar << std::endl;
    msg.highPriceCollar = MessageParser::readPrice4();
    std::cout << "High Price Collar:" << msg.highPriceCollar << std::endl;
    inFile.seekg(2, std::ios::cur);
    return msg;
}

BrokenTradeMessage parseBrokenTradeMessage(){
    BrokenTradeMessage msg;
    inFile.seekg(5, std::ios::cur);
    msg.timestamp = MessageParser::readTimestamp();
    std::cout << "Timestamp:" << msg.timestamp << std::endl;
    msg.matchNumber = MessageParser::readRefNumber();
    std::cout << "Match Number:" << msg.matchNumber << std::endl;
    inFile.seekg(2, std::ios::cur);
    return msg;
}
