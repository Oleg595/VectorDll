#pragma once

class ICompact {
public:
    static ICompact* createCompact(IVector const * vec1, IVector const * vec2, IVector const * vec3, Logger* pLogger);

private:
    ICompact(const ICompact& compact) = delete;
    ICompact& operator=(const ICompact& compact) = delete;

protected:
    ICompact() = default;
};