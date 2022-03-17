

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>  
#include <ctime>      

using namespace std;


class Karta
{
public:
    enum rank {
        ESO = 1, DVOJKA, TROJKA, STVORKA, PATKA, SESTKA, SEDMICKA, OSMICKA, DEVIATKA, DESIATKA,
        DOLNIK, KRALOVNA, KRAL
    };
    enum suit { Krize, Kary, Srdcia, Piky };

    Karta(rank r, suit s) : m_Rank(r), m_Suit(s) {}

    int getValue() const
    {
        int value = m_Rank;
        if (value > 10) value = 10;
        return value;
    }

    friend ostream& operator<<(ostream& os, const Karta& Karta);



private:
    rank m_Rank;
    suit m_Suit;
};



ostream& operator<<(ostream& os, const Karta& Karta)
{
    switch (Karta.m_Rank)
    {
    case Karta::ESO:   os << "Eso";   break;
    case Karta::DVOJKA:   os << "DVOJKA";   break;
    case Karta::TROJKA: os << "TROJKA"; break;
    case Karta::STVORKA:  os << "STVORKA";  break;
    case Karta::PATKA:  os << "PATKA";  break;
    case Karta::SESTKA:   os << "SESTKA";   break;
    case Karta::SEDMICKA: os << "SEDMICKA"; break;
    case Karta::OSMICKA: os << "OSMICKA"; break;
    case Karta::DEVIATKA:  os << "DEVIATKA";  break;
    case Karta::DESIATKA:   os << "DESIATKA";   break;
    case Karta::DOLNIK:  os << "DOLNIK";  break;
    case Karta::KRALOVNA: os << "KRALOVNA"; break;
    case Karta::KRAL:  os << "KRAL";  break;
    }

    switch (Karta.m_Suit)
    {
    case Karta::Krize:    os << " Krize";    break;
    case Karta::Kary: os << " Kary"; break;
    case Karta::Srdcia:   os << " Srdcia";   break;
    case Karta::Piky:   os << " Piky";   break;
    }

    return os;
}


class SkupinaKariet
{
public:
    SkupinaKariet(int n) : m_otocenanahor(true) { m_Karty.reserve(n); }

    void addCard(Karta Karta) { m_Karty.push_back(Karta); }

    void otockarty() { m_otocenanahor = !m_otocenanahor; }

    ostream& printTo(ostream& os) const
    {
        if (m_otocenanahor) for (auto& Karta : m_Karty) os << Karta << ", ";
        return os << m_Karty.size() << " karty";
    }

protected:
    vector<Karta> m_Karty;
    bool m_otocenanahor;
};

ostream& operator<<(ostream& os, SkupinaKariet& gc)
{
    return gc.printTo(os);
}



class GenericHrac : public SkupinaKariet
{
public:
    GenericHrac(const string& meno = "") : SkupinaKariet(7), m_Meno(meno) {}


    virtual bool hituje() const = 0;

    int getTotal() const
    {
        int total = 0;
        bool containsAce = false;

        for (auto& Karta : m_Karty)
        {
            total += Karta.getValue();
            if (Karta.getValue() == Karta::ESO) containsAce = true;
        }


        if (containsAce && total <= 11) total += 10;
        return total;
    }

    bool jeBustnuty() const { return (getTotal() > 21); }
    void bust() const { cout << m_Meno << ": bust.\n"; }
    void total() const { cout << m_Meno << ":\t(" << getTotal() << ")"; }

    ostream& printTo(ostream& os) const
    {
        os << m_Meno << ": ";
        return SkupinaKariet::printTo(os);
    }

protected:
    string m_Meno;
};

ostream& operator<<(ostream& os, GenericHrac& gp)
{
    return gp.printTo(os);
}


class Hrac : public GenericHrac
{
public:
    Hrac(const string& meno = "") : GenericHrac(meno) {}

    virtual bool hituje() const override
    {
        cout << m_Meno << " Chces si zobrat dalsiu kartu? (A/N): ";
        char odpoved;
        cin >> odpoved;
        return (odpoved == 'a' || odpoved == 'A');
    }

    void vyhra()  const { total(); cout << " vyhral.\n"; }
    void prehra() const { total(); cout << " prehral.\n"; }
    void push() const { total(); cout << " push.\n"; }
};


