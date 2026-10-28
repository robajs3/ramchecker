#include <iostream>
#include <iomanip>
using namespace std;

const int MAX = 100;
const int DL_NAZWY = 50;   // rozmiar tablicy znakow (49 znakow + '\0')

// Jeden rekord bazy - zastepuje trzy tablice rownolegle
struct Usluga {
    char nazwa[DL_NAZWY];
    int port;
    double ramMB;
};

// ---------- WLASNE FUNKCJE TEKSTOWE (bez <string.h>) ----------

// Dlugosc C-stringa: idziemy wskaznikiem az do '\0'
int mojaDlugosc(const char* s) {
    const char* p = s;
    while (*p != '\0') p++;
    return static_cast<int>(p - s);   // roznica wskaznikow = liczba znakow
}

// Zamiana duzej litery na mala (zeby wyszukiwanie nie zalezalo od wielkosci liter)
char malaLitera(char c) {
    if (c >= 'A' && c <= 'Z') return c + ('a' - 'A');
    return c;
}

// Czy "fragment" wystepuje gdziekolwiek w "tekst"?
// Dla kazdej pozycji startowej p w tekscie porownujemy kolejne znaki fragmentu.
bool zawiera(const char* tekst, const char* fragment) {
    if (*fragment == '\0') return true;          // pusty fragment pasuje do wszystkiego
    for (const char* p = tekst; *p != '\0'; p++) {
        const char* a = p;
        const char* b = fragment;
        while (*a != '\0' && *b != '\0' && malaLitera(*a) == malaLitera(*b)) {
            a++;
            b++;
        }
        if (*b == '\0') return true;             // doszlismy do konca fragmentu = jest dopasowanie
    }
    return false;
}

// ---------- WCZYTYWANIE I WALIDACJA ----------

