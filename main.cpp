#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

const int MAX = 100;

// Wczytuje int z zakresu [min, max]; pyta do skutku
int wczytajInt(const string& komunikat, int min, int max) {
    int x;
    while (true) {
        cout << komunikat;
        if (cin >> x && x >= min && x <= max) {
            cin.ignore(10000, '\n');
            return x;
        }
        cout << "Bledna wartosc! Dozwolony zakres: " << min << "-" << max << "\n";
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

// Wczytuje double > 0
double wczytajDodatni(const string& komunikat) {
    double x;
    while (true) {
        cout << komunikat;
        if (cin >> x && x > 0) {
            cin.ignore(10000, '\n');
            return x;
        }
        cout << "Wartosc musi byc wieksza od zera!\n";
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

// Tablice i licznik przekazane WYLACZNIE przez wskazniki
void dodaj(string* nazwy, int* porty, double* ramy, int* liczba) {
    if (*liczba >= MAX) {
        cout << "Baza pelna (" << MAX << " wpisow)!\n";
        return;
    }
    cout << "Nazwa uslugi: ";
    getline(cin, *(nazwy + *liczba));
    *(porty + *liczba) = wczytajInt("Port (1-65535): ", 1, 65535);
    *(ramy + *liczba) = wczytajDodatni("RAM w MB (>0): ");
    (*liczba)++;   // nawiasy sa konieczne! *liczba++ przesunieloby wskaznik, a nie wartosc
    cout << "Dodano.\n";
}

void wypisz(const string* nazwy, const int* porty, const double* ramy, int liczba) {
    if (liczba == 0) {
        cout << "Baza jest pusta.\n";
        return;
    }
    cout << left << setw(4) << "Lp" << setw(25) << "Nazwa"
         << setw(8) << "Port" << "RAM [MB]\n";
    cout << string(50, '-') << "\n";
    for (int i = 0; i < liczba; i++) {
        cout << left << setw(4) << i + 1 << setw(25) << *(nazwy + i)
             << setw(8) << *(porty + i) << fixed << setprecision(1)
             << *(ramy + i) << "\n";
    }
}

// --- ARYTMETYKA WSKAZNIKOWA: *(ramy + i) zamiast ramy[i] ---

double sumaRam(const double* ramy, int liczba) {
    double suma = 0;
    for (int i = 0; i < liczba; i++)
        suma += *(ramy + i);
    return suma;
}

double sredniRam(const double* ramy, int liczba) {
    if (liczba == 0) return 0;
    return sumaRam(ramy, liczba) / liczba;
}

// Zwraca indeks uslugi z najwiekszym RAM-em (lub -1 dla pustej bazy)
int indeksNajwiekszegoRamu(const double* ramy, int liczba) {
    if (liczba == 0) return -1;
    int idxMax = 0;
    for (int i = 1; i < liczba; i++) {
        if (*(ramy + i) > *(ramy + idxMax))
            idxMax = i;
    }
    return idxMax;
}

int ileNaPortachPowyzej(const int* porty, int liczba, int prog) {
    int licznik = 0;
    for (int i = 0; i < liczba; i++)
        if (*(porty + i) > prog) licznik++;
    return licznik;
}

int main() {
    // Wszystko lokalnie w main - zero zmiennych globalnych
    string nazwy[MAX];
    int porty[MAX];
    double ramy[MAX];
    int liczba = 0;

    int opcja;
    do {
        cout << "\n1. Dodaj  2. Wypisz  3. Statystyki  0. Wyjscie\n";
        // szeroki zakres, zeby "nieistniejaca opcja" (np. 7) dotarla do default
        opcja = wczytajInt("Wybor: ", -1000000, 1000000);

        switch (opcja) {
            case 1:
                dodaj(nazwy, porty, ramy, &liczba);   // &liczba - adres licznika z main
                break;
            case 2:
                wypisz(nazwy, porty, ramy, liczba);
                break;
            case 3: {
                if (liczba == 0) {
                    cout << "Baza jest pusta.\n";
                    break;
                }
                int idx = indeksNajwiekszegoRamu(ramy, liczba);
                cout << fixed << setprecision(1);
                cout << "Suma RAM:    " << sumaRam(ramy, liczba) << " MB\n";
                cout << "Srednia RAM: " << sredniRam(ramy, liczba) << " MB\n";
                cout << "Najwiecej RAM: " << *(nazwy + idx) << " ("
                     << *(ramy + idx) << " MB)\n";
                cout << "Uslug na portach > 1024: "
                     << ileNaPortachPowyzej(porty, liczba, 1024) << "\n";
                break;
            }
            case 0:
                cout << "Koniec programu.\n";
                break;
            default:
                cout << "Nie ma takiej opcji! Wybierz 0-3.\n";
                break;
        }
    } while (opcja != 0);

    return 0;
}
