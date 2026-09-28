#include "KittyUtils.h"

static void xtrim(std::string &hex){
    if(hex.compare(0, 2, "0x") == 0){
        hex.erase(0, 2);
    }

    hex.erase(std::remove_if(hex.begin(), hex.end(), [](char c){
								return (c == ' ' || c == '\n' || c == '\r' ||
										c == '\t' || c == '\v' || c == '\f');
							}),
							hex.end());
}

bool KittyUtils::validateHexString(std::string &xstr){
    if(xstr.length() < 2) return false;
    xtrim(xstr);
    if(xstr.length() % 2 != 0) return false;
    for(size_t i = 0; i < xstr.length(); i++){
        if(!std::isxdigit((unsigned char)xstr[i])){
            return false;
        }
    }
    return true;
}

#include <sstream>
#include <iomanip>

void KittyUtils::toHex(
    void *const data,
    const size_t dataLength,
    std::string &dest
    )
{
    unsigned char     *byteData = reinterpret_cast<unsigned char*>(data);
    std::stringstream hexStringStream;

    hexStringStream << std::hex << std::setfill('0');
    for(size_t index = 0; index < dataLength; ++index)
        hexStringStream << std::setw(2) << static_cast<int>(byteData[index]);
    dest = hexStringStream.str();
}

void KittyUtils::fromHex(
    const std::string &in,
    void *const data
    )
{
    size_t          length    = in.length();
    unsigned char   *byteData = reinterpret_cast<unsigned char*>(data);

    std::stringstream hexStringStream; hexStringStream >> std::hex;
    for(size_t strIndex = 0, dataIndex = 0; strIndex < length; ++dataIndex)
    {

        const char tmpStr[3] = { in[strIndex++], in[strIndex++], 0 };

        hexStringStream.clear();
        hexStringStream.str(tmpStr);

        int tmpValue = 0;
        hexStringStream >> tmpValue;
        byteData[dataIndex] = static_cast<unsigned char>(tmpValue);
    }
}