// Wczytuje int z zakresu [min, max]; pyta do skutku.
// Na koncu danych wejsciowych (EOF) zwraca 0, zeby program sie nie zapetlil.
int wczytajInt(const char* komunikat, int min, int max) {
    int x;
    while (true) {
        cout << komunikat;
        if (cin >> x && x >= min && x <= max) {
            cin.ignore(10000, '\n');
            return x;
        }
        if (cin.eof()) return 0;
        cout << "Bledna wartosc! Dozwolony zakres: " << min << "-" << max << "\n";
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

// Wczytuje double > 0
double wczytajDodatni(const char* komunikat) {
    double x;
    while (true) {
        cout << komunikat;
        if (cin >> x && x > 0) {
            cin.ignore(10000, '\n');
            return x;
        }
        if (cin.eof()) return 0;
        cout << "Wartosc musi byc wieksza od zera!\n";
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

// Wczytuje niepusty tekst ze spacjami do tablicy znakow
void wczytajTekst(const char* komunikat, char* bufor, int rozmiar) {
    while (true) {
        cout << komunikat;
        cin.getline(bufor, rozmiar);
        if (cin.eof()) return;
        if (cin.fail()) {                 // linia byla dluzsza niz bufor
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Tekst skrocono do " << rozmiar - 1 << " znakow.\n";
            return;
        }
        if (mojaDlugosc(bufor) > 0) return;
        cout << "Tekst nie moze byc pusty!\n";
    }
}

// ---------- OPERACJE NA BAZIE (tablica i licznik przez wskazniki) ----------

void dodaj(Usluga* baza, int* liczba) {
    if (*liczba >= MAX) {
        cout << "Baza pelna (" << MAX << " wpisow)!\n";
        return;
    }
    Usluga* nowa = baza + *liczba;          // adres pierwszego wolnego rekordu
    wczytajTekst("Nazwa uslugi: ", nowa->nazwa, DL_NAZWY);
    nowa->port = wczytajInt("Port (1-65535): ", 1, 65535);
    nowa->ramMB = wczytajDodatni("RAM w MB (>0): ");
    if (cin.eof()) return;                  // niepelny rekord nie wchodzi do bazy
    (*liczba)++;
    cout << "Dodano.\n";
}

void wypiszNaglowek() {
    cout << left << setw(4) << "Lp" << setw(25) << "Nazwa"
         << setw(8) << "Port" << "RAM [MB]\n";
    for (int i = 0; i < 50; i++) cout << '-';
    cout << "\n";
}

void wypiszRekord(const Usluga* u, int lp) {
    cout << left << setw(4) << lp << setw(25) << u->nazwa
         << setw(8) << u->port << fixed << setprecision(1) << u->ramMB << "\n";
}

void wypisz(const Usluga* baza, int liczba) {
    if (liczba == 0) {
        cout << "Baza jest pusta.\n";
        return;
    }
    wypiszNaglowek();
    for (int i = 0; i < liczba; i++)
        wypiszRekord(baza + i, i + 1);
}

void szukaj(const Usluga* baza, int liczba) {
    char fragment[DL_NAZWY];
    wczytajTekst("Szukany fragment nazwy: ", fragment, DL_NAZWY);
    int znalezione = 0;
    for (int i = 0; i < liczba; i++) {
        if (zawiera((baza + i)->nazwa, fragment)) {
            if (znalezione == 0) wypiszNaglowek();   // naglowek tylko przy pierwszym trafieniu
            wypiszRekord(baza + i, i + 1);
            znalezione++;
        }
    }
    if (znalezione == 0) cout << "Nic nie znaleziono.\n";
    else cout << "Znaleziono: " << znalezione << "\n";
}

// ---------- STATYSTYKI (arytmetyka wskaznikowa: (baza + i)->pole) ----------

double sumaRam(const Usluga* baza, int liczba) {
    double suma = 0;
    for (int i = 0; i < liczba; i++)
        suma += (baza + i)->ramMB;
    return suma;
}

double sredniRam(const Usluga* baza, int liczba) {
    if (liczba == 0) return 0;
    return sumaRam(baza, liczba) / liczba;
}

// Indeks uslugi z najwiekszym RAM-em (-1 dla pustej bazy)
int indeksNajwiekszegoRamu(const Usluga* baza, int liczba) {
    if (liczba == 0) return -1;
    int idxMax = 0;
    for (int i = 1; i < liczba; i++)
        if ((baza + i)->ramMB > (baza + idxMax)->ramMB)
            idxMax = i;
    return idxMax;
}

int ileNaPortachPowyzej(const Usluga* baza, int liczba, int prog) {
    int licznik = 0;
    for (int i = 0; i < liczba; i++)
        if ((baza + i)->port > prog) licznik++;
    return licznik;
}

int main() {
    // Wszystko lokalnie w main - zero zmiennych globalnych
    Usluga baza[MAX];
    int liczba = 0;

    int opcja;
    do {
        cout << "\n1. Dodaj  2. Wypisz  3. Szukaj  4. Statystyki  0. Wyjscie\n";
        opcja = wczytajInt("Wybor: ", -1000000, 1000000);

        switch (opcja) {
            case 1:
                dodaj(baza, &liczba);
                break;
            case 2:
                wypisz(baza, liczba);
                break;
            case 3:
                szukaj(baza, liczba);
                break;
            case 4: {
                if (liczba == 0) {
                    cout << "Baza jest pusta.\n";
                    break;
                }
                int idx = indeksNajwiekszegoRamu(baza, liczba);
                cout << fixed << setprecision(1);
                cout << "Suma RAM:    " << sumaRam(baza, liczba) << " MB\n";
                cout << "Srednia RAM: " << sredniRam(baza, liczba) << " MB\n";
                cout << "Najwiecej RAM: " << (baza + idx)->nazwa << " ("
                     << (baza + idx)->ramMB << " MB)\n";
                cout << "Uslug na portach > 1024: "
                     << ileNaPortachPowyzej(baza, liczba, 1024) << "\n";
                break;
            }
            case 0:
                cout << "Koniec programu.\n";
                break;
            default:
                cout << "Nie ma takiej opcji! Wybierz 0-4.\n";
                break;
        }
    } while (opcja != 0 && !cin.eof());

    return 0;
}
