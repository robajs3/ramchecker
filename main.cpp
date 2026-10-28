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
            cin.ignore(10000, '\n');   // zjedz Enter po liczbie
            return x;
        }
        cout << "Bledna wartosc! Dozwolony zakres: " << min << "-" << max << "\n";
        cin.clear();                    // wyczysc flage bledu
        cin.ignore(10000, '\n');        // wyrzuc smieci z bufora
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

void dodaj(string nazwy[], int porty[], double ramy[], int& liczba) {
    if (liczba >= MAX) {                       // walidacja przepelnienia
        cout << "Baza pelna (" << MAX << " wpisow)!\n";
        return;
    }
    cout << "Nazwa uslugi: ";
    getline(cin, nazwy[liczba]);               // getline - nazwa moze miec spacje
    porty[liczba] = wczytajInt("Port (1-65535): ", 1, 65535);
    ramy[liczba] = wczytajDodatni("RAM w MB (>0): ");
    liczba++;                                  // licznik rosnie dopiero po komplecie danych
    cout << "Dodano.\n";
}

void wypisz(const string nazwy[], const int porty[], const double ramy[], int liczba) {
    if (liczba == 0) {
        cout << "Baza jest pusta.\n";
        return;
    }
    cout << left << setw(4) << "Lp" << setw(25) << "Nazwa"
         << setw(8) << "Port" << "RAM [MB]\n";
    cout << string(50, '-') << "\n";
    for (int i = 0; i < liczba; i++) {
        cout << left << setw(4) << i + 1 << setw(25) << nazwy[i]
             << setw(8) << porty[i] << fixed << setprecision(1) << ramy[i] << "\n";
    }
}

double sumaRam(const double ramy[], int liczba) {
    double suma = 0;
    for (int i = 0; i < liczba; i++) suma += ramy[i];
    return suma;
}

int ileNaPortachPowyzej(const int porty[], int liczba, int prog) {
    int licznik = 0;
    for (int i = 0; i < liczba; i++)
        if (porty[i] > prog) licznik++;
    return licznik;
}

int main() {
    string nazwy[MAX];
    int porty[MAX];
    double ramy[MAX];
    int liczba = 0;

    int opcja;
    do {
        cout << "\n1. Dodaj  2. Wypisz  3. Statystyki  0. Wyjscie\n";
        opcja = wczytajInt("Wybor: ", 0, 3);
        if (opcja == 1) dodaj(nazwy, porty, ramy, liczba);
        else if (opcja == 2) wypisz(nazwy, porty, ramy, liczba);
        else if (opcja == 3) {
            cout << "Suma RAM: " << sumaRam(ramy, liczba) << " MB\n";
            cout << "Uslug na portach > 1024: " << ileNaPortachPowyzej(porty, liczba, 1024) << "\n";
        }
    } while (opcja != 0);

    return 0;
}
