#include "MessageParser.h"
#include <string>
#include <iostream>

int main(){
    const std::string filename = "03272019.NASDAQ_ITCH50";
    MessageParser parser (filename);

    while(parser.hasMoreMessages()){
    //for(int i = 0; i < 6; i++){
        char type = parser.getMessageType();
        std::cout << type << std::endl;
        switch(type){
            case 'S':
                parser.parseSystemEvent();
                break;
            case 'R':
                parser.parseStockDirectory();
                break;
            case 'P':
                parser.parseNonCrossTradeMessage();
                break;
            case 'H':
                parser.parseTradingAction();
                break;
            case 'Y':
                parser.parseShortSaleIndicator();
                break;
            case 'L':
                parser.parseMarketParticipantPosition();
                break;
            case 'A':
                parser.parseBasicAddOrder();
                break;
            case 'D':
                parser.parseDeleteOrder();
                break;
            case 'F':
                parser.parseMPIDAddOrder();
                break;
            case 'X':
                parser.parsePartialCancellation();
                break;
            case 'E':
                parser.parseOrderExecuted();
                break;
            case 'U':
                parser.parseOrderReplace();
                break;
            case 'V':
                parser.parseCircutBreaker();
                break;
            case 'C':
                parser.parseOrderExecuttionWithPrice();
                break;
            case 'K':
                parser.parseIPOUpdate();
                break;
            case 'I':
                parser.parseNOIIMessage();
                break;
            default:
                return 0;
        }
    }
    std::cout << "End of program." << std::endl;
    return 0;
}
