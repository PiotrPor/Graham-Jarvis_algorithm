//Saint John Festival
//Algorytm Grahama i Algorytm Jarvisa

#include <iostream>
#include <cmath> //potrzebne do atanf()

struct Lampion
{
    double x; //wspolrzedne dochodza do 2^30 (long ma 32 bity, a double 64)
    double y;
};

//wiecej komentarzy o funkcjach juz za main()'em
bool operator==(Lampion a, Lampion b);
Lampion znajdz_najnizszy(Lampion tab[], int ilosc);
int indeks_nastepnego(int indeks_teraz, Lampion tab[], Lampion granica[], int ile_w_otoczce, int ilosc);
bool czy_jest_w_srodku(Lampion a, Lampion b, Lampion sprawdzany);
//ponizsze 2 funkcje sa potrzebne do zrealizowania sposobu stad (wiecej wspomniane w pozniejszych komentarzach):
//www.tutorialspoint.com/convex-hull-jarvis-s-algorithm-or-wrapping-in-cplusplus
int budowa_otoczki(Lampion tab[], Lampion hull[], int ilosc);
bool przeciwny_zegarowi(Lampion a, Lampion b, Lampion c);

int main()
{
    int ilosc_duzych; // 3 < L < 10000
    int ilosc_malych; // 1 < S < 50000
    int a, b; //potrzebne do petli
    //czytanie wartosci o duzych lampionach
    std::cin >> ilosc_duzych;
    if (ilosc_duzych < 3) { ilosc_duzych = 3; } //IF tylko by nie bylo warning
    Lampion* duze = new Lampion[ilosc_duzych];
    for (a = 0; a < ilosc_duzych; a++)
    {
        std::cin >> duze[a].x;
        std::cin >> duze[a].y;
    }
    //czytanie wartosci o malych lampionach
    std::cin >> ilosc_malych;
    Lampion* male = new Lampion[ilosc_malych];
    for (a = 0; a < ilosc_malych; a++)
    {
        std::cin >> male[a].x;
        std::cin >> male[a].y;
    }
    //od teraz zaczyna sie algorytm Grahama
    //najpierw trzeba znalezc punkt o najnizszej wspolrzednej "y"
    Lampion najnizszy = znajdz_najnizszy(duze, ilosc_duzych);
    //Potem trzeba przesunac wszystkie punkty tak, by znalazl sie on w poczatku ukladu
    for (a = 0; a < ilosc_duzych; a++)
    {
        duze[a].x -= najnizszy.x;
        duze[a].y -= najnizszy.y;
    }
    for (a = 0; a < ilosc_malych; a++)
    {
        male[a].x -= najnizszy.x;
        male[a].y -= najnizszy.y;
    }
    //Teraz trzeba znalezc otoczke dla gromady punktow
    //w naszym zadaniu odpowiada to otoczce w zbiorze duzych lampionow
    //ta otoczke wykonamy algorytmem Jarvisa
    //algorytm Jarvisa (pl.wikipedia.org/wiki/Algorytm_Jarvisa)
    //moje zastosowanie algorytmu Jarvisa wykorzystuje zmodyfikowane funkcje z:
    // www.tutorialspoint.com/convex-hull-jarvis-s-algorithm-or-wrapping-in-cplusplus
    Lampion* otoczka = new Lampion[ilosc_duzych];
    otoczka[0].x = 0;
    otoczka[0].y = 0;
    int i = 1;
    int nr_nastepnego = indeks_nastepnego(0, duze, otoczka, i, ilosc_duzych);
    bool juz_wpisany = false;
    otoczka[1] = duze[nr_nastepnego];
    i++;
    int ilosc_elementow_otoczki = 1; //potrzebne do petli
    //funkcja zbuduje otoczke i jeszcze zwroci ilosc elementow otoczki
    ilosc_elementow_otoczki = budowa_otoczki(duze, otoczka, ilosc_duzych);
    //teraz czas na sprawdzanie czy poszczegolne male lampiony sa wewnatrz otoczki
    bool* srodkowe = new bool[ilosc_malych]; //informuje czy dane male lampiony sa wewnatrz otoczki
    for (a = 0; a < ilosc_malych; a++)
    {
        srodkowe[a] = true; //najpierw zakladamy, ze wszystkie sa wewnatrz otoczki
    }
    for (a = 0; a < ilosc_elementow_otoczki - 1; a++)
    {
        for (b = 0; b < ilosc_malych; b++)
        {
            if (!czy_jest_w_srodku(otoczka[a], otoczka[a + 1], male[b]))
            {
                if (srodkowe[b]) { srodkowe[b] = false; } //ten male lampion nie jest wewnatrz otoczki
            }
        }
    }
    //to powinien byc ostatni przebieg poprzedniej petli FOR() (tej najwiekszej)
    //tutaj bierzemy pod uwage ostatni odcinek otoczki (biegnie od ostatniego punktu do pierwszego)
    for (b = 0; b < ilosc_malych; b++)
    {
        if (!czy_jest_w_srodku(otoczka[ilosc_elementow_otoczki - 1], otoczka[0], male[b]))
        {
            if (srodkowe[b]) { srodkowe[b] = false; }
        }
    }
    //trzeba napisac ile bylo malych lampionow w obrysie z duzych lampionow
    int ile_wewnatrz = 0;
    for (a = 0; a < ilosc_malych; a++)
    {
        if (srodkowe[a]) { ile_wewnatrz++; }
    }
    std::cout << ile_wewnatrz << std::endl;
    //zwalnianie pamieci po skonczonej pracy
    delete[] duze;
    delete[] male;
    delete[] otoczka;
    delete[] srodkowe;
    return 0;
}

