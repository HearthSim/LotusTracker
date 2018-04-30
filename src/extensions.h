#ifndef EXTENSIONS_H
#define EXTENSIONS_H

#include <QString>

class Extensions final
{
public:
	static QString colorIdentityListToString(QList<QChar> distinctManaSymbols)
	{
        QString identity = "";
        if (distinctManaSymbols.contains(QChar('b'))) { identity += "b"; }
        if (distinctManaSymbols.contains(QChar('g'))) { identity += "g"; }
        if (distinctManaSymbols.contains(QChar('r'))) { identity += "r"; }
        if (distinctManaSymbols.contains(QChar('w'))) { identity += "w"; }
        if (distinctManaSymbols.contains(QChar('u'))) { identity += "u"; }
        if (identity == "bgr") { identity = "brg"; }
        if (identity == "grw") { identity = "rgw"; }
        if (identity == "bru") { identity = "ubr"; }
        if (identity == "bwu") { identity = "wub"; }
        return (!identity.isEmpty()) ? identity : "default";
	}
};

#endif // EXTENSIONS_H
