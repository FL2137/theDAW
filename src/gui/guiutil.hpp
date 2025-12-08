#pragma once

#include <QString>
#include <string>

namespace guiutil {

inline QString intToQStr(int val) {
    std::string tmp = std::to_string(val);
    return QString::fromStdString(tmp);
}

}