//potrzebne do porownania czy dwa punkty/lampiony maja identyczne wspolrzedne
bool operator==(Lampion a, Lampion b)
{
    if ((a.x == b.x) && (a.y == b.y))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//szuka duzego lampiona, ktory bedzie punktem zerowym otoczki
Lampion znajdz_najnizszy(Lampion tab[], int ilosc)
{
    Lampion najnizszy = tab[0];
    int a;
    //szukamy punktu z najmniejsza wspolrzedna "y"
    for (a = 1; a < ilosc; a++)
    {
        if (tab[a].y < najnizszy.y)
        {
            najnizszy = tab[a];
        }
    }
    //jesli jest kilka najnizszych punktow, to wybieramy ten z najmniejszym "x"
    for (a = 0; a < ilosc; a++)
    {
        if ((tab[a].y == najnizszy.y) && (tab[a].x < najnizszy.x))
        {
            najnizszy = tab[a];
        }
    }
    return najnizszy;
}

/*
  Funkcja indeks_nastepnego() zwraca indeks z tabeli z duzymi lampionami.
  Duzy lampion o tym indeksie ma byc nastepnym w otoczce
  Argumenty:
   > indeks_teraz - indeks w tablicy duzych lampionow wskazujacy na aktualnie ostatni lampion otoczki
   > tab[] - tablica z duzymi lampionami
   > granica[] - tablica duzych lampionow bedacych otoczka
   > ile_w_otoczce - ile aktualnie lampionow tworzy otoczke
   > ilosc - ile jest duzych lampionow
*/
int indeks_nastepnego(int indeks_teraz, Lampion tab[], Lampion granica[], int ile_w_otoczce, int ilosc)
{
    Lampion tenteraz = tab[indeks_teraz];
    int a;
    int jegoindeks;
    double roznica_x, roznica_y;
    double kat, kat_poprzedniego_od_osi;
    double najwiekszy_kat;
    double liczbapi = 2 * asin(1); // asin(1) = pi/2
    if (ile_w_otoczce > 1) //otoczka[0] ma wspolrzedne (0;0)
    {
        double jego_y = granica[ile_w_otoczce - 1].y;
        double jego_x = granica[ile_w_otoczce - 1].x;
        kat_poprzedniego_od_osi = atan(jego_y / jego_x);
    }
    else
    {
        kat_poprzedniego_od_osi = (double)0;
    }
    jegoindeks = 0;
    najwiekszy_kat = 0;
    for (a = 0; a < ilosc; a++)
    {
        roznica_x = tenteraz.x - tab[a].x;
        roznica_y = tenteraz.y - tab[a].y;
        kat = atan((double)(-1) * roznica_y / roznica_x); //mnozenie przez -1, bo kat zaczyna sie liczyc od osi poziomej po lewo.
        kat += kat_poprzedniego_od_osi;
        while (kat >= 2 * liczbapi) //na wypadek gdyby kat byl wiekszy niz 360*
        {
            kat -= 2 * liczbapi;
        }
        if (kat > najwiekszy_kat)
        {
            jegoindeks = a;
            najwiekszy_kat = kat;
        }
    }
    return jegoindeks; //indeks duzego lampiona, ktory ma byc nastepny w otoczce
}

//liczy iloczyn wektorowy, aby sprawdzic czy wektor od lampiona otoczki do malego lampiona
// jest na prawo od wektora laczacego ten lampion otoczki z nastepnym lampionem otoczki
bool czy_jest_w_srodku(Lampion a, Lampion b, Lampion sprawdzany)
{
    Lampion wektorAB;
    wektorAB.x = b.x - a.x;
    wektorAB.y = b.y - a.y;
    Lampion wektorSPR;
    wektorSPR.x = sprawdzany.x - a.x;
    wektorSPR.y = sprawdzany.y - a.y;
    //liczenie iloczynu wektorowego
    long long iloczyn;
    iloczyn = (long long)(wektorAB.x * wektorSPR.y) - (long long)(wektorSPR.x * wektorAB.y);
    //werdykt
    if (iloczyn < 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}
/*
 Funkcja budowa_otoczki() buduje otoczke i zwraca ilosc duzych lampionow tworzacych otoczke
 Argumenty: tablica z duzymi lampionami, tablica bedaca otoczka, ilosc duzych lampionow
*/
int budowa_otoczki(Lampion tab[], Lampion hull[], int ilosc)
{
    //na tym etapie punkt startowy jest juz wpisany
    int ile_juz_w_otoczce = 1;
    int a, indeks=0, xyz;
    for (a = 0; a < ilosc; a++)
    {
        if (tab[a] == hull[0])
        {
            indeks = a;
            break;
        }
    }
    //mamy juz indeks punktu zerowego z tabeli duzych lampionow wpisany do 'indeks'
    //imitujemy pierwszy przebieg do{}while() bez dopisywania do otoczki
    xyz = (indeks + 1) % ilosc; //czy ta linijka jest potrzebna?
    for (a = 0; a < ilosc; a++)
    {
        if (przeciwny_zegarowi(tab[indeks], tab[a], tab[xyz]))
        {
            xyz = a;
        }
    }
    indeks = xyz;
    //wlasciwe do{}while()
    do {
        hull[ile_juz_w_otoczce] = tab[indeks];
        ile_juz_w_otoczce++;
        xyz = (indeks + 1) % ilosc; //czy ta linijka jest potrzebna?
        for (a = 0; a < ilosc; a++)
        {
            if (przeciwny_zegarowi(tab[indeks], tab[a], tab[xyz]))
            {
                xyz = a;
            }
        }
        indeks = xyz;
    } while (!(tab[indeks] == hull[0]));
    return ile_juz_w_otoczce;
}

bool przeciwny_zegarowi(Lampion a, Lampion b, Lampion c)
{
    long long iloczyn_wektorowy = 0;
    iloczyn_wektorowy = (long long)(b.y - a.y) * (long long)(c.x - b.x) - (long long)(b.x - a.x) * (long long)(c.y - b.y);
    if (iloczyn_wektorowy < 0) { return true; }
    else { return false; }
}