class krupier : public GenericHrac
{
public:
    krupier(const string& meno = "krupier") : GenericHrac(meno) {}

    virtual bool hituje() const override { return getTotal() <= 16; }
};


class Deck : public SkupinaKariet
{
public:
    Deck() : SkupinaKariet(52)
    {
        for (int s = Karta::Krize; s <= Karta::Piky; ++s)
            for (int r = Karta::ESO; r <= Karta::KRAL; ++r)
                m_Karty.push_back(Karta(static_cast<Karta::rank>(r),
                    static_cast<Karta::suit>(s)));

        srand(static_cast<unsigned int>(time(0)));
        random_shuffle(m_Karty.begin(), m_Karty.end());
    }

    void deal(GenericHrac& hrac)
    {
        hrac.addCard(m_Karty.back());
        m_Karty.pop_back();
    }

    void additionalCards(GenericHrac& gp)
    {
        cout << endl;
        while (!gp.jeBustnuty() && gp.hituje())
        {
            deal(gp);
            cout << gp << endl;
            if (gp.jeBustnuty()) gp.bust();
        }
    }
};

//-------------------------------------------------------------------- Class Game:
class Game
{
public:
    Game(const vector<string>& mena)
    {
        for (auto& meno : mena) m_Hraci.push_back(Hrac(meno));
    }

    void hraj();

private:
    Deck m_Deck;
    krupier m_krupier;
    vector<Hrac> m_Hraci;
};


void Game::hraj()
{
    for (int i = 0; i < 2; ++i)
    {
        for (auto& hrac : m_Hraci) m_Deck.deal(hrac);
        m_Deck.deal(m_krupier);
    }

    m_krupier.otockarty();

    for (auto& hrac : m_Hraci) cout << hrac << endl;
    cout << m_krupier << endl;

    for (auto& hrac : m_Hraci) m_Deck.additionalCards(hrac);

    m_krupier.otockarty();
    cout << endl << m_krupier;

    m_Deck.additionalCards(m_krupier);

    cout << endl;
    m_krupier.total();
    cout << endl;
    if (m_krupier.jeBustnuty())
    {
        for (auto& hrac : m_Hraci)
            if (!hrac.jeBustnuty()) hrac.vyhra();
    }
    else
    {
        for (auto& hrac : m_Hraci)
        {
            if (!hrac.jeBustnuty())
            {
                int hracTotal = hrac.getTotal();
                int krupierTotal = m_krupier.getTotal();

                if (hracTotal > krupierTotal)      hrac.vyhra();
                else if (hracTotal < krupierTotal) hrac.prehra();
                else                              hrac.push();
            }
        }
    }
}


//-------------------------------------------------------------------------- main:
int main()
{
    cout << "================== Vitajte v Blackjacku ==================\n\n";
    cout << "Princíp hry: " << endl;
    cout << "Každý hráè dostane na zaèiatku hry dve karty, ku ktorým mu postupne krupier ponúka ïalšie a hráè sa rozhoduje, èi ich prijme, alebo nie. V kasíne hráte proti krupiérovi a snažíte sa dosiahnu súèet (hodnotu) kariet vyšší ako má krupier, nie však vyšší ako 21.A to môžete viacerými spôsobmi. Buï skonèíte kolo s vyššou hodnotou kariet ako má krupier alebo na zaèiatku kola dostanete karty eso a kartu s hodnotou 10. Takisto sa môže sta, že krupier prehrá, ak prekroèí hodnotu 21." << endl;


    int numhraci = 0;
    while (numhraci < 1 || numhraci>7)
    {
        cout << "Ko¾ko hráèov?(1 az 7): ";
        cin >> numhraci;
    }

    vector<string> mena;

    cout << endl;
    for (int i = 1; i <= numhraci; ++i)
    {
        cout << "Zadajte svoje meno: " << i << ": ";
        string meno;
        cin >> meno;
        mena.push_back(meno);


    }

    char znova = 'A';
    while (znova != 'n' && znova != 'N')
    {

        Game game(mena);
        game.hraj();
        cout << "\nChcete hrat znova?(A/N): ";
        cin >> znova;
    }

    return 0;
}




