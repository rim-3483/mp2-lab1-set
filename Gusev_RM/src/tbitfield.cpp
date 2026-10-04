// ННГУ, ВМК, Курс "Методы программирования-2", С++, ООП
//
// tbitfield.cpp - Copyright (c) Гергель В.П. 07.05.2001
//   Переработано для Microsoft Visual Studio 2008 Сысоевым А.В. (19.04.2015)
//
// Битовое поле

#include "tbitfield.h"

// Fake variables used as placeholders in tests
static const int BITS_PER_WORD = sizeof(TELEM) * 8;

TBitField::TBitField(int len)
{
    if (len < 0)                                         
        throw std::invalid_argument("length must be non negative");
    if (len < 0) len = 0;
    BitLen = len;
    MemLen = (len + BITS_PER_WORD - 1) / BITS_PER_WORD;
    pMem = (MemLen > 0) ? new TELEM[MemLen] : nullptr;
    for (int i = 0; i < MemLen; i++)
        pMem[i] = 0;
}

TBitField::TBitField(const TBitField &bf) // конструктор копирования
{
    BitLen = bf.BitLen;
    MemLen = bf.MemLen;
    pMem = (MemLen > 0) ? new TELEM[MemLen] : nullptr;
    for (int i = 0; i < MemLen; i++)
        pMem[i] = bf.pMem[i];
}

TBitField::~TBitField()
{
    delete[] pMem;
}

int TBitField::GetMemIndex(const int n) const // индекс Мем для бита n
{
    return n/BITS_PER_WORD;
}

TELEM TBitField::GetMemMask(const int n) const // битовая маска для бита n
{
    return 1u << (n % BITS_PER_WORD);
}

// доступ к битам битового поля

int TBitField::GetLength(void) const // получить длину (к-во битов)
{
  return BitLen;
}

void TBitField::SetBit(const int n) // установить бит
{
    if (n < 0 || n >= BitLen)                              
        throw std::out_of_range("bit index out of range");
    pMem[GetMemIndex(n)] |= GetMemMask(n);
}

void TBitField::ClrBit(const int n) // очистить бит
{
    if (n < 0 || n >= BitLen)                              
        throw std::out_of_range("bit index out of range");
    pMem[GetMemIndex(n)] &= ~GetMemMask(n);
}

int TBitField::GetBit(const int n) const // получить значение бита
{
    if (n < 0 || n >= BitLen)                              // ← проверка
        throw std::out_of_range("bit index out of range");
    return (pMem[GetMemIndex(n)] & GetMemMask(n)) ? 1 : 0;
}

// битовые операции

TBitField& TBitField::operator=(const TBitField &bf) // присваивание
{
    if (this == &bf)
        return *this;

    if (MemLen != bf.MemLen) {
        delete[] pMem;
        MemLen = bf.MemLen;
        pMem = (MemLen > 0) ? new TELEM[MemLen] : nullptr;
    }
    BitLen = bf.BitLen;
    for (int i = 0; i < MemLen; i++)
        pMem[i] = bf.pMem[i];

    return *this;
}

int TBitField::operator==(const TBitField &bf) const // сравнение
{
    if (BitLen != bf.BitLen)
        return 0;                  

    for (int i = 0; i < MemLen; i++)
        if (pMem[i] != bf.pMem[i])
            return 0;              

    return 1;
}

int TBitField::operator!=(const TBitField &bf) const // сравнение
{
    if (BitLen != bf.BitLen)
        return 1;                 

    for (int i = 0; i < MemLen; i++)
        if (pMem[i] != bf.pMem[i])
            return 1;              

    return 0;
}

TBitField TBitField::operator|(const TBitField &bf) // операция "или"
{
    int maxBitLen = (BitLen > bf.BitLen) ? BitLen : bf.BitLen;
    TBitField res(maxBitLen);

    int minWords = (MemLen < bf.MemLen) ? MemLen : bf.MemLen;
    for (int i = 0; i < minWords; i++)
        res.pMem[i] = pMem[i] | bf.pMem[i];

    for (int i = minWords; i < MemLen; i++)
        res.pMem[i] = pMem[i];

    for (int i = minWords; i < bf.MemLen; i++)
        res.pMem[i] = bf.pMem[i];

    return res;
}

TBitField TBitField::operator&(const TBitField &bf) // операция "и"
{
    int maxlen = (BitLen > bf.BitLen) ? BitLen : bf.BitLen;
    TBitField res(maxlen);
    int minword = (MemLen < bf.MemLen) ? MemLen : bf.MemLen;
    for (int i = 0; i < minword; i++) {
        res.pMem[i] = pMem[i] & bf.pMem[i];
    }
    return res;
}

TBitField TBitField::operator~(void) // отрицание
{
    TBitField res(BitLen);
    for (int i = 0; i < MemLen; i++) {
        res.pMem[i] = ~pMem[i];
    }
    int tail = BitLen % BITS_PER_WORD;
    if (tail != 0 && MemLen > 0)
        res.pMem[MemLen - 1] &= (1u << tail) - 1;
    return res;
}

// ввод/вывод

istream &operator>>(istream &istr, TBitField &bf) // ввод
{
    for (int i = 0; i < bf.BitLen; i++) {
        char ch;
        istr >> ch;
        if (ch == '1') {
            bf.SetBit(i);
        }
        else {
            bf.ClrBit(i);
        }
    }
    return istr;
}

ostream &operator<<(ostream &ostr, const TBitField &bf) // вывод
{
    for (int i = 0; i < bf.GetLength(); i++) {
        ostr << bf.GetBit(i);
    }
    return ostr;
}
