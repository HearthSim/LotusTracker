#ifndef LOTUSEXCEPTION_H
#define LOTUSEXCEPTION_H

#include <exception>
#include <QString>

class LotusException : public std::exception
{
    const QString content;

public:
    LotusException(QString content): content(content){}

    const char * what () const noexcept
    {
        return content.toUtf8();
    }
};

#endif // LOTUSEXCEPTION_H